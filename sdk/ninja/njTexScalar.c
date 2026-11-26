#include <sg_xpt.h>
#include "../ninja/copy_elements.h"
#include "../ninja/tex_copy.h"

/* njTexMemCopy's last instruction, TST #3,R0, runs on into the first BF
 * below. R0 holds dst | src | size. the BF falls through into njTexMemCopy4
 * where the three are multiples of 4. otherwise halfword_test's BF falls
 * through into njTexMemCopy2 where they are multiples of 2. it branches to
 * njTexMemCopy1 where they aren't. each BF opens its code section, PSGWORD or
 * PSG, on a four-byte boundary. so njTexMemCopy4 and njTexMemCopy2 each start 2
 * bytes past a four-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: every instruction is one SHC writes for C */
#pragma asm
        .SECTION    PSGWORD,CODE,ALIGN=4
        BF          halfword_test
        .SECTION    PSG,CODE,ALIGN=4
#pragma endasm
#pragma section WORD
#endif
#endif
/* clang-format on */

void njTexMemCopy4(void *dst, const void *src, Uint32 size)
{
    size >>= 2;
#ifdef NONMATCHING
    /* this side copies nothing where size holds no whole element. COPY_ELEMENTS
     * copies before it tests its count, so it takes no count of 0 */
    if (size == 0) {
        return;
    }
#endif
    COPY_ELEMENTS(Uint32, dst, src, size);
}

/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: every instruction is one SHC writes for C */
#pragma asm
halfword_test:
        TST         #1,R0
        .SECTION    PSG,CODE,ALIGN=4
        BF          _njTexMemCopy1
        .SECTION    PSGWORD,CODE,ALIGN=4
#pragma endasm
#pragma section
#endif
#endif
/* clang-format on */

void njTexMemCopy2(void *dst, const void *src, Uint32 size)
{
    size >>= 1;
#ifdef NONMATCHING
    /* this side copies nothing where size holds no whole element. COPY_ELEMENTS
     * copies before it tests its count, so it takes no count of 0 */
    if (size == 0) {
        return;
    }
#endif
    COPY_ELEMENTS(Sint16, dst, src, size);
}

void njTexMemCopy1(void *dst, const void *src, Uint32 size)
{
#ifdef NONMATCHING
    /* this side copies nothing for a size of 0. COPY_ELEMENTS copies before it
     * tests its count, so it takes no count of 0 */
    if (size == 0) {
        return;
    }
#endif
    COPY_ELEMENTS(Sint8, dst, src, size);
}
