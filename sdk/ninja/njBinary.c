/* a file of chunks, read whole, and its chunks copied out one at a time */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports njSwapCNK, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* name with its four bytes reversed. a chunk's name, read from a file as a
 * little-endian word, then equals its iff_ constant */
Uint32 njSwapCNK(Uint32 name);
/* NOLINTEND(misc-use-internal-linkage) */

/* the chunks njReadBinary copies beside iff_NJCM and iff_NJBM. each name is
 * a chunk's four characters as a big-endian word. iff_POF0 names the chunk
 * that lists a copy's pointers */
#define iff_NJINFO 0x4e4a494e
#define iff_NJTL 0x4e4a544c
#define iff_NJLI 0x4e4a4c49
#define iff_NJCA 0x4e4a4341
#define iff_NMDM 0x4e4d444d
#define iff_NLIM 0x4e4c494d
#define iff_NCAM 0x4e43414d
#define iff_NSSM 0x4e53534d
#define iff_POF0 0x504f4630
/* the name njReadBinary gives a chunk it doesn't copy, '****' */
#define iff_UNKNOWN 0x2a2a2a2a
/* a chunk opens with its name, then its size, CHUNK_WORD bytes each.
 * CHUNK_HEAD is the two */
#define CHUNK_WORD 4
#define CHUNK_HEAD (2 * CHUNK_WORD)
/* a POF0 entry counts the words from one pointer to the next. the first
 * entry counts from the copy's start. the POF0_FORM bits of an entry's first
 * byte say how many bytes the count takes. the byte's POF0_COUNT bits are the
 * count's high bits */
#define POF0_FORM 0xc0
#define POF0_COUNT 0x3f
#define POF0_PAD 0x00 /* a byte that holds no entry */
#define POF0_BYTE 0x40 /* the count is POF0_COUNT's bits alone */
#define POF0_HALF 0x80 /* one more byte follows */
#define POF0_WORD 0xc0 /* three more bytes follow */

Sint8 *njOpenBinary(const char *fname)
{
    GDFS gdfs;
    Sint32 sectors;
    Sint8 *buf;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((gdfs = gdFsOpen(fname, NULL)) == NULL) {
        return NULL;
    }
    if (gdFsGetFileSctSize(gdfs, &sectors) != TRUE) {
        gdFsClose(gdfs);
        return NULL;
    }
    /* past the sectors, CHUNK_HEAD bytes keep a read of the next chunk's name
     * and size in the block */
    buf = syMalloc((sectors * GDD_FS_SCTSIZE) + CHUNK_HEAD);
    if (buf == NULL) {
        gdFsClose(gdfs);
        return NULL;
    }
    if (gdFsRead(gdfs, sectors, buf) != GDD_ERR_OK) {
        syFree(buf);
        gdFsClose(gdfs);
        return NULL;
    }
    gdFsClose(gdfs);
    return buf;
}

void njCloseBinary(Sint8 *wbuff)
{
    syFree(wbuff);
}

Uint32 njSwapCNK(Uint32 name)
{
    /* @hack spelling: swapped, a local for the result, where the expression
     * returned would do */
    Uint32 swapped;

    swapped = ((name & 0x000000ff) << 24) | ((name & 0x0000ff00) << 8) |
              ((name & 0x00ff0000) >> 8) | ((name & 0xff000000) >> 24);
    return swapped;
}

void *njReadBinary(Sint8 *wbuff, Uint32 *fpos, Uint32 *ptype)
{
    Uint32 size;
    Uint32 name;
    Uint8 head;
    Uint8 byte1;
    Uint8 byte2;
    Uint8 byte3;
    Sint8 *p;
    Uint8 *copy;
    Uint32 offset;
    Uint32 words;
    Uint32 *slot;

    p = wbuff + *fpos;
    offset = 0;
    njMemCopy(&name, p, CHUNK_WORD);
    p += CHUNK_WORD;
    njMemCopy(&size, p, CHUNK_WORD);
    *fpos += CHUNK_HEAD;
    p += CHUNK_WORD;
    name = njSwapCNK(name);
    switch (name) {
    case iff_NJINFO:
        *ptype = iff_NJINFO;
        break;
    case iff_NJCM:
        *ptype = iff_NJCM;
        break;
    case iff_NJBM:
        *ptype = iff_NJBM;
        break;
    case iff_NJTL:
        *ptype = iff_NJTL;
        break;
    case iff_NJLI:
        *ptype = iff_NJLI;
        break;
    case iff_NJCA:
        *ptype = iff_NJCA;
        break;
    case iff_NMDM:
        *ptype = iff_NMDM;
        break;
    case iff_NLIM:
        *ptype = iff_NLIM;
        break;
    case iff_NCAM:
        *ptype = iff_NCAM;
        break;
    case iff_NSSM:
        *ptype = iff_NSSM;
        break;
    default:
        goto unknown;
    }
    copy = syMalloc(size);
    if (copy == NULL) {
        goto failed;
    }
    njMemCopy(copy, p, size);
    p += size;
    *fpos += size;
    goto relocate;
    /* @hack spelling: the unknown name's exit after the copy, reached by a
     * goto, where its statements in the switch's default would do */
unknown:
    *ptype = iff_UNKNOWN;
failed:
    return NULL;
relocate:
    njMemCopy(&name, p, CHUNK_WORD);
    *fpos += CHUNK_WORD;
    p += CHUNK_WORD;
    name = njSwapCNK(name);
    if (name == iff_POF0) {
        njMemCopy(&size, p, CHUNK_WORD);
        p += CHUNK_WORD;
        *fpos += CHUNK_WORD;
        while (size > 0) {
            njMemCopy(&head, p, 1);
            p++;
            (*fpos)++;
            size--;
            switch (head & POF0_FORM) {
            case POF0_PAD:
                continue;
            case POF0_BYTE:
                words = head & POF0_COUNT;
                break;
            case POF0_HALF:
                njMemCopy(&byte1, p, 1);
                p++;
                size--;
                (*fpos)++;
                words = ((head & POF0_COUNT) << 8) | byte1;
                break;
            case POF0_WORD:
                njMemCopy(&byte1, p, 1);
                p++;
                size--;
                njMemCopy(&byte2, p, 1);
                p++;
                size--;
                njMemCopy(&byte3, p, 1);
                p++;
                size--;
                *fpos += 3;
                words = ((head & POF0_COUNT) << 24) | (byte1 << 16) |
                        (byte2 << 8) | byte3;
                break;
            /* @hack redundant: a default the switch's four cases leave no
             * value to reach */
            default:
                continue;
            }
            offset += words * sizeof(Uint32);
            slot = (Uint32 *)(copy + offset);
            if (*slot != 0) {
                *slot += (Uint32)copy;
            }
            /* @hack redundant: a goto to the statement after it */
            goto next;
        next:;
        }
    }
    return copy;
}
