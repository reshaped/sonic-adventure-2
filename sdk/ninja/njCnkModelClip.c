/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>
#include <ninja.h>

/* The radius follows the center in NJS_CNK_MODEL. The body also reads the
 * screen's distance and center, and the two corners of its clip rectangle. */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c14919e transforms the
 * model's center by the current matrix */
#pragma inline_asm(clip_sphere)
static Bool clip_sphere(const NJS_POINT3 *center)
{
        .IMPORT     __nj_far_clip_
        .IMPORT     __nj_near_clip_
        .IMPORT     __nj_screen_
        .IMPORT     __nj_scr_aspect_x_
        .IMPORT     __nj_scr_aspect_y_
        .IMPORT     __nj_2d_clip_
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FLDI1       FR7
        FTRV        XMTRX,FV4
        FMOV.S      @R4+,FR8
        MOV.L       ?far,R1
        FMOV.S      @R1,FR1
        FADD        FR8,FR1
        MOV.L       ?near,R1
        FCMP/GT     FR1,FR6
        FADD        FR8,FR6
        BT          ?clipped
        FMOV.S      @R1,FR1
        FLDI1       FR0
        FCMP/GT     FR1,FR6
        FDIV        FR6,FR0
        BF          ?clipped
        MOV.L       ?screen,R1
        MOV.L       ?aspect_x,R2
        MOV.L       ?aspect_y,R3
        MOV.L       ?clip,R4
        FMOV.S      @R1,FR1
        FMOV.S      @R2,FR2
        FMOV.S      @R3,FR3
        FMUL        FR1,FR2
        ADD         #12,R1
        FMUL        FR1,FR3
        FMOV.S      @R1+,FR9
        FMUL        FR0,FR2
        FMOV.S      @R1+,FR10
        FMUL        FR0,FR3
        FMOV.S      FR4,FR0
        FSUB        FR8,FR0
        MOV         #8,R0
        FMUL        FR2,FR0
        FMOV.S      @(R0,R4),FR1
        FADD        FR9,FR0
        MOV         #0,R0
        FCMP/GT     FR1,FR0
        FMOV.S      FR4,FR0
        BT/S        ?clipped
        FADD        FR8,FR0
        FMOV.S      @(R0,R4),FR1
        FMAC        FR0,FR2,FR9
        FMOV.S      FR5,FR0
        FCMP/GT     FR9,FR1
        FSUB        FR8,FR0
        BT/S        ?clipped
        MOV         #12,R0
        FMUL        FR3,FR0
        FMOV.S      @(R0,R4),FR1
        FADD        FR10,FR0
        MOV         #4,R0
        FCMP/GT     FR1,FR0
        FMOV.S      FR5,FR0
        BT/S        ?clipped
        FADD        FR8,FR0
        FMOV.S      @(R0,R4),FR1
        FMAC        FR0,FR3,FR10
        FCMP/GT     FR10,FR1
        BT          ?clipped
        RTS
        MOV         #0,R0
?clipped:
        RTS
        MOV         #1,R0
        .ALIGN      4
?far:
        .DATA.L     __nj_far_clip_
?near:
        .DATA.L     __nj_near_clip_
?screen:
        .DATA.L     __nj_screen_
?aspect_x:
        .DATA.L     __nj_scr_aspect_x_
?aspect_y:
        .DATA.L     __nj_scr_aspect_y_
?clip:
        .DATA.L     __nj_2d_clip_
}
/* clang-format on */

Bool njCnkModelClip(NJS_CNK_MODEL *model)
{
    register const NJS_POINT3 *center = &model->center;

    return clip_sphere(center);
}
#endif
