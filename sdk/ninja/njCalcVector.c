#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* store v in vd. while _nj_matrix_flag_ is set, scale v to unit length
 * first. the scale takes the FIPR, FSRRA and FMULs the body below runs */
ZAM_INLINE void unit_store(shz_vec3_t v, NJS_VECTOR *vd)
{
    if (_nj_matrix_flag_) {
        v = shz_vec3_scale(v, zam_fsrra(zam_squared_length(v)));
    }
    zam_store_vec3(vd, v);
}

/* turn vs by m with the three FIPRs the body below runs, by zam_fipr_columns.
 * store the result in vd through unit_store. each FIPR's first vector is vs
 * and a 0. its second is a column of m, with 0 in place of its last float.
 * the turn stays out of line, so njCalcVector keeps vs in the registers its
 * FTRV turns in place */
static void __attribute__((noinline)) turn_by(NJS_MATRIX *m, NJS_VECTOR *vs,
                                              NJS_VECTOR *vd)
{
    unit_store(zam_fipr_columns(m, zam_load_vec3(vs)), vd);
}

void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *vs, NJS_VECTOR *vd)
{
    if (m != NULL) {
        turn_by(m, vs, vd);
        return;
    }
    unit_store(shz_xmtrx_transform_vec3(zam_load_vec3(vs)), vd);
}
#else
/* clang-format off */
/* @asm operand: fipr-registers, FIPR FV0,FV4 at 0x8c118b3c takes the
 * result's x from m's first column */
#pragma inline_asm(njCalcVector)
void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *vs, NJS_VECTOR *vd)
{
        .IMPORT     __nj_matrix_flag_
        FMOV.S      @R5+,FR0
        TST         R4,R4
        FMOV.S      @R5+,FR1
        FMOV.S      @R5+,FR2
        BF/S        ?memory
        FLDI0       FR3
        BRA         ?unit
        FTRV        XMTRX,FV0
?memory:
        MOV         #16,R0
        FMOV.S      @R4,FR4
        FMOV.S      @(R0,R4),FR5
        MOV         #32,R0
        FMOV.S      @(R0,R4),FR6
        FLDI0       FR7
        FIPR        FV0,FV4
        MOV         #4,R0
        FMOV.S      @(R0,R4),FR8
        MOV         #20,R0
        FMOV.S      @(R0,R4),FR9
        MOV         #36,R0
        FMOV.S      @(R0,R4),FR10
        FLDI0       FR11
        FIPR        FV0,FV8
        FMOV.S      FR7,FR10
        MOV         #8,R0
        FMOV.S      @(R0,R4),FR4
        MOV         #24,R0
        FMOV.S      @(R0,R4),FR5
        MOV         #40,R0
        FMOV.S      @(R0,R4),FR6
        FLDI0       FR7
        FIPR        FV0,FV4
        FMOV.S      FR10,FR0
        FMOV.S      FR11,FR1
        FMOV.S      FR7,FR2
?unit:
        FLDI0       FR3
        MOV.L       #__nj_matrix_flag_,R3
        MOV.L       @R3,R3
        TST         R3,R3
        BT/S        ?store
        ADD         #12,R6
        FIPR        FV0,FV0
        FSRRA       FR3
        FMUL        FR3,FR2
        FMUL        FR3,FR1
        FMUL        FR3,FR0
?store:
        FMOV.S      FR2,@-R6
        FMOV.S      FR1,@-R6
        RTS
        FMOV.S      FR0,@-R6
}
/* clang-format on */
#endif
#endif
