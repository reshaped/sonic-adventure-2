#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
#include "engine/dc/xmtrx.h"

/* this side repairs the bug tagged below: for a null m it reads only ps[0] to
 * ps[num - 1], where the shipped body reads ps[num] */
void njCalcPoints(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd, Int num)
{
    Int i;

    if (num <= 0) {
        return;
    }
    /* The batch reads one point ahead. Keep the forward scalar walk when
     * writing a point would change a later input. */
    if (m == NULL &&
        ((uintptr_t)pd <= (uintptr_t)ps ||
         (uintptr_t)pd - (uintptr_t)ps >= (size_t)num * sizeof(*ps))) {
        zam_xmtrx_transform_points(&ps->x, sizeof(*ps), &pd->x, sizeof(*pd),
                                   (uint32_t)num);
        return;
    }
    for (i = 0; i < num; i++) {
        zam_njCalcPoint(m, &ps[i], &pd[i]);
    }
}
#else
/* clang-format off */
/* with a null m, each pass of ?pair transforms two points by FTRV. the first
 * is in FV0, and the second in FV4 */
/* @bug memory: with a null m the body reads ps[num], the point after the
 * last. each half of a pass loads its next point before DT counts down. the
 * last point's half loads ps[num]: at 0x8c102de0 for an odd num, and at
 * 0x8c102df6 for an even one */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c102df4 transforms the
 * second point of a pass */
#pragma inline_asm(njCalcPoints)
void njCalcPoints(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd, Int num)
{
        CMP/PL      R7
        BF          ?done
        TST         R4,R4
        BF          ?memory
        ADD         #12,R6
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FLDI1       FR3
?pair:
        FTRV        XMTRX,FV0
        FMOV.S      @R5+,FR4
        FMOV.S      @R5+,FR5
        FMOV.S      @R5+,FR6
        FLDI1       FR7
        FMOV.S      FR2,@-R6
        FMOV.S      FR1,@-R6
        FMOV.S      FR0,@-R6
        DT          R7
        BT          ?done
        ADD         #24,R6
        FTRV        XMTRX,FV4
        FMOV.S      @R5+,FR0
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        FLDI1       FR3
        FMOV.S      FR6,@-R6
        FMOV.S      FR5,@-R6
        FMOV.S      FR4,@-R6
        DT          R7
        BF/S        ?pair
        ADD         #24,R6
?done:
        RTS
        NOP
?memory:
        FMOV.S      @R4,FR4
        MOV         #16,R0
        FMOV.S      @R5+,FR0
        FMOV.S      @(R0,R4),FR5
        FMOV.S      @R5+,FR1
        ADD         #16,R0
        FMOV.S      @R5+,FR2
        FMOV.S      @(R0,R4),FR6
        ADD         #16,R0
        FLDI1       FR3
        FMOV.S      @(R0,R4),FR7
?point:
        MOV         #4,R0
        FIPR        FV0,FV4
        FMOV.S      @(R0,R4),FR8
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR9
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR10
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR11
        FMOV.S      FR7,@R6
        FIPR        FV0,FV8
        MOV         #8,R0
        ADD         #4,R6
        FMOV.S      @(R0,R4),FR4
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR5
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR6
        ADD         #16,R0
        FMOV.S      @(R0,R4),FR7
        FMOV.S      FR11,@R6
        DT          R7
        FIPR        FV0,FV4
        ADD         #4,R6
        BT/S        ?memory_done
        FMOV.S      FR7,@R6
        MOV         #16,R0
        FMOV.S      @R4,FR4
        FMOV.S      @R5+,FR0
        FMOV.S      @(R0,R4),FR5
        FMOV.S      @R5+,FR1
        ADD         #16,R0
        FMOV.S      @R5+,FR2
        FMOV.S      @(R0,R4),FR6
        ADD         #16,R0
        ADD         #4,R6
        FLDI1       FR3
        BRA         ?point
        FMOV.S      @(R0,R4),FR7
?memory_done:
        RTS
        NOP
}
/* clang-format on */
#endif
#endif
