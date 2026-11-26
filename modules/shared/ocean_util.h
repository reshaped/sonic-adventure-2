/* helpers Sand Ocean's objects share. they draw scaled and tinted models, and
 * raise dust. they turn a point about y, and shake the camera */
#ifndef SHARED_OCEAN_UTIL_H
#define SHARED_OCEAN_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

/* the texture list of the stage's PVM stg21_fire: OCEAN_FIRE_FRAMES frames of
 * flame, which the flames show in turn. the stage's data holds it, and no
 * source defines it */
extern NJS_TEXLIST ocean_fire_texlist;
#define OCEAN_FIRE_FRAMES 16

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void ocean_polar_xz(Angle ang, Float radius, Float *x, Float *z);

/* raise a puff with dust_create of shared/dust.h, of speed spd and size scale.
 * it lies radius from pos, behind pos as the current matrix faces. the offset
 * from pos starts along z. it turns by a random 0 to 60 degrees about x. it
 * then turns by a random 120 to 240 degrees about y, which points it backward.
 * it last turns as the current matrix does, by the angles
 * current_matrix_angles_xyz reads. the call leaves the current matrix as it
 * was. Ninja's matrix stack must have room for one more matrix */
void ocean_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                       Float scale);

/* draw model scaled by scale along each axis. ocean_model_draw_scaled draws
 * with njCnkEasyDrawModel, ocean_model_draw_simple_scaled with
 * njCnkSimpleDrawModel, and ocean_model_draw_tinted_scaled with
 * model_draw_tinted of shared/common_object.h. a _v call scales each axis by
 * its own part of scale. it draws as its twin without _v does.
 *
 * each call scales the current matrix as njScale does, and leaves it scaled.
 * model's r, which a clip test reads, is scaled by the largest factor for the
 * draw */
void ocean_model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void ocean_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void ocean_model_draw_simple_scaled(NJS_CNK_MODEL *model, Float scale);
void ocean_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                    Float scale);
void ocean_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                      NJS_VECTOR *scale);

/* turn (x, 0, z) by ang about y, as njRotateY turns a point. *turned_x and
 * *turned_z take the result's x and z */
void ocean_turn_xz(Float x, Float z, Angle ang, Float *turned_x,
                   Float *turned_z);

/* draw model with njCnkEasyDrawModel, from a copy of its first vertex chunk in
 * 0x300 words of the stack. the copy's vertex colors are tinted by color, as
 * model_draw_tinted tints them. the chunk must be NJD_CV_D8. the call never
 * returns where the chunk's size passes 0x2fc words, or where NJD_CE doesn't
 * follow the chunk */
void ocean_model_draw_tinted_stack(NJS_CNK_MODEL *model, Uint32 color);
/* tint the vertex colors of model's first chunk in place, as
 * ocean_model_draw_tinted_stack tints its copy. the chunk must be NJD_CV_D8.
 * where NJD_CE doesn't follow the chunk, the call traps with trapa(0) */
void ocean_model_tint(NJS_CNK_MODEL *model, Uint32 color);

/* shake camera 0 with camera_shake for steps. then vibrate player 0's pad with
 * vibration_start, of frequency 20 and time 1. let d be the distance from pos
 * to player 0's pos. past a d of 1, the shake's size is size / d. the
 * vibration's power is 18 up to a d of 10. past it the power is 18 / (0.1 * d),
 * truncated. the call does nothing while camera 0 still shakes with a
 * shake_size above the size after d's cut. player_work[0] must not be null */
void ocean_camera_shake_at(NJS_POINT3 *pos, Float size, int steps);

#endif
