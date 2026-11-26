#include <nj_global.h>
#include <nj_km.h>
#include <ninja.h>
#include <sg_xpt.h>

Uint32 *_nj_ptcl_sq_ptr_;

void njPtclPolygonStart(Uint32 col)
{
    njGetkmPointer();
    _nj_ptcl_sq_ptr_ = njStartVertexNonTexSpriteOpaque(col);
}

void njPtclPolygonEnd(void)
{
    njEndVertex(_nj_ptcl_sq_ptr_);
    njSetkmPointer();
}

#ifdef DREAMCAST
#ifdef NONMATCHING
#include <kamui.h>
#include <machine.h>

/* make dc builds the two square draws from this C, which sends the sprite
 * the bodies under the #else send. only the square's half width differs: it
 * takes the x aspect its center takes, which is 1 but under the port's
 * 16:9 (engine/dc/port.h). this side serves the screen alone, and its
 * products run in another order than the bodies' */

/* the words a square takes of the store queue: a block of eight, and half
 * of the next */
#define SQUARE_WORDS 16

/* send a square for each of the n points stride bytes apart from p */
static void ptcl_squares(const NJS_POINT3 *p, Sint32 n, Float h, Sint32 stride)
{
    Float scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    Float scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    Float half_x = h * _nj_scr_aspect_x_;
    Float *sq = (Float *)_nj_ptcl_sq_ptr_;
    NJS_POINT3 view;
    Float depth;
    Float x;
    Float y;

    for (; n > 0; n--, p = (const NJS_POINT3 *)((const Uint8 *)p + stride)) {
        njCalcPoint(NULL, (NJS_POINT3 *)p, &view);
        if (_nj_near_clip_ > view.z || view.z > _nj_far_clip_) {
            continue;
        }
        depth = 1.0F / view.z;
        x = (view.x * scale_x * depth) + _nj_screen_.cx;
        y = (view.y * scale_y * depth) + _nj_screen_.cy;
        *(Uint32 *)&sq[0] = KM_VERTEXPARAM_ENDOFSTRIP;
        sq[1] = x - half_x;
        sq[2] = y + h;
        sq[3] = depth;
        sq[4] = x - half_x;
        sq[5] = y - h;
        sq[6] = depth;
        sq[7] = x + half_x;
        prefetch(sq);
        sq[8] = y - h;
        sq[9] = depth;
        sq[10] = x + half_x;
        sq[11] = y + h;
        prefetch(&sq[8]);
        sq += SQUARE_WORDS;
    }
    _nj_ptcl_sq_ptr_ = (Uint32 *)sq;
}

void njPtclDrawPolygon(NJS_POINT3 *p, Sint32 n, Float h)
{
    ptcl_squares(p, n, h, sizeof(NJS_POINT3));
}

void njPtclDrawPolygon2(NJS_POINT3 *p, Sint32 n, Float h)
{
    ptcl_squares(p, n, h, 2 * sizeof(NJS_POINT3));
}
#else
/* clang-format off */
/* FTRV transforms each point in FV0 by XMTRX. FLDI1 then sets FR3 back to 1,
 * so FDIV makes 1 / z whatever XMTRX's last column holds. 1 / z is the depth
 * of the square's corners. with (X, Y) the point on the screen, the corners
 * run (X - h, Y + h), (X - h, Y - h), (X + h, Y - h) and (X + h, Y + h).
 *
 * a square fills a 32-byte block of the store queue and the first 16 bytes
 * of the next. a PREF sends each block. the pool keeps a word of
 * _nj_2d_clip_ that nothing loads, as the image's pool does */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c116ce8 follows the body's own
 * return at 0x8c116cc4 and its pool */
#pragma inline_asm(njPtclDrawPolygon)
void njPtclDrawPolygon(NJS_POINT3 *p, Sint32 n, Float h)
{
        .IMPORT     __nj_near_clip_
        .IMPORT     __nj_far_clip_
        .IMPORT     __nj_screen_
        .IMPORT     __nj_scr_aspect_x_
        .IMPORT     __nj_scr_aspect_y_
        .IMPORT     __nj_2d_clip_
        MOV.L       ?near_clip,R1
        FMOV.S      @R1,FR5
        MOV.L       ?far_clip,R1
        FMOV.S      @R1,FR6
        MOV.L       ?screen,R1
        MOV.L       ?aspect_x,R2
        MOV.L       ?aspect_y,R3
        FMOV.S      @R1,FR0
        ADD         #12,R1
        FMOV.S      @R2,FR7
        FMOV.S      @R3,FR8
        FMOV.S      @R1+,FR9
        FMUL        FR0,FR7
        FMOV.S      @R1+,FR10
        FMUL        FR0,FR8
        MOV.L       ?sq_ptr,R3
        MOV.L       ?end_of_strip,R2
        MOV.L       @R3,R1
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FLDI1       FR3
        FTRV        XMTRX,FV0
        MOV         R4,R0
        ADD         #12,R0
        FLDI1       FR3
        FDIV        FR2,FR3
?next:
        PREF        @R0
        FCMP/GT     FR2,FR5
        BT          ?skip
        FCMP/GT     FR6,FR2
        BT          ?skip
        FMUL        FR7,FR0
        FMUL        FR3,FR0
        FADD        FR9,FR0
        FMUL        FR8,FR1
        FMUL        FR3,FR1
        FADD        FR10,FR1
        FADD        FR4,FR0
        ADD         #32,R1
        FMOV.S      FR0,@-R1
        FSUB        FR4,FR1
        FMOV.S      FR3,@-R1
        FSUB        FR4,FR0
        FMOV.S      FR1,@-R1
        FSUB        FR4,FR0
        FMOV.S      FR0,@-R1
        FADD        FR4,FR1
        FMOV.S      FR3,@-R1
        FADD        FR4,FR1
        FMOV.S      FR1,@-R1
        FMOV.S      FR0,@-R1
        MOV.L       R2,@-R1
        PREF        @R1
        FADD        FR4,FR0
        ADD         #48,R1
        FADD        FR4,FR0
        FMOV.S      FR1,@-R1
        FSUB        FR4,FR1
        FMOV.S      FR0,@-R1
        FSUB        FR4,FR1
        FMOV.S      FR3,@-R1
        FMOV.S      FR1,@-R1
?skip:
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FLDI1       FR3
        BT/S        ?count
        FTRV        XMTRX,FV0
        PREF        @R1
        ADD         #32,R1
?count:
        DT          R5
        ADD         #12,R0
        FLDI1       FR3
        BF/S        ?next
        FDIV        FR2,FR3
        RTS
        MOV.L       R1,@R3
?near_clip:
        .DATA.L     __nj_near_clip_
?far_clip:
        .DATA.L     __nj_far_clip_
?screen:
        .DATA.L     __nj_screen_
?aspect_x:
        .DATA.L     __nj_scr_aspect_x_
?aspect_y:
        .DATA.L     __nj_scr_aspect_y_
        .DATA.L     __nj_2d_clip_
?sq_ptr:
        .DATA.L     __nj_ptcl_sq_ptr_
?end_of_strip:
        .DATA.L     H'F0000000
}

/* the body of njPtclDrawPolygon, for points 24 bytes apart */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c116dac follows the body's own
 * return at 0x8c116d88 and its pool */
#pragma inline_asm(njPtclDrawPolygon2)
void njPtclDrawPolygon2(NJS_POINT3 *p, Sint32 n, Float h)
{
        MOV.L       ?near_clip,R1
        FMOV.S      @R1,FR5
        MOV.L       ?far_clip,R1
        FMOV.S      @R1,FR6
        MOV.L       ?screen,R1
        MOV.L       ?aspect_x,R2
        MOV.L       ?aspect_y,R3
        FMOV.S      @R1,FR0
        ADD         #12,R1
        FMOV.S      @R2,FR7
        FMOV.S      @R3,FR8
        FMOV.S      @R1+,FR9
        FMUL        FR0,FR7
        FMOV.S      @R1+,FR10
        FMUL        FR0,FR8
        MOV.L       ?sq_ptr,R3
        MOV.L       ?end_of_strip,R2
        MOV.L       @R3,R1
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FLDI1       FR3
        FTRV        XMTRX,FV0
        ADD         #12,R4
        MOV         R4,R0
        ADD         #24,R0
        FLDI1       FR3
        FDIV        FR2,FR3
?next:
        PREF        @R0
        FCMP/GT     FR2,FR5
        BT          ?skip
        FCMP/GT     FR6,FR2
        BT          ?skip
        FMUL        FR7,FR0
        FMUL        FR3,FR0
        FADD        FR9,FR0
        FMUL        FR8,FR1
        FMUL        FR3,FR1
        FADD        FR10,FR1
        FADD        FR4,FR0
        ADD         #32,R1
        FMOV.S      FR0,@-R1
        FSUB        FR4,FR1
        FMOV.S      FR3,@-R1
        FSUB        FR4,FR0
        FMOV.S      FR1,@-R1
        FSUB        FR4,FR0
        FMOV.S      FR0,@-R1
        FADD        FR4,FR1
        FMOV.S      FR3,@-R1
        FADD        FR4,FR1
        FMOV.S      FR1,@-R1
        FMOV.S      FR0,@-R1
        MOV.L       R2,@-R1
        PREF        @R1
        FADD        FR4,FR0
        ADD         #48,R1
        FADD        FR4,FR0
        FMOV.S      FR1,@-R1
        FSUB        FR4,FR1
        FMOV.S      FR0,@-R1
        FSUB        FR4,FR1
        FMOV.S      FR3,@-R1
        FMOV.S      FR1,@-R1
?skip:
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FLDI1       FR3
        BT/S        ?count
        FTRV        XMTRX,FV0
        PREF        @R1
        ADD         #32,R1
?count:
        DT          R5
        ADD         #24,R0
        FLDI1       FR3
        ADD         #12,R4
        BF/S        ?next
        FDIV        FR2,FR3
        RTS
        MOV.L       R1,@R3
?near_clip:
        .DATA.L     __nj_near_clip_
?far_clip:
        .DATA.L     __nj_far_clip_
?screen:
        .DATA.L     __nj_screen_
?aspect_x:
        .DATA.L     __nj_scr_aspect_x_
?aspect_y:
        .DATA.L     __nj_scr_aspect_y_
        .DATA.L     __nj_2d_clip_
?sq_ptr:
        .DATA.L     __nj_ptcl_sq_ptr_
?end_of_strip:
        .DATA.L     H'F0000000
}
/* clang-format on */
#endif
#endif
