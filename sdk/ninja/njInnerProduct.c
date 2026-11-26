#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper runs the shipped
 * body's FIPR */
#undef njInnerProduct
Float njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2)
{
    return zam_njInnerProduct(v1, v2);
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c118ca4 follows the body's own
 * return at 0x8c118c9e */
#pragma inline_asm(njInnerProduct)
Float njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2)
{
        MOV         #4,R0
        FMOV.S      @R4,FR0
        FMOV.S      @R5,FR4
        FMOV.S      @(R0,R4),FR1
        FMOV.S      @(R0,R5),FR5
        MOV         #8,R0
        FLDI0       FR7
        FMOV.S      @(R0,R4),FR2
        FMOV.S      @(R0,R5),FR6
        FLDI0       FR3
        FIPR        FV4,FV0
        RTS
        FMOV        FR3,FR0
}
/* clang-format on */
#endif
#endif
