/* no entry reaches the code from each C function's return to the next RTS
 * or BRA */
#include "system/cache.h"

/* clang-format off */
void fun_8c0187a0(void)
{
}

#ifdef DREAMCAST
#ifdef NONMATCHING
/* no entry reaches any code in the block below, so this side leaves it out */
#else
/* @asm open: every instruction is one SHC writes for C */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
; after the return of fun_8c0187a0: set CCR.ORA
        MOV.L       #CCR,R1
        MOV.L       @R1,R0
        OR          #CCR_ORA,R0
        MOV.L       R0,@R1
        RTS
        NOP
#pragma endasm
#endif
#endif

void fun_8c0187b4(void)
{
}

#ifdef DREAMCAST
/* @asm open: every instruction is one SHC writes for C */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
        .IMPORT     __nj_current_matrix_ptr_

; CCR is the cache control register. its ORA bit maps half the operand
; cache as RAM
CCR:            .EQU    H'FF00001C
CCR_ORA:        .EQU    H'20
; the operand cache's address array: a word for each of its 512 lines,
; OC_LINE bytes apart, in two halves of OC_HALF_LINES lines. writing
; OC_INVALID to a line's word writes the line back if it is dirty, and then
; invalidates it
OC_ADDRESS:     .EQU    H'F4000000
OC_LINE:        .EQU    32
OC_HALF_LINES:  .EQU    256
OC_HALF:        .EQU    OC_HALF_LINES*OC_LINE
OC_INVALID:     .EQU    0
; P2 maps memory uncached. ORing P2_BASE into a P1 address gives its P2
; address
P2_BASE:        .EQU    H'A0000000
; SR's interrupt mask, bits 4 to 7. MOV's signed byte can't hold it, so the
; code moves SR_IMASK/16 and shifts it left by 4
SR_IMASK:       .EQU    H'F0
TRUE:           .EQU    1

; after the return of fun_8c0187b4: set CCR.ORA, and empty the operand cache
        MOV.L       #CCR,R1
        MOV.L       @R1,R0
        OR          #CCR_ORA,R0
        MOV.L       R0,@R1
        MOV.W       #0,R2
        MOV.W       #OC_HALF,R3
        MOV.W       #OC_HALF_LINES,R7
        BRA         _cache_empty_lines
        NOP

; empty R7 lines of the operand cache from each of two byte offsets into its
; address array, R2 and R3. R7 must be above 0. interrupts are masked while
; it works. it then unmasks them all, whatever mask they had
_cache_empty_lines:
        MOV         #SR_IMASK/16,R1
        SHLL2       R1
        SHLL2       R1
        NOT         R1,R1                   ; every bit but the mask's
        STC         SR,R0
        AND         R1,R0
        OR          #SR_IMASK,R0
        LDC         R0,SR
        MOV.L       #OC_ADDRESS,R1
        ADD         R1,R2
        ADD         R1,R3
        MOV         #OC_INVALID,R0
?line:
        MOV.L       R0,@R2
        MOV.L       @R3,R4                  ; read, and not used
        MOV.L       R0,@R3
        ADD         #OC_LINE,R2
        ADD         #OC_LINE,R3
        DT          R7
        BF          ?line
        NOP
        NOP
        NOP
        NOP
        MOV         #SR_IMASK/16,R1
        SHLL2       R1
        SHLL2       R1
        NOT         R1,R1
        STC         SR,R0
        AND         R1,R0
        LDC         R0,SR
        RTS
        NOP
#pragma endasm

void fun_8c018818(void)
{
}

/* @asm open: every instruction is one SHC writes for C */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
        .EXPORT     _cache_empty
        .EXPORT     _matrix_load_parent

; after the return of fun_8c018818: clear CCR.ORA, and empty the operand
; cache
        MOV.L       #CCR,R1
        MOV.L       @R1,R0
        MOV.L       #~CCR_ORA,R2
        AND         R2,R0
        MOV.L       R0,@R1
        MOV.W       #0,R2
        MOV.W       #OC_HALF,R3
        MOV.W       #OC_HALF_LINES,R7
        BRA         _cache_empty_lines
        NOP

; void cache_empty(void)
_cache_empty:
        MOV.L       #_cache_empty_all,R0
        MOV.L       #P2_BASE,R1
        OR          R1,R0
        JMP         @R0
        NOP

_cache_empty_all:
        MOV.W       #0,R2
        MOV.W       #OC_HALF,R3
        MOV.W       #OC_HALF_LINES,R7
        BRA         _cache_empty_lines
        NOP

; Bool matrix_load_parent(void)
_matrix_load_parent:
        MOV         #1,R4
        SHLL2       R4
        SHLL2       R4
        MOV.L       #__nj_current_matrix_ptr_,R5
        SHLL2       R4                      ; R4 is 64, the size of a matrix
        MOV.L       @R5,R1
        SUB         R4,R1
; between this FSCHG and the next, an FMOV moves two floats at once
        FSCHG
        FMOV        @R1+,XD0
        FMOV        @R1+,XD2
        FMOV        @R1+,XD4
        FMOV        @R1+,XD6
        FMOV        @R1+,XD8
        FMOV        @R1+,XD10
        FMOV        @R1+,XD12
        FMOV        @R1+,XD14
        FSCHG
        RTS
        MOV         #TRUE,R0
#pragma endasm
/* clang-format on */
#endif
