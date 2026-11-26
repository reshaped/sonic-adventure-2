#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* set XMTRX's row first_row to a times the current matrix, and its row
 * second_row to b times it. both products are taken before either row is
 * written */
ZAM_INLINE void turn_xmtrx(unsigned first_row, shz_vec4_t a,
                           unsigned second_row, shz_vec4_t b)
{
    shz_vec4_t to_first = shz_xmtrx_transform_vec4(a);
    shz_vec4_t to_second = shz_xmtrx_transform_vec4(b);

    shz_xmtrx_write_col(first_row, to_first);
    shz_xmtrx_write_col(second_row, to_second);
}

/* turn m's rows first_row and second_row. a, an element of the first,
 * becomes a * cosine + b * sine. b, the second's element in the same column,
 * becomes b * cosine - a * sine. the shipped body's turns differ in which
 * products they add and which they subtract. so njRotate's first two calls
 * pass -sine. that changes no bit: b * -sine is -(b * sine), and x + -y is
 * x - y */
ZAM_INLINE void turn_rows(NJS_MATRIX *m, unsigned first_row,
                          unsigned second_row, Float cosine, Float sine)
{
    int i;

    for (i = 0; i < 4; i++) {
        Float a = (*m)[4 * first_row + i];
        Float b = (*m)[4 * second_row + i];
        Float a_cosine = a * cosine;
        Float b_cosine = b * cosine;
        Float b_sine = b * sine;
        Float a_sine = a * sine;

        (*m)[4 * first_row + i] = a_cosine + b_sine;
        (*m)[4 * second_row + i] = b_cosine - a_sine;
    }
}

/* this side keeps the console's bits. the turns about x and y take their sines
 * and cosines from the shipped body's arithmetic and branches. the turn by ang
 * takes FSCA's. each turn of XMTRX runs the same two FTRVs on the same rows.
 * each turn of m runs the same products and sums.
 *
 * SH4ZAM's shz_xmtrx_rotate_dir_u16 and shz_xmtrx_rotate_u16 can't stand in
 * for these turns. each sums one matrix's floats by FMACs and applies it by
 * three FTRVs, which gives other bits than the five turns. the first scales
 * XMTRX where v isn't unit length, and the second scales v to unit length.
 * for a zero v, this body turns by ang about z */
void njRotate(NJS_MATRIX *m, NJS_VECTOR *v, Angle ang)
{
    Float x = v->x;
    Float y = v->y;
    Float z = v->z;
    Float across = __builtin_fmaf(z, z, y * y);
    Float length;
    Float sin_x;
    Float cos_x;
    Float sin_y;
    Float cos_y;
    shz_sincos_t turn;

    if (z == 0.0F && y != 0.0F) {
        sin_x = y > 0.0F ? 1.0F : -1.0F;
        cos_x = 0.0F;
    } else if (y == 0.0F) {
        sin_x = 0.0F;
        cos_x = z > 0.0F ? 1.0F : -1.0F;
    } else {
        Float scale = zam_fsrra(across);

        sin_x = y * scale;
        cos_x = z * scale;
    }
    length = __builtin_sqrtf(across);
    if (length == 0.0F && x != 0.0F) {
        sin_y = x > 0.0F ? 1.0F : -1.0F;
        cos_y = 0.0F;
    } else if (x == 0.0F) {
        sin_y = 0.0F;
        cos_y = length > 0.0F ? 1.0F : -1.0F;
    } else {
        sin_y = x;
        cos_y = length;
    }
    turn = zam_fsca(ang);
    if (m == NULL) {
        turn_xmtrx(1, shz_vec4_init(0.0F, cos_x, -sin_x, 0.0F), 2,
                   shz_vec4_init(0.0F, sin_x, cos_x, 0.0F));
        turn_xmtrx(0, shz_vec4_init(cos_y, 0.0F, -sin_y, 0.0F), 2,
                   shz_vec4_init(sin_y, 0.0F, cos_y, 0.0F));
        turn_xmtrx(0, shz_vec4_init(turn.cos, turn.sin, 0.0F, 0.0F), 1,
                   shz_vec4_init(-turn.sin, turn.cos, 0.0F, 0.0F));
        turn_xmtrx(0, shz_vec4_init(cos_y, 0.0F, sin_y, 0.0F), 2,
                   shz_vec4_init(-sin_y, 0.0F, cos_y, 0.0F));
        turn_xmtrx(1, shz_vec4_init(0.0F, cos_x, sin_x, 0.0F), 2,
                   shz_vec4_init(0.0F, -sin_x, cos_x, 0.0F));
    } else {
        turn_rows(m, 1, 2, cos_x, -sin_x);
        turn_rows(m, 0, 2, cos_y, -sin_y);
        turn_rows(m, 0, 1, turn.cos, turn.sin);
        turn_rows(m, 0, 2, cos_y, sin_y);
        turn_rows(m, 1, 2, cos_x, sin_x);
    }
}
#else
/* clang-format off */
/* the body makes five turns, and a point takes them in the reverse order.
 * the first two a point takes bring v onto z, about x and then about y. the
 * third turns by ang about z, and the last two undo the first two. v's parts
 * give the turns about x and y their sines and cosines. only ang's go through
 * FSCA */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c119c4a multiplies the
 * second row of the first turn the body makes */
#pragma inline_asm(njRotate)
void njRotate(NJS_MATRIX *m, NJS_VECTOR *v, Angle ang)
{
        MOV         R5,R3
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FMOV.S      FR5,FR7
        FMOV.S      FR6,FR0
        FMUL        FR5,FR7
        FMAC        FR0,FR6,FR7
        FLDI0       FR0
        FLDI1       FR1
        FMOV.S      FR7,FR3
        FCMP/EQ     FR0,FR6
        BF/S        ?no_y
        FCMP/EQ     FR0,FR5
        BT          ?no_y
        FMOV.S      FR0,FR9
        FCMP/GT     FR0,FR5
        BT/S        ?about_y
        FMOV.S      FR1,FR8
        BRA         ?about_y
        FNEG        FR8
?no_y:
        BF          ?scale
        FMOV.S      FR0,FR8
        FCMP/GT     FR0,FR6
        BT/S        ?about_y
        FMOV.S      FR1,FR9
        BRA         ?about_y
        FNEG        FR9
?scale:
        FSRRA       FR3
        FMOV.S      FR5,FR8
        FMOV.S      FR6,FR9
        FMUL        FR3,FR8
        FMUL        FR3,FR9
?about_y:
        FSQRT       FR7
        FCMP/EQ     FR0,FR7
        BF/S        ?no_x
        FCMP/EQ     FR0,FR4
        BT          ?no_x
        FMOV.S      FR0,FR11
        FCMP/GT     FR0,FR4
        BT/S        ?turn
        FMOV.S      FR1,FR10
        BRA         ?turn
        FNEG        FR10
?no_x:
        BF          ?as_is
        FMOV.S      FR0,FR10
        FCMP/GT     FR0,FR7
        BT/S        ?turn
        FMOV.S      FR1,FR11
        BRA         ?turn
        FNEG        FR11
?as_is:
        FMOV.S      FR4,FR10
        FMOV.S      FR7,FR11
?turn:
        TST         R4,R4
        BF          ?memory
        FLDI0       FR0
        FMOV.S      FR9,FR1
        FMOV.S      FR8,FR2
        FNEG        FR2
        FLDI0       FR3
        FLDI0       FR4
        FMOV.S      FR8,FR5
        FMOV.S      FR9,FR6
        FLDI0       FR7
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD4
        FMOV        DR2,XD6
        FMOV        DR4,XD8
        FMOV        DR6,XD10
        FSCHG
        FMOV.S      FR11,FR0
        FLDI0       FR1
        FMOV.S      FR10,FR2
        FNEG        FR2
        FLDI0       FR3
        FMOV.S      FR10,FR4
        FLDI0       FR5
        FMOV.S      FR11,FR6
        FLDI0       FR7
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD0
        FMOV        DR2,XD2
        FMOV        DR4,XD8
        FMOV        DR6,XD10
        FSCHG
        LDS         R6,FPUL
        FSCA        FPUL,DR0
        FMOV.S      FR0,FR4
        FMOV.S      FR1,FR5
        FMOV.S      FR4,FR1
        FMOV.S      FR5,FR0
        FNEG        FR4
        FLDI0       FR7
        FLDI0       FR6
        FLDI0       FR3
        FLDI0       FR2
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD0
        FMOV        DR2,XD2
        FMOV        DR4,XD4
        FMOV        DR6,XD6
        FSCHG
        FMOV.S      FR11,FR0
        FLDI0       FR1
        FMOV.S      FR10,FR2
        FLDI0       FR3
        FMOV.S      FR10,FR4
        FNEG        FR4
        FLDI0       FR5
        FMOV.S      FR11,FR6
        FLDI0       FR7
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD0
        FMOV        DR2,XD2
        FMOV        DR4,XD8
        FMOV        DR6,XD10
        FSCHG
        FLDI0       FR0
        FMOV.S      FR9,FR1
        FMOV.S      FR8,FR2
        FLDI0       FR3
        FLDI0       FR4
        FMOV.S      FR8,FR5
        FNEG        FR5
        FMOV.S      FR9,FR6
        FLDI0       FR7
        FTRV        XMTRX,FV0
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD4
        FMOV        DR2,XD6
        FMOV        DR4,XD8
        FMOV        DR6,XD10
        RTS
        FSCHG
?memory:
        FMOV.S      FR8,FR3
        FNEG        FR3
        MOV         R4,R7
        MOV         #16,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #32,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR3,FR7
        FMUL        FR9,FR6
        FMUL        FR3,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #20,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #36,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR3,FR7
        FMUL        FR9,FR6
        FMUL        FR3,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #24,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #40,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR3,FR7
        FMUL        FR9,FR6
        FMUL        FR3,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #28,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #44,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR3,FR7
        FMUL        FR9,FR6
        FMUL        FR3,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        FMOV.S      @R4,FR4
        FMOV.S      FR4,FR5
        ADD         #32,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR10,FR7
        FMUL        FR11,FR6
        FMUL        FR10,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@R4
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #4,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #36,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR10,FR7
        FMUL        FR11,FR6
        FMUL        FR10,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #8,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #40,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR10,FR7
        FMUL        FR11,FR6
        FMUL        FR10,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #12,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #44,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR10,FR7
        FMUL        FR11,FR6
        FMUL        FR10,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        LDS         R6,FPUL
        FSCA        FPUL,DR0
        MOV         R4,R7
        FMOV.S      @R4,FR4
        FMOV.S      FR4,FR5
        ADD         #16,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR1,FR4
        FMUL        FR0,FR7
        FMUL        FR1,FR6
        FMUL        FR0,FR5
        FADD        FR7,FR4
        FSUB        FR5,FR6
        FMOV.S      FR4,@R4
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #4,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #20,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR1,FR4
        FMUL        FR0,FR7
        FMUL        FR1,FR6
        FMUL        FR0,FR5
        FADD        FR7,FR4
        FSUB        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #8,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #24,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR1,FR4
        FMUL        FR0,FR7
        FMUL        FR1,FR6
        FMUL        FR0,FR5
        FADD        FR7,FR4
        FSUB        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #12,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #28,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR1,FR4
        FMUL        FR0,FR7
        FMUL        FR1,FR6
        FMUL        FR0,FR5
        FADD        FR7,FR4
        FSUB        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        FMOV.S      FR10,FR3
        FNEG        FR3
        MOV         R4,R7
        FMOV.S      @R4,FR4
        FMOV.S      FR4,FR5
        ADD         #32,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR3,FR7
        FMUL        FR11,FR6
        FMUL        FR3,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@R4
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #4,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #36,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR3,FR7
        FMUL        FR11,FR6
        FMUL        FR3,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #8,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #40,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR3,FR7
        FMUL        FR11,FR6
        FMUL        FR3,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #12,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #44,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR11,FR4
        FMUL        FR3,FR7
        FMUL        FR11,FR6
        FMUL        FR3,FR5
        FSUB        FR7,FR4
        FADD        FR5,FR6
        FMOV.S      FR4,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #16,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #32,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR8,FR7
        FMUL        FR9,FR6
        FMUL        FR8,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #20,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #36,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR8,FR7
        FMUL        FR9,FR6
        FMUL        FR8,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #24,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #40,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR8,FR7
        FMUL        FR9,FR6
        FMUL        FR8,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        FMOV.S      FR6,@R7
        MOV         R4,R7
        MOV         #28,R0
        FMOV.S      @(R0,R4),FR4
        FMOV.S      FR4,FR5
        ADD         #44,R7
        FMOV.S      @R7,FR6
        FMOV.S      FR6,FR7
        FMUL        FR9,FR4
        FMUL        FR8,FR7
        FMUL        FR9,FR6
        FMUL        FR8,FR5
        FADD        FR4,FR7
        FSUB        FR5,FR6
        FMOV.S      FR7,@(R0,R4)
        RTS
        FMOV.S      FR6,@R7
}
/* clang-format on */
#endif
#endif
