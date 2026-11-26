/* the GCC build's PVM loads. each checks an archive whole before Ninja reads
 * it. a load keeps the handles of the texture records it acquires */
#include <kamui.h>
#include <ninja.h>
#include <nj_texlist.h>
#include <shinobi.h>
#include <stddef.h>
#include <string.h>

#include "engine/dc/pvm.h"

/* the most textures the check takes in one archive */
#define PVM_CAPACITY 512
/* the largest archive the check takes, grown or not, the largest Sint32.
 * njMemRead and njMemFseek, which read an archive in memory, take Sint32
 * sizes and offsets */
#define ARCHIVE_MAX 0x7fffffffU
/* a PVMH chunk's name, as nj_texlist.h's iff_ names are */
#define iff_PVMH 0x484d5650U
/* the bytes of a chunk's name, and of its name and size */
#define CHUNK_NAME 4
#define CHUNK_HEAD 8
/* the offset of field in a chunk's data. record is the chunk after its name,
 * as nj_texlist.h declares it, whose first word is the chunk's size */
#define DATA_AT(record, field) (offsetof(struct record, field) - sizeof(Uint32))
/* a PVMH chunk's data is its status and its count, a halfword each, then its
 * entries */
#define STATUS_SIZE 2
#define PVMH_HEAD 4
/* an entry is its texture's number, then a field for each of these bits the
 * status sets. the fields follow in the bits' order */
#define PVMH_NAME 1U
#define PVMH_CATEGORYCODE 2U
#define PVMH_ENTRYINFO 4U
#define PVMH_GLOBALINDEX 8U
/* the bytes of an entry's fields. its name takes PVM_NAME_SIZE */
#define NUMBER_SIZE 2
#define CATEGORYCODE_SIZE 2
#define ENTRYINFO_SIZE 2
#define GLOBALINDEX_SIZE 4
/* the bits of a name's attr that hold its entry's number */
#define NUMBER_MASK 0xffffU
/* the bytes of a PVRT chunk's header, before its texels */
#define PVRT_HEAD ((Uint32)sizeof(NJS_PVRHEADERDATA))
/* a texture's sides, as njCheckTextureFormatLow takes them. each is a power
 * of two from MIN_SIDE to MAX_SIDE, or to SMALLVQ_MAX_SIDE for a small VQ
 * texture */
#define MIN_SIDE 8
#define MAX_SIDE 1024
#define SMALLVQ_MAX_SIDE 64
/* the palette's colors, and those of each of its banks */
#define PALETTE_COLORS 1024
#define BANK_COLORS 16
/* the bits of a texture's format above its type, which must be 0 */
#define FORMAT_UNUSED (~(Uint32)(NJD_TEXFMT_TYPE_MASK | NJD_TEXFMT_COLOR_MASK))

/* a list texture_load_pvm_memory filled, named by its address. handles holds
 * the count texture records the load acquired for the list. a list can lie in
 * an overlay since replaced, or outlive a reset of texture memory. so a
 * release takes the handles from here, never from the list */
struct pvm_owner {
    NJS_TEXLIST *target;
    struct pvm_owner *next;
    Uint32 count;
    Uint32 handles[];
};
/* the lists that own texture records, newest first */
static struct pvm_owner *owners;

static Uint32 le16(const Uint8 *p)
{
    return p[0] | ((Uint32)p[1] << 8);
}

static Uint32 le32(const Uint8 *p)
{
    return le16(p) | (le16(p + 2) << 16);
}

static Bool texture_side_valid(Uint32 side)
{
    return side >= MIN_SIDE && side <= MAX_SIDE && (side & (side - 1)) == 0;
}

/* the size a PVRT chunk's data must have. that is its own size, grown where
 * Ninja's copy of the surface would read past it. it returns 0 for a texture
 * the check refuses. p is the data, past the chunk's name and size, and size
 * its bytes */
static Uint32 texture_chunk_size(const Uint8 *p, Uint32 size)
{
    NJS_TEXSURFACE surface;
    Uint32 width, height, format, type, encoded;

    if (size < PVRT_HEAD) {
        return 0;
    }
    format = le32(p + DATA_AT(pvrt_chunk, header.nTextureType));
    type = format & NJD_TEXFMT_TYPE_MASK;
    width = le16(p + DATA_AT(pvrt_chunk, header.nWidth));
    height = le16(p + DATA_AT(pvrt_chunk, header.nHeight));
    if ((format & FORMAT_UNUSED) != 0 || !texture_side_valid(width) ||
        !texture_side_valid(height) ||
        ((type == NJD_TEXFMT_SMALLVQ || type == NJD_TEXFMT_SMALLVQ_MM) &&
         (width > SMALLVQ_MAX_SIDE || height > SMALLVQ_MAX_SIDE))) {
        return 0;
    }
    if (njCheckTextureFormatLow(&surface, width, height, format) !=
        KMSTATUS_SUCCESS) {
        return 0;
    }
    /* njCalcTextureSize returns 0 for NJD_TEXFMT_TWIDDLED_MM_DMA, a type it
     * doesn't know */
    encoded = type == NJD_TEXFMT_TWIDDLED_MM_DMA
                  ? surface.TextureSize
                  : njCalcTextureSize(width, height, format);
    if (encoded == 0 || encoded > size - PVRT_HEAD) {
        return 0;
    }
    /* Ninja copies the surface's whole TextureSize. in some shipped archives a
     * VQ mipmap's chunk ends at its last texel, short of that */
    return surface.TextureSize + PVRT_HEAD > size
               ? surface.TextureSize + PVRT_HEAD
               : size;
}

/* whether the check takes a PVPL chunk's data, p, of size bytes. its count
 * colors, one or more, must fit the palette from offset colors past the start
 * of bank. they must fit the chunk too. a color is a Uint32 in
 * NJD_TEXFMT_ARGB_8888, and a Uint16 in any other mode */
static Bool palette_chunk_valid(const Uint8 *p, Uint32 size)
{
    Uint32 mode, bank, offset, count;
    if (size < PVPL_HEADER_REST) {
        return FALSE;
    }
    mode = le16(p + DATA_AT(pvpl_header, mode));
    bank = le16(p + DATA_AT(pvpl_header, bank));
    offset = le16(p + DATA_AT(pvpl_header, offset));
    count = le16(p + DATA_AT(pvpl_header, count));
    return count != 0 && bank < PALETTE_COLORS / BANK_COLORS &&
           offset < PALETTE_COLORS - BANK_COLORS * bank &&
           count <= PALETTE_COLORS - BANK_COLORS * bank - offset &&
           count <= (size - PVPL_HEADER_REST) / (mode == NJD_TEXFMT_ARGB_8888
                                                     ? sizeof(Uint32)
                                                     : sizeof(Uint16));
}

static void put_le32(Uint8 *p, Uint32 value)
{
    p[0] = (Uint8)value;
    p[1] = (Uint8)(value >> 8);
    p[2] = (Uint8)(value >> 16);
    p[3] = (Uint8)(value >> 24);
}

/* check the archive, and return its count of textures, or 0 for one the check
 * refuses. with a count, *upload_size is the size of a copy that grows each
 * PVRT chunk as texture_chunk_size says. it is 0 where no chunk grows.
 * output, if not null, then holds that copy */
static Uint32 inspect_pvm(const void *archive, Uint32 size, Uint32 *upload_size,
                          Uint8 *output)
{
    const Uint8 *p = archive;
    Uint8 used[PVM_CAPACITY / 8] = {0};
    Uint32 pvmh_size, status, count, stride, i, number, textures, at, name;
    Uint32 bytes;
    const Uint8 *entry;
    Uint32 written, stored;

    if (p == NULL || size < CHUNK_HEAD + PVMH_HEAD || size > ARCHIVE_MAX ||
        le32(p) != iff_PVMH) {
        return 0;
    }
    pvmh_size = le32(p + CHUNK_NAME);
    status = le16(p + CHUNK_HEAD);
    count = le16(p + CHUNK_HEAD + STATUS_SIZE);
    stride = NUMBER_SIZE + ((status & PVMH_NAME) ? PVM_NAME_SIZE : 0) +
             ((status & PVMH_CATEGORYCODE) ? CATEGORYCODE_SIZE : 0) +
             ((status & PVMH_ENTRYINFO) ? ENTRYINFO_SIZE : 0) +
             ((status & PVMH_GLOBALINDEX) ? GLOBALINDEX_SIZE : 0);
    if (count == 0 || count > PVM_CAPACITY || pvmh_size < PVMH_HEAD ||
        pvmh_size > size - CHUNK_HEAD ||
        count > (pvmh_size - PVMH_HEAD) / stride) {
        return 0;
    }
    entry = p + CHUNK_HEAD + PVMH_HEAD;
    for (i = 0; i < count; i++, entry += stride) {
        number = le16(entry);
        if (number >= count || (used[number / 8] & (1U << (number % 8))) ||
            ((status & PVMH_GLOBALINDEX) &&
             le32(entry + stride - GLOBALINDEX_SIZE) == TEXMEM_FREE)) {
            return 0;
        }
        used[number / 8] |= (Uint8)(1U << (number % 8));
    }
    at = pvmh_size + CHUNK_HEAD;
    written = at;
    if (output != NULL) {
        memcpy(output, p, at);
    }
    textures = 0;
    while (at < size) {
        if (size - at < CHUNK_HEAD) {
            return 0;
        }
        name = le32(p + at);
        bytes = le32(p + at + CHUNK_NAME);
        at += CHUNK_HEAD;
        if (bytes > size - at) {
            return 0;
        }
        stored = bytes;
        if (name == iff_PVRT) {
            stored = texture_chunk_size(p + at, bytes);
            if (stored == 0 || textures >= count) {
                return 0;
            }
            textures++;
        } else if (name == iff_PVPL && !palette_chunk_valid(p + at, bytes)) {
            return 0;
        }
        if (written > ARCHIVE_MAX - CHUNK_HEAD ||
            stored > ARCHIVE_MAX - written - CHUNK_HEAD) {
            return 0;
        }
        if (output != NULL) {
            memcpy(output + written, p + at - CHUNK_HEAD, bytes + CHUNK_HEAD);
            put_le32(output + written + CHUNK_NAME, stored);
            memset(output + written + CHUNK_HEAD + bytes, 0, stored - bytes);
        }
        written += CHUNK_HEAD + stored;
        at += bytes;
    }
    *upload_size = written != at ? written : 0;
    return textures == count ? count : 0;
}

Uint32 texture_pvm_count(const void *archive, Uint32 size)
{
    Uint32 upload_size;
    return inspect_pvm(archive, size, &upload_size, NULL);
}

/* grow, where it lies, an archive inspect_pvm has taken: each PVRT chunk
 * takes the size texture_chunk_size says, with zeros past its own bytes. p
 * holds the archive's size bytes in a room of capacity, which the grown
 * archive fits. the chunks up to the first that grows stay. the rest wait at
 * the room's end, and each comes back to follow the chunk before it. the
 * chunks before one grow by no more than the room has past the archive, so
 * none comes back over a byte that still waits */
static void grow_pvm(Uint8 *p, Uint32 size, Uint32 capacity)
{
    Uint8 *from = p;
    Uint32 at = le32(p + CHUNK_NAME) + CHUNK_HEAD;
    Uint32 written = at;
    Uint32 bytes, stored;

    while (at < size) {
        bytes = le32(from + at + CHUNK_NAME);
        stored = le32(from + at) == iff_PVRT
                     ? texture_chunk_size(from + at + CHUNK_HEAD, bytes)
                     : bytes;
        if (from != p) {
            memmove(p + written, from + at, CHUNK_HEAD + bytes);
        }
        at += CHUNK_HEAD + bytes;
        if (stored != bytes) {
            if (from == p) {
                from = p + (capacity - size);
                memmove(from + at, p + at, size - at);
            }
            put_le32(p + written + CHUNK_NAME, stored);
            memset(p + written + CHUNK_HEAD + bytes, 0, stored - bytes);
        }
        written += CHUNK_HEAD + stored;
    }
}

static struct pvm_owner **owner_of(const NJS_TEXLIST *target)
{
    struct pvm_owner **slot = &owners;
    while (*slot != NULL && (*slot)->target != target) {
        slot = &(*slot)->next;
    }
    return slot;
}

static void clear_handles(NJS_TEXLIST *target)
{
    Uint32 i;

    if (target == NULL || target->textures == NULL ||
        target->nbTexture > PVM_CAPACITY) {
        return;
    }
    for (i = 0; i < target->nbTexture; i++) {
        target->textures[i].texaddr = 0;
        target->textures[i].attr = 0;
    }
}

Bool texture_release_owned(NJS_TEXLIST *target)
{
    struct pvm_owner **slot = owner_of(target);
    struct pvm_owner *owner = *slot;
    Uint32 i;

    if (owner == NULL) {
        return FALSE;
    }
    *slot = owner->next;
    for (i = 0; i < owner->count; i++) {
        njReleaseTextureLN(owner->handles[i]);
    }
    syFree(owner);
    clear_handles(target);
    return TRUE;
}

void texture_forget_all_pvms(void)
{
    struct pvm_owner *owner;
    while (owners != NULL) {
        owner = owners;
        owners = owner->next;
        syFree(owner);
    }
}

void texture_discard_pvm(NJS_TEXLIST *target)
{
    if (!texture_release_owned(target)) {
        clear_handles(target);
    }
}

Bool texture_load_pvm_memory(const void *archive, Uint32 size,
                             NJS_TEXLIST *target)
{
    NJS_TEXLIST staged;
    NJS_TEXNAME *entries;
    struct pvm_owner *owner;
    Uint32 count, i, failed, failed_number, upload_size;
    Uint8 *upload = NULL;
    char *names;

    count = inspect_pvm(archive, size, &upload_size, NULL);
    if (count == 0 || target == NULL || target->textures == NULL ||
        target->nbTexture != count) {
        texture_discard_pvm(target);
        return FALSE;
    }
    owner = syMalloc(sizeof(*owner) + count * sizeof(owner->handles[0]));
    entries = syCalloc(count, sizeof(*entries) + PVM_NAME_SIZE);
    if (upload_size != 0) {
        upload = syMalloc(upload_size);
    }
    if (owner == NULL || entries == NULL ||
        (upload_size != 0 && upload == NULL)) {
        syFree(owner);
        syFree(entries);
        syFree(upload);
        texture_discard_pvm(target);
        return FALSE;
    }
    if (upload != NULL) {
        inspect_pvm(archive, size, &upload_size, upload);
        archive = upload;
    }
    names = (char *)(entries + count);
    staged.nbTexture = 0;
    njSetPvmTextureList(&staged, entries, names, count);
    if (njLoadTexturePvmMemory((Uint8 *)archive, &staged) < 0) {
        /* Ninja gives each name its entry's global index before it loads any
         * texture. it then loads the textures in the order of their numbers.
         * so a name holds a texture record only where its number lies below
         * the failed name's */
        failed = njGetTextureErrStat()->n;
        if (failed < count) {
            failed_number = entries[failed].attr & NUMBER_MASK;
            for (i = 0; i < count; i++) {
                if ((entries[i].attr & NUMBER_MASK) < failed_number) {
                    njReleaseTextureLN(entries[i].texaddr);
                }
            }
        }
        syFree(upload);
        syFree(entries);
        syFree(owner);
        texture_discard_pvm(target);
        return FALSE;
    }
    texture_discard_pvm(target);
    owner->count = count;
    for (i = 0; i < count; i++) {
        owner->handles[i] = entries[i].texaddr;
        target->textures[i].texaddr = entries[i].texaddr;
        target->textures[i].attr = entries[i].attr;
    }
    syFree(upload);
    owner->target = target;
    owner->next = owners;
    owners = owner;
    syFree(entries);
    return TRUE;
}

Bool texture_load_pvm_buffer(void *buffer, Uint32 size, Uint32 capacity,
                             NJS_TEXLIST *target)
{
    Uint32 upload_size;

    /* the heap may lack a whole archive's bytes: an event loads with some
     * 2.3 MB of it free, and the largest of the disc's archives that grow
     * takes 2.9 MB. so the archive grows in its room where the room holds
     * it */
    if (size <= capacity &&
        inspect_pvm(buffer, size, &upload_size, NULL) != 0 &&
        upload_size != 0 && upload_size <= capacity) {
        grow_pvm(buffer, size, capacity);
        size = upload_size;
    }
    return texture_load_pvm_memory(buffer, size, target);
}
