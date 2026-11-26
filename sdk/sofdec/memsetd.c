/* Sofdec's fill of 4-byte words */
#include <sg_xpt.h>
#include <sofdec.h>

/* the fill stores a block of BLOCK_WORDS words at a time, 1 << BLOCK_SHIFT */
#define BLOCK_SHIFT 4
#define BLOCK_WORDS (1 << BLOCK_SHIFT)

void UTY_MemsetDword(Uint32 *dst, Uint32 value, Uint32 nword)
{
    Uint32 *end;
    Uint32 rest;

    end = dst + nword;
    rest = (nword & (BLOCK_WORDS - 1)) + 1;
    while (--rest != 0) {
        *--end = value;
    }
    /* @hack spelling: nword reused as the count of 16-word blocks, where a
     * local of its own would do */
    nword = (nword >> BLOCK_SHIFT) + 1;
    while (--nword != 0) {
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
        *--end = value;
    }
}
