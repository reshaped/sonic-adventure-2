#include <ninja.h>
#include <nj_global.h>
#include <nj_km.h>
#include <sg_xpt.h>

#ifdef NONMATCHING
/* the body keeps Ninja's name and its arguments. engine/dc/port.h's macro
 * gives the game's calls the width the port's 16:9 asks */
#undef njPtclDrawSprite
#endif

void njPtclSpriteStart(Sint32 texid, Uint32 col, Sint32 flag)
{
    njGetkmPointer();
    njSetkmTextureNum(texid);
    if (flag) {
        _nj_ptcl_sq_ptr_ = njStartVertexTexSpriteTrans(col);
    } else {
        _nj_ptcl_sq_ptr_ = njStartVertexTexSpriteOpaque(col);
    }
}

void njPtclSpriteEnd(void)
{
    njEndVertex(_nj_ptcl_sq_ptr_);
    njSetkmPointer();
}

/* clang-format off */
#ifdef DREAMCAST
/* FTRV transforms each point in FV0 by XMTRX. FLDI1 then sets FR3 back to 1,
 * so FDIV makes 1 / z whatever XMTRX's last column holds. 1 / z is the depth
 * of the sprite's corners. w and h scale by the screen's distance over z.
 * with (X, Y) the point on the screen, the corners run (X - w, Y + h),
 * (X - w, Y - h), (X + w, Y - h) and (X + w, Y + h). the first three take
 * the texture coordinates (0, 1), (0, 0) and (1, 0). a sprite fills two
 * 32-byte blocks of the store queue, and a PREF sends each.
 *
 * the body loads its constants from a pool of its own, after .ALIGN's NOP.
 * a pool asmsh wrote for MOV.L #value would pad with a halfword of 0 where
 * the image holds that NOP */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c102dc8 follows the body's own
 * return at 0x8c102d9a and its pool */
#pragma inline_asm(njPtclDrawSprite)
void njPtclDrawSprite(NJS_POINT3 *p, Sint32 n, Float w, Float h)
{
        .IMPORT     __nj_near_clip_
        .IMPORT     __nj_far_clip_
        .IMPORT     __nj_screen_
        .IMPORT     __nj_scr_aspect_x_
        .IMPORT     __nj_scr_aspect_y_
        FMOV.S      FR13,@-R15
        FMOV.S      FR12,@-R15
        MOV.L       R8,@-R15
        MOV.L       ?near_clip,R1
        FMOV.S      @R1,FR13
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
        MOV.L       ?u0_v1,R6
        FMUL        FR0,FR4
        MOV.L       ?u0_v0,R7
        FMUL        FR0,FR5
        MOV.L       ?u1_v0,R8
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
        FMOV.S      FR4,FR11
        PREF        @R0
        FCMP/GT     FR2,FR13
        BT          ?skip
        FCMP/GT     FR6,FR2
        BT          ?skip
        FMUL        FR7,FR0
        FMUL        FR3,FR0
        FADD        FR9,FR0
        FMOV.S      FR5,FR12
        FMUL        FR8,FR1
        FMUL        FR3,FR1
        FADD        FR10,FR1
        FMUL        FR3,FR11
        FADD        FR11,FR0
        ADD         #32,R1
        FMUL        FR3,FR12
        FMOV.S      FR0,@-R1
        FSUB        FR12,FR1
        FMOV.S      FR3,@-R1
        FSUB        FR11,FR0
        FMOV.S      FR1,@-R1
        FSUB        FR11,FR0
        FMOV.S      FR0,@-R1
        FADD        FR12,FR1
        FMOV.S      FR3,@-R1
        FADD        FR12,FR1
        FMOV.S      FR1,@-R1
        FMOV.S      FR0,@-R1
        MOV.L       R2,@-R1
        PREF        @R1
        ADD         #64,R1
        MOV.L       R8,@-R1
        FADD        FR11,FR0
        MOV.L       R7,@-R1
        FADD        FR11,FR0
        MOV.L       R6,@-R1
        ADD         #-4,R1
        FMOV.S      FR1,@-R1
        FSUB        FR12,FR1
        FMOV.S      FR0,@-R1
        FSUB        FR12,FR1
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
        MOV.L       @R15+,R8
        FMOV.S      @R15+,FR12
        FMOV.S      @R15+,FR13
        RTS
        MOV.L       R1,@R3
        .ALIGN      4
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
?sq_ptr:
        .DATA.L     __nj_ptcl_sq_ptr_
?end_of_strip:
        .DATA.L     H'F0000000
?u0_v1:
        .DATA.L     H'00003F80
?u0_v0:
        .DATA.L     H'00000000
?u1_v0:
        .DATA.L     H'3F800000
}
#endif
/* clang-format on */
