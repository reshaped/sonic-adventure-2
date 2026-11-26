/* vector arithmetic, and matrix_push_unit, which matrix.h declares */
#ifdef DREAMCAST
#include "system/matrix.h"
#include "system/vector.h"

#define UNREACHED_COMPARISONS(x, y, z, second_x)                               \
    /* @hack redundant: a trap and comparisons of x, y, z and second_x that    \
     * no path reaches */                                                      \
    if (0) {                                                                   \
        _builtin_trapa(0);                                                     \
        if ((x) == (y))                                                        \
            if ((y) == (z))                                                    \
                if ((y) == (z))                                                \
                    if ((y) == (z))                                            \
                        if ((z) == (second_x))                                 \
                            if ((z) == (second_x))                             \
                                if ((z) == (second_x)) {                       \
                                }                                              \
    }

void vector_sum(const NJS_VECTOR *first, const NJS_VECTOR *second,
                NJS_VECTOR *out)
{
#ifdef NONMATCHING
    /* the hack's plain form: the same sum without the volatile read and the
     * dead comparisons that steer SHC */
    NJS_VECTOR result;

    result.x = first->x + second->x;
    result.y = first->y + second->y;
    result.z = first->z + second->z;
    *out = result;
#else
    /* @hack spelling: the components walked by pointers, and stored from z
     * back to x, where named components would do */
    Float *out_at = (Float *)out;
    const Float *second_at = (const Float *)second;
    const Float *first_at = (const Float *)first;
    Float x, y, z, second_x, second_y, second_z;

    second_x = *second_at++;
    x = *first_at++;
    x += second_x;
    y = *first_at++;
    /* @hack type: a volatile read of second_y, where a plain read would do */
    second_y = *(const volatile Float *)second_at++;
    second_z = *second_at++;
    z = *first_at++;
    y += second_y;
    z += second_z;
    out_at += sizeof *out / sizeof *out_at;
    /* @hack redundant: a test of first_at and second_at that no path
     * reaches */
    if (0) {
        if (first_at == second_at) {
        }
    }
    *--out_at = z;
    *--out_at = y;
    *--out_at = x;
    UNREACHED_COMPARISONS(x, y, z, second_x)
#endif
}

void vector_diff(const NJS_VECTOR *first, const NJS_VECTOR *second,
                 NJS_VECTOR *out)
{
#ifdef NONMATCHING
    /* the hack's plain form: the same difference without the volatile read and
     * the dead comparisons that steer SHC */
    NJS_VECTOR result;

    result.x = first->x - second->x;
    result.y = first->y - second->y;
    result.z = first->z - second->z;
    *out = result;
#else
    /* @hack spelling: the components walked by pointers, and stored from z
     * back to x, where named components would do */
    Float *out_at = (Float *)out;
    const Float *second_at = (const Float *)second;
    const Float *first_at = (const Float *)first;
    Float x, y, z, second_x, second_y, second_z;

    second_x = *second_at++;
    x = *first_at++;
    x -= second_x;
    y = *first_at++;
    /* @hack type: a volatile read of second_y, where a plain read would do */
    second_y = *(const volatile Float *)second_at++;
    second_z = *second_at++;
    z = *first_at++;
    y -= second_y;
    z -= second_z;
    out_at += sizeof *out / sizeof *out_at;
    /* @hack redundant: a test of first_at and second_at that no path
     * reaches */
    if (0) {
        if (first_at == second_at) {
        }
    }
    *--out_at = z;
    *--out_at = y;
    *--out_at = x;
    UNREACHED_COMPARISONS(x, y, z, second_x)
#endif
}

/* clang-format off */
/* @asm operand: fipr-unloaded, FIPR FV4,FV0 at 0x8c03305a reads FR3 and FR7,
 * which the function zeroes and never loads */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
        .EXPORT     _vector_dot_from

; Float vector_dot_from(const NJS_POINT3 *from, const NJS_POINT3 *first,
;                       const NJS_POINT3 *second)
; FV0 holds first - from, and FV4 second - from
_vector_dot_from:
        FMOV.S      @R5+,FR0
        FLDI0       FR3
        FMOV.S      @R4+,FR8
        FSUB        FR8,FR0
        FMOV.S      @R5+,FR1
        FLDI0       FR7
        FMOV.S      @R4+,FR9
        FSUB        FR9,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R4+,FR10
        FSUB        FR10,FR2
        FMOV.S      @R6+,FR4
        FSUB        FR8,FR4
        FMOV.S      @R6+,FR5
        FSUB        FR9,FR5
        FMOV.S      @R6+,FR6
        FSUB        FR10,FR6
        FIPR        FV4,FV0
        RTS
        FMOV.S      FR3,FR0
; clang-format reads the block as C, and needs this line to end with a ;
#pragma endasm
/* clang-format on */

Bool matrix_push_unit(void)
{
#ifdef NONMATCHING
    /* this side pushes by njPushMatrixEx and stores SH4ZAM's unit matrix in the
     * new slot, where the matching side's assembly does both */
    if (!njPushMatrixEx()) {
        return FALSE;
    }
    shz_xmtrx_init_identity();
    zam_store_slot(*_nj_current_matrix_ptr_);
#else
    /* where Ninja's matrix stack is full, the RTS in the assembly below
     * returns FALSE from matrix_push_unit itself. otherwise the assembly runs
     * on into the return of TRUE */
    /* clang-format off */
    /* @asm operand: fschg-register-moves, FMOV DR0,XD0 at 0x8c0330a6 writes
     * the unit matrix's first two elements to XMTRX as a pair */
#pragma asm
        .IMPORT     __nj_max_stack_size_
        .IMPORT     __nj_stack_size_
        .IMPORT     __nj_current_matrix_ptr_
MATRIX_SIZE:    .EQU    64
FALSE:          .EQU    0
        MOV.L       #__nj_max_stack_size_,R1
        MOV.L       #__nj_stack_size_,R6
        MOV.L       @R1,R3
        MOV.L       @R6,R2
        CMP/GE      R3,R2
        BF          ?room
        RTS
        MOV         #FALSE,R0
?room:
        ADD         #1,R2
        MOV.L       #__nj_current_matrix_ptr_,R1
        MOV.L       R2,@R6
        MOV.L       @R1,R5
        ADD         #MATRIX_SIZE,R5
        MOV.L       R5,@R1
        MOV         R5,R4
        FLDI1       FR0                     ; DR0 holds (1, 0), DR2 (0, 1)
        FLDI0       FR1                     ; and DR4 (0, 0): the pairs that
        FLDI0       FR2                     ; make up the unit matrix
        FLDI1       FR3
        FLDI0       FR4
        FLDI0       FR5
; between this FSCHG and the next, an FMOV moves two floats at once
        FSCHG
; save the current matrix to the old slot, which ends at R5
        FMOV        XD14,@-R5
        FMOV        XD12,@-R5
        FMOV        XD10,@-R5
        FMOV        XD8,@-R5
        FMOV        XD6,@-R5
        FMOV        XD4,@-R5
        FMOV        XD2,@-R5
        FMOV        XD0,@-R5
; make the unit matrix current, and store it to the new slot, which ends at R4
        ADD         #MATRIX_SIZE,R4
        FMOV        DR0,XD0
        FMOV        DR4,XD2
        FMOV        DR2,XD4
        FMOV        DR4,XD6
        FMOV        DR4,XD8
        FMOV        DR0,XD10
        FMOV        DR4,XD12
        FMOV        DR2,XD14
        FMOV        XD14,@-R4
        FMOV        XD12,@-R4
        FMOV        XD10,@-R4
        FMOV        XD8,@-R4
        FMOV        XD6,@-R4
        FMOV        XD4,@-R4
        FMOV        XD2,@-R4
        FMOV        XD0,@-R4
        FSCHG
#pragma endasm
    /* clang-format on */
#endif
    return TRUE;
}

/* clang-format off */
/* @asm open: every instruction is one SHC writes for C */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
        .EXPORT     _vector_distance

; Float vector_distance(const NJS_POINT3 *from, const NJS_POINT3 *to)
_vector_distance:
        FMOV.S      @R4+,FR1
        FMOV.S      @R5+,FR3
        FMOV.S      @R4+,FR0
        FSUB        FR3,FR1
        FMOV.S      @R5+,FR4
        FMUL        FR1,FR1
        FMOV.S      @R4,FR2
        FSUB        FR4,FR0
        FMOV.S      @R5,FR5
        FMAC        FR0,FR0,FR1
        FSUB        FR5,FR2
        FMOV        FR2,FR0
        FMAC        FR0,FR0,FR1
        FLDI0       FR5
        FMOV        FR1,FR0
        FCMP/EQ     FR1,FR5
        BF          ?root
        RTS
        FLDI0       FR0
?root:
        RTS
        FSQRT       FR0
; asmsh reads nothing past this, so the .ALIGN 4 SHC writes after the block
; doesn't pad P past FSQRT
        .END
#pragma endasm
/* clang-format on */
#endif
