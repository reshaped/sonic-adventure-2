#ifdef DREAMCAST
#include <ninja.h>

/* the elements of a matrix that scale x and y */
#define SCALE_X 0
#define SCALE_Y 5

#ifdef NONMATCHING
#include <stdint.h>

/* Each 64-byte slot occupies two complete MOVCA.L cache lines. */
void njInitMatrix(NJS_MATRIX *m, Sint32 n, Int flag)
{
    if (m == NULL || n < 1 || ((uintptr_t)m & 31U) ||
        (Uint32)n > (UINTPTR_MAX - (uintptr_t)m) / sizeof(*m)) {
        return;
    }
    _nj_stack_size_ = 1;
    _nj_matrix_flag_ = flag;
    _nj_max_stack_size_ = n;
    _nj_current_matrix_ptr_ = m;
    _nj_base_matrix_ptr_ = m;
    zam_copy_matrix(*m, _nj_unit_matrix_);
    (*m)[SCALE_X] = -(*m)[SCALE_X];
    (*m)[SCALE_Y] = -(*m)[SCALE_Y];
    shz_xmtrx_load_4x4((const shz_mat4x4_t *)m);
}
#else
/* clang-format off */
/* m[0] is _nj_unit_matrix_ with elements SCALE_X and SCALE_Y negated. FRCHG
 * turns the registers' banks over. the sixteen loads of m[0] then land in
 * XMTRX */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c118c84 follows the body's own
 * return at 0x8c118c66 and its pool */
#pragma inline_asm(njInitMatrix)
void njInitMatrix(NJS_MATRIX *m, Sint32 n, Int flag)
{
        .IMPORT     __nj_stack_size_
        .IMPORT     __nj_matrix_flag_
        .IMPORT     __nj_max_stack_size_
        .IMPORT     __nj_current_matrix_ptr_
        .IMPORT     __nj_base_matrix_ptr_
        .IMPORT     __nj_unit_matrix_
        MOV.L       #__nj_stack_size_,R0
        MOV         #1,R7
        MOV.L       #__nj_matrix_flag_,R2
        MOV.L       R7,@R0
        MOV.L       #__nj_max_stack_size_,R1
        MOV.L       R6,@R2
        MOV.L       #__nj_current_matrix_ptr_,R0
        MOV.L       #__nj_base_matrix_ptr_,R3
        MOV.L       R4,@R0
        MOV.L       R5,@R1
        MOV.L       R4,@R3
        MOV.L       #__nj_unit_matrix_,R1
        ADD         #64,R4
        ADD         #32,R1
        FMOV.S      @R1+,FR0
        FMOV.S      @R1+,FR1
        FMOV.S      @R1+,FR2
        FMOV.S      @R1+,FR3
        FMOV.S      @R1+,FR4
        FMOV.S      @R1+,FR5
        FMOV.S      @R1+,FR6
        FMOV.S      @R1+,FR7
        FMOV.S      FR7,@-R4
        FMOV.S      FR6,@-R4
        ADD         #-64,R1
        FMOV.S      FR5,@-R4
        FMOV.S      FR4,@-R4
        FMOV.S      FR3,@-R4
        FMOV.S      FR2,@-R4
        FMOV.S      FR1,@-R4
        FMOV.S      FR0,@-R4
        FMOV.S      @R1+,FR0
        FMOV.S      @R1+,FR1
        FMOV.S      @R1+,FR2
        FMOV.S      @R1+,FR3
        FMOV.S      @R1+,FR4
        FMOV.S      @R1+,FR5
        FMOV.S      @R1+,FR6
        FMOV.S      @R1+,FR7
        FMOV.S      FR7,@-R4
        FMOV.S      FR6,@-R4
        FMOV.S      FR5,@-R4
        FMOV.S      FR4,@-R4
        FMOV.S      FR3,@-R4
        FMOV.S      FR2,@-R4
        FMOV.S      FR1,@-R4
        FMOV.S      FR0,@-R4
        FNEG        FR0
        MOV         #20,R0
        FNEG        FR5
        FMOV.S      FR0,@R4
        FMOV.S      FR5,@(R0,R4)
        FRCHG
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FMOV.S      @R4+,FR7
        FMOV.S      @R4+,FR8
        FMOV.S      @R4+,FR9
        FMOV.S      @R4+,FR10
        FMOV.S      @R4+,FR11
        FMOV.S      @R4+,FR12
        FMOV.S      @R4+,FR13
        FMOV.S      @R4+,FR14
        FMOV.S      @R4+,FR15
        RTS
        FRCHG
}
/* clang-format on */
#endif
#endif
