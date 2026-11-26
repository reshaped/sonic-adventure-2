#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side stores in C, field by field, the values the block below stores
 * from ly.z down to px */
void njInitCamera(NJS_CAMERA *camera)
{
    camera->px = 0.0F;
    camera->py = 0.0F;
    camera->pz = 0.0F;
    camera->vx = 0.0F;
    camera->vy = 0.0F;
    camera->vz = -1.0F;
    camera->roll = 0;
    camera->ang = NJM_DEG_ANG(60);
    camera->n_clip = -1.0F;
    camera->f_clip = -60000.0F;
    camera->lx.x = 1.0F;
    camera->lx.y = 0.0F;
    camera->lx.z = 0.0F;
    camera->ly.x = 0.0F;
    camera->ly.y = 1.0F;
    camera->ly.z = 0.0F;
}
#else
/* R4 points one camera past the destination. The stores run from ly.z to px. */
/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c125a68 follows the body's own
 * return at 0x8c125a5c and its pool */
#pragma inline_asm(initialize_previous_camera)
static void initialize_previous_camera(NJS_CAMERA *end)
{
        FLDI0       FR0
        FLDI1       FR1
        FMOV.S      FR0,@-R4
        FLDI1       FR2
        FMOV.S      FR1,@-R4
        FNEG        FR2
        FMOV.S      FR0,@-R4
        MOVA        #H'C76A6000,R0
        FMOV.S      FR0,@-R4
        FMOV.S      @R0,FR3
        FMOV.S      FR0,@-R4
        MOV.W       #H'2AAA,R2
        FMOV.S      FR1,@-R4
        MOV         #0,R1
        FMOV.S      FR3,@-R4
        FMOV.S      FR2,@-R4
        MOV.L       R2,@-R4
        MOV.L       R1,@-R4
        FMOV.S      FR2,@-R4
        FMOV.S      FR0,@-R4
        FMOV.S      FR0,@-R4
        FMOV.S      FR0,@-R4
        FMOV.S      FR0,@-R4
        RTS
        FMOV.S      FR0,@-R4
}
/* clang-format on */

void njInitCamera(NJS_CAMERA *camera)
{
    /* @hack spelling: naming the end pointer keeps this function frameless
     * for the inline body's return */
    NJS_CAMERA *end = camera + 1;

    initialize_previous_camera(end);
}
#endif
#endif
