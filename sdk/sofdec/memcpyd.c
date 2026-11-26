/* Sofdec's copy of 4-byte words */
#include <machine.h>
#include <sg_xpt.h>
#include <sofdec.h>

/* the copy moves a block of BLOCK_WORDS words at a time, 1 << BLOCK_SHIFT */
#define BLOCK_SHIFT 4
#define BLOCK_WORDS (1 << BLOCK_SHIFT)

void UTY_MemcpyDword(Uint32 *dst, const Uint32 *src, Uint32 nword)
{
    Uint32 rest;
    Uint32 blocks;
    /* @hack redundant: the four words declared register, where plain locals
     * would do */
    register Uint32 w0;
    register Uint32 w1;
    register Uint32 w2;
    register Uint32 w3;

    rest = (nword & (BLOCK_WORDS - 1)) + 1;
    while (--rest != 0) {
        *dst++ = *src++;
    }
    blocks = (nword >> BLOCK_SHIFT) + 1;
    while (--blocks != 0) {
        w0 = *src++;
        w1 = *src++;
        w2 = *src++;
        w3 = *src++;
        dst[0] = w0;
        dst[1] = w1;
        dst[2] = w2;
        dst[3] = w3;
        w0 = *src++;
        w1 = *src++;
        w2 = *src++;
        w3 = *src++;
        dst[4] = w0;
        dst[5] = w1;
        dst[6] = w2;
        dst[7] = w3;
        /* the next block's first word */
        prefetch((void *)(src + (BLOCK_WORDS / 2)));
        w0 = *src++;
        w1 = *src++;
        w2 = *src++;
        w3 = *src++;
        dst[8] = w0;
        dst[9] = w1;
        dst[10] = w2;
        dst[11] = w3;
        w0 = *src++;
        w1 = *src++;
        w2 = *src++;
        w3 = *src++;
        dst[12] = w0;
        dst[13] = w1;
        dst[14] = w2;
        dst[15] = w3;
        dst += BLOCK_WORDS;
    }
}
