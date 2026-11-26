/* the AHX bit reader's pool and its reads (ahxbsr.h) */
#include <adx_errs.h>
#include <ahxbsr.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

#define READERS 8
#define WORD_BITS 32
#define WORD_BYTES 4
#define BYTE_BITS 8
#define SYNC_BITS 12
#define FRAME_SYNC 0xfff
#define END_MARK 0x8001000c
/* after its first SYNC_BITS, ahxbsr_find_frame reads on this many at a time */
#define FRAME_STEP_BITS 4

struct ahxbsr_obj {
    Sint8 used;
    SJ joint;
    Uint32 bits; /* its unread bits, at its low end. a read takes the highest */
    Sint32 available; /* the count of bits' unread bits */
    Sint32 position;
    Sint32 capacity;
    Sint32 chunk_size;
    SJCK chunk;
    Sint32 unread; /* chunk's bytes from next on, which bits hasn't taken */
    Uint8 *next;
};

static struct ahxbsr_obj readers[READERS];
static const char *build =
    "\nCRI AHXBSR Ver 1.00C Build:Apr 11 2001 10:42:18\n";
static Sint32 init_count = 0;
Sint32 ahxbsr_read_count = 0;
static Uint32 low_mask[WORD_BITS + 1] = {
    0,          0x1,        0x3,        0x7,       0xf,       0x1f,
    0x3f,       0x7f,       0xff,       0x1ff,     0x3ff,     0x7ff,
    0xfff,      0x1fff,     0x3fff,     0x7fff,    0xffff,    0x1ffff,
    0x3ffff,    0x7ffff,    0xfffff,    0x1fffff,  0x3fffff,  0x7fffff,
    0xffffff,   0x1ffffff,  0x3ffffff,  0x7ffffff, 0xfffffff, 0x1fffffff,
    0x3fffffff, 0x7fffffff, 0xffffffff,
};

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ahxbsr_fill_word(AHXBSR reader);
Uint32 ahxbsr_make_mask(Sint32 width);
void ahxbsr_fill_word_2(AHXBSR reader);
Bool ahxbsr_find_code(AHXBSR reader, Uint32 code, Sint32 width);
/* NOLINTEND(misc-use-internal-linkage) */

void ahxbsr_init(void)
{
    if (init_count == 0) {
        memset(readers, 0, sizeof(readers));
    }
    init_count++;
}

void ahxbsr_finish(void)
{
    init_count--;
    if (init_count == 0) {
        memset(readers, 0, sizeof(readers));
    }
}

AHXBSR ahxbsr_create(SJ sj)
{
    Sint32 i;
    AHXBSR reader;

    for (i = 0; i < READERS; i++) {
        if (!readers[i].used) {
            break;
        }
    }
    if (i >= READERS) {
        return NULL;
    }
    reader = &readers[i];
    memset(reader, 0, sizeof(*reader));
    reader->joint = sj;
    /* @hack redundant: position, unread, chunk and next cleared, which the
     * memset has cleared */
    reader->position = 0;
    reader->capacity =
        SJ_GetNumData(sj, SJ_LIN_DATA) + SJ_GetNumData(sj, SJ_LIN_FREE);
    reader->chunk_size = reader->capacity / 4;
    reader->unread = 0;
    reader->chunk.data = NULL;
    reader->chunk.len = 0;
    reader->next = NULL;
    reader->used = TRUE;
    return reader;
}

void ahxbsr_destroy(AHXBSR reader)
{
    if (reader) {
        memset(reader, 0, sizeof(*reader));
    }
}

#define AHXBSR_FILL ahxbsr_fill_word
#include "../sofdec/ahxbsr_fill_impl.h"
#undef AHXBSR_FILL

Uint32 ahxbsr_make_mask(Sint32 width)
{
    Sint32 bytes;
    Sint32 trailing;
    Sint32 i;
    Uint32 mask = 0;

    bytes = width / BYTE_BITS;
    for (i = 0; i < bytes; i++) {
        mask <<= BYTE_BITS;
        mask |= 0xff;
    }
    trailing = width % BYTE_BITS;
    for (i = 0; i < trailing; i++) {
        mask <<= 1;
        mask |= 1;
    }
    return mask;
}

#define AHXBSR_FILL ahxbsr_fill_word_2
#include "../sofdec/ahxbsr_fill_impl.h"
#undef AHXBSR_FILL

static void refill(AHXBSR reader)
{
    SJCK chunk;
    SJCK tail;
    Sint32 room;
    Sint32 bytes;
    /* @hack redundant: bits declared register, where a plain local would do */
    register Uint32 bits;
    Uint8 *next;

    room = (WORD_BITS - reader->available) / BYTE_BITS;
    if (reader->unread < WORD_BYTES) {
        chunk = reader->chunk;
        if (chunk.len) {
            SJ_SplitChunk(&chunk, chunk.len - reader->unread, &chunk, &tail);
            SJ_PutChunk(reader->joint, SJ_LIN_FREE, &chunk);
            SJ_UngetChunk(reader->joint, SJ_LIN_DATA, &tail);
        }
        SJ_GetChunk(reader->joint, SJ_LIN_DATA, reader->chunk_size,
                    &reader->chunk);
        reader->next = (Uint8 *)reader->chunk.data;
        reader->unread = reader->chunk.len;
    }
    bytes = room < reader->unread ? room : reader->unread;
    /* @hack spelling: the shifts of each count of bytes written out, where a
     * loop over bytes would do */
    if (bytes == 3) {
        bits = reader->bits;
        next = reader->next;
        bits = (bits << BYTE_BITS) | *next++;
        bits = (bits << BYTE_BITS) | *next++;
        bits = (bits << BYTE_BITS) | *next++;
        reader->next = next;
        reader->bits = bits;
        reader->available += 3 * BYTE_BITS;
        reader->unread -= 3;
    } else if (bytes == 2) {
        bits = reader->bits;
        next = reader->next;
        bits = (bits << BYTE_BITS) | *next++;
        bits = (bits << BYTE_BITS) | *next++;
        reader->next = next;
        reader->bits = bits;
        reader->available += 2 * BYTE_BITS;
        reader->unread -= 2;
    } else if (bytes == 1) {
        bits = reader->bits;
        next = reader->next;
        bits = (bits << BYTE_BITS) | *next++;
        reader->next = next;
        reader->bits = bits;
        reader->available += BYTE_BITS;
        reader->unread--;
    } else if (bytes == 4) {
        bits = reader->bits;
        next = reader->next;
        bits = (bits << BYTE_BITS) | *next++;
        bits = (bits << BYTE_BITS) | *next++;
        bits = (bits << BYTE_BITS) | *next++;
        bits = (bits << BYTE_BITS) | *next++;
        reader->next = next;
        reader->bits = bits;
        reader->available += WORD_BITS;
        reader->unread -= WORD_BYTES;
    }
}

Uint32 AHXBSR_GetBitStm(AHXBSR reader, Sint32 width)
{
    Sint32 value;
    Uint32 mask;

    ahxbsr_read_count++;
    if (reader->available < width) {
        refill(reader);
    }
    if (width > reader->available) {
        reader->position += reader->available;
        reader->available = 0;
        ADXERR_CallErrFunc1(
            "E1031218:can not get bit stream data.(AHXBSR_GetBitStm)\n");
        return 0;
    }
    /* @hack type: bits shifted as a Sint32, where a Uint32 shift would do.
     * the mask, read first, drops the copies of the sign */
    mask = low_mask[width];
    value = (Sint32)reader->bits >> (reader->available - width);
    value &= mask;
    reader->available -= width;
    reader->position += width;
    return value;
}

Sint32 ahxbsr_position(AHXBSR reader)
{
    return reader->position;
}

Bool ahxbsr_empty(AHXBSR reader)
{
    if (!SJ_GetNumData(reader->joint, SJ_LIN_DATA) && !reader->available &&
        !reader->unread) {
        return TRUE;
    }
    return FALSE;
}

Bool ahxbsr_find_code(AHXBSR reader, Uint32 code, Sint32 width)
{
    Uint32 mask;
    Sint32 offset;
    Uint32 window;

    mask = low_mask[width];
    offset = ahxbsr_position(reader) % BYTE_BITS;
    if (offset) {
        AHXBSR_GetBitStm(reader, BYTE_BITS - offset);
    }
    window = AHXBSR_GetBitStm(reader, width);
    while ((window & mask) != code && !ahxbsr_empty(reader)) {
        window <<= BYTE_BITS;
        window |= AHXBSR_GetBitStm(reader, BYTE_BITS);
    }
    return !ahxbsr_empty(reader);
}

Sint32 ahxbsr_find_frame(AHXBSR reader)
{
    Uint32 mask;
    Sint32 offset;
    Uint32 window;

    mask = low_mask[SYNC_BITS];
    offset = ahxbsr_position(reader) % BYTE_BITS;
    if (offset) {
        AHXBSR_GetBitStm(reader, BYTE_BITS - offset);
    }
    window = AHXBSR_GetBitStm(reader, SYNC_BITS);
    while (!ahxbsr_empty(reader)) {
        if ((window & mask) == FRAME_SYNC) {
            return AHXBSR_SYNC;
        }
        if (window == END_MARK) {
            return AHXBSR_END;
        }
        window <<= FRAME_STEP_BITS;
        window |= AHXBSR_GetBitStm(reader, FRAME_STEP_BITS);
    }
    return AHXBSR_EMPTY;
}
