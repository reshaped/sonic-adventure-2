/* the 64-bit products of two words, and a 64-bit dividend's quotient by a
 * word */
#ifdef DREAMCAST
#include <sofdec_uty.h>

#ifdef NONMATCHING
/* this side forms each product in C, as a 64-bit integer, in place of SHC's
 * assembly */
#include <stdint.h>

void UTY_Dmulu(Uint32 left, Uint32 right, struct uty_u64 *product)
{
    uint64_t value;

    value = (uint64_t)left * right;
    product->high = (Uint32)(value >> 32);
    product->low = (Uint32)value;
}

void UTY_Dmuls(Sint32 left, Sint32 right, struct uty_s64 *product)
{
    uint64_t bits;

    bits = (uint64_t)((int64_t)left * right);
    product->high = (Sint32)(bits >> 32);
    product->low = (Uint32)bits;
}
#else
/* clang-format off */
/* @asm opcode: dmulu, DMULU.L R4,R5 at 0x8c551670 forms UTY_Dmulu's
 * unsigned 64-bit product */
#pragma asm
        .SECTION    PSG,CODE,ALIGN=4
        .EXPORT     _UTY_Dmulu

_UTY_Dmulu:
        DMULU.L     R4,R5
        ADD         #8,R6
        STS.L       MACL,@-R6
        RTS
        STS.L       MACH,@-R6
        NOP                     ; align UTY_Dmuls on four bytes
#pragma endasm

/* @asm opcode: dmuls, DMULS.L R4,R5 at 0x8c55167c forms UTY_Dmuls's signed
 * 64-bit product */
#pragma asm
        .EXPORT     _UTY_Dmuls

_UTY_Dmuls:
        DMULS.L     R4,R5
        ADD         #8,R6
        STS.L       MACL,@-R6
        RTS
        STS.L       MACH,@-R6
        NOP                     ; align UTY_Div64u on four bytes
#pragma endasm

/* return high:low / divisor from UTY_Div64u, the function the body expands
 * in. high must be below divisor, so that the quotient fits in 32 bits */
/* @asm opcode: rotcl, ROTCL R5 at 0x8c551692 and the 32 ROTCLs after it
 * shift the dividend out of low through T, and the quotient into low */
#pragma inline_asm(return_division_quotient(size=136))
static Uint32 return_division_quotient(Uint32 high, Uint32 low,
                                       Uint32 divisor)
{
        DIV0U
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        DIV1        R6,R4
        ROTCL       R5
        RTS
        MOV         R5,R0
}
/* clang-format on */
#endif

Uint32 UTY_Div64u(Uint32 high, Uint32 low, Uint32 divisor)
{
    if (divisor != 0 && high < divisor) {
#ifdef NONMATCHING
        /* this side runs return_division_quotient's steps in GCC's asm: DIV0U,
         * then 32 DIV1s, each fed by a ROTCL */
        __asm__("div0u\n\t"
                "rotcl %1\n\t"
                ".rept 32\n\t"
                "div1 %2,%0\n\t"
                "rotcl %1\n\t"
                ".endr"
                : "+&r"(high), "+&r"(low)
                : "r"(divisor)
                : "t");
        return low;
#else
        /* the body returns the quotient from UTY_Div64u itself */
        return_division_quotient(high, low, divisor);
#endif
    }
    return ~(Uint32)0;
}
#endif
