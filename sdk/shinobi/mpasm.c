/* the Maple driver's byte reversal of a run of words */
#ifdef DREAMCAST
#include <maple_driver.h>
#include <sg_xpt.h>

Sint32 mpdrv_swaplong(Uint32 *words, Sint32 count)
{
#ifdef NONMATCHING
    /* this side reverses each word's bytes in C, as the block's SWAP.B, SWAP.W
     * and SWAP.B do */
    do {
        Uint32 word = *words;

        *words++ = (word << 24) | ((word & 0x0000ff00) << 8) |
                   ((word & 0x00ff0000) >> 8) | (word >> 24);
    } while (--count != 0);
#else
    /* the block reads words in R4 and count in R5, where the call passed
     * them */
    /* clang-format off */
    /* @asm opcode: swap, SWAP.B R0,R1 at 0x8c170e8e starts reversing the
     * word's bytes */
#pragma asm
?word:
        MOV.L       @R4,R0
        SWAP.B      R0,R1
        SWAP.W      R1,R0
        SWAP.B      R0,R1
        DT          R5
        MOV.L       R1,@R4
        BF/S        ?word
        ADD         #4,R4
#pragma endasm
    /* clang-format on */
#endif
    return 0;
}
#endif
