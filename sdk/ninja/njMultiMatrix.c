/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

/* for a null md, FTRV multiplies each row of ms by XMTRX. the first two
 * products wait on the stack, since the last two still need XMTRX whole. the
 * four then replace XMTRX. for md in memory, each FIPR takes a row of ms with
 * a column of md. the sixteen products wait on the stack until md has been
 * read whole, so ms may be md */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c1194c6 multiplies the
 * second row of ms */
#pragma inline_asm(njMultiMatrix)
void njMultiMatrix(NJS_MATRIX *md, NJS_MATRIX *ms)
{
        TST         R4,R4
        BF          ?memory
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FMOV.S      FR7,@-R15
        FMOV.S      FR6,@-R15
        FMOV.S      FR5,@-R15
        FMOV.S      FR4,@-R15
        FMOV.S      FR3,@-R15
        FMOV.S      FR2,@-R15
        FMOV.S      FR1,@-R15
        FMOV.S      FR0,@-R15
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR6,XD14
        FMOV        DR4,XD12
        FMOV        DR2,XD10
        FMOV        DR0,XD8
        FSCHG
        FMOV.S      @R15+,FR0
        FMOV.S      @R15+,FR1
        FMOV.S      @R15+,FR2
        FMOV.S      @R15+,FR3
        FMOV.S      @R15+,FR4
        FMOV.S      @R15+,FR5
        FMOV.S      @R15+,FR6
        FMOV.S      @R15+,FR7
        FSCHG
        FMOV        DR6,XD6
        FMOV        DR4,XD4
        FMOV        DR2,XD2
        FMOV        DR0,XD0
        RTS
        FSCHG
?memory:
        MOV         R5,R6
        MOV         #16,R0
        FMOV.S      @R4,FR8
        FMOV.S      @(R0,R4),FR9
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR10
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR11
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        MOV         R6,R5
        MOV         #4,R0
        FMOV.S      @(R0,R4),FR8
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR9
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR10
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR11
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        MOV         R6,R5
        MOV         #8,R0
        FMOV.S      @(R0,R4),FR8
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR9
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR10
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR11
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        MOV         R6,R5
        MOV         #12,R0
        FMOV.S      @(R0,R4),FR8
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR9
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR10
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR11
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R5+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR7
        FIPR        FV8,FV0
        FIPR        FV8,FV4
        FMOV.S      FR3,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      @R15+,FR1
        FMOV.S      @R15+,FR3
        FMOV.S      @R15+,FR5
        FMOV.S      @R15+,FR7
        FMOV.S      @R15+,FR0
        FMOV.S      @R15+,FR2
        FMOV.S      @R15+,FR4
        FMOV.S      @R15+,FR6
        MOV         #60,R0
        FMOV.S      FR1,@(R0,R4)
        MOV         #56,R0
        FMOV.S      FR0,@(R0,R4)
        MOV         #44,R0
        FMOV.S      FR3,@(R0,R4)
        MOV         #40,R0
        FMOV.S      FR2,@(R0,R4)
        MOV         #28,R0
        FMOV.S      FR5,@(R0,R4)
        MOV         #24,R0
        FMOV.S      FR4,@(R0,R4)
        MOV         #12,R0
        FMOV.S      FR7,@(R0,R4)
        MOV         #8,R0
        FMOV.S      FR6,@(R0,R4)
        FMOV.S      @R15+,FR1
        FMOV.S      @R15+,FR3
        FMOV.S      @R15+,FR5
        FMOV.S      @R15+,FR7
        FMOV.S      @R15+,FR0
        FMOV.S      @R15+,FR2
        FMOV.S      @R15+,FR4
        FMOV.S      @R15+,FR6
        MOV         #52,R0
        FMOV.S      FR1,@(R0,R4)
        MOV         #48,R0
        FMOV.S      FR0,@(R0,R4)
        MOV         #36,R0
        FMOV.S      FR3,@(R0,R4)
        MOV         #32,R0
        FMOV.S      FR2,@(R0,R4)
        MOV         #20,R0
        FMOV.S      FR5,@(R0,R4)
        MOV         #16,R0
        FMOV.S      FR4,@(R0,R4)
        MOV         #4,R0
        FMOV.S      FR7,@(R0,R4)
        RTS
        FMOV.S      FR6,@R4
}
#endif
/* clang-format on */

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void njMultiMatrix_old(void)
{
}
/* NOLINTEND(misc-use-internal-linkage) */
