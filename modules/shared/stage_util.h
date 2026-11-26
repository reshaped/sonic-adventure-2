/* helpers the stages share: turns about y, chunk model draws, a dust puff
 * and a pad's vibration */
#ifndef SHARED_STAGE_UTIL_H
#define SHARED_STAGE_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void polar_xz(Angle ang, Float radius, Float *x, Float *z);
/* make a puff with dust_create, of speed spd and size scale. it lies at pos
 * plus an offset of (0, 0, radius). the offset first turns about x by a random
 * 0 to 60 degrees. it then turns about y by a random 120 to 240 degrees. it
 * last takes the current matrix's turn, as current_matrix_angles_xyz reads
 * it.
 *
 * Ninja's matrix stack must have room for one more matrix, unchecked. the
 * call leaves the current matrix as it found it */
void dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius, Float scale);

/* draw model scaled. model_draw_scaled and modifier_draw_scaled scale by
 * scale along each axis. the _v calls scale by scale's x, y and z.
 * model_draw_scaled and model_draw_scaled_v draw with njCnkEasyDrawModel,
 * modifier_draw_scaled with njCnkModDrawModel, and model_draw_tinted_scaled_v
 * with model_draw_tinted.
 *
 * each call scales the current matrix, as njScale does, and leaves it so.
 * for the draw, model->r is scaled by the largest factor. any clip test in the
 * draw takes the scaled r. the call then puts r back */
void model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                NJS_VECTOR *scale);
void modifier_draw_scaled(NJS_CNK_MODEL *model, Float scale);

/* turn (x, 0, z) by ang about y, as njRotateY turns a point. point->x and
 * point->z take the result's x and z. point->y keeps its value */
void point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point);

/* draw model with njCnkSimpleDrawModel, adding its colors to those drawn
 * already. each material's diffuse color is (alpha, red, green, blue). each
 * strip's flags become NJD_FST_UA, NJD_FST_IS and NJD_FST_IL in place of its
 * own. model_draw_tinted_additive draws the same way through
 * model_draw_tinted, under the caller's constant material.
 *
 * each call puts back _nj_control_3d_flag_ and njSetConstantAttr's masks. it
 * saves them with draw_control_save and draw_attr_save, so a caller's own
 * save is lost. it leaves the constant material at 0. it leaves
 * njSetCnkBlendMode's blend at NJD_FBS_SA | NJD_FBD_ISA. it leaves
 * njColorBlendingMode's factors at NJD_COLOR_BLENDING_SRCALPHA for the source
 * and NJD_COLOR_BLENDING_INVSRCALPHA for the destination */
void model_draw_material_additive(NJS_CNK_MODEL *model, Float alpha, Float red,
                                  Float green, Float blue);
void model_draw_tinted_additive(NJS_CNK_MODEL *model, Uint32 color);

/* draw object with njCnkEasyDrawShapeMotion, its own model tinted by color as
 * model_draw_tinted of shared/common_object.h tints a model. the tinted copy
 * lies at WORK_AREA, over what was there. the models of object's children
 * draw as they are.
 *
 * object's model must not be null, unchecked. a chunk the call tints must
 * hold a vertex or more, unchecked. NJD_CE must follow that chunk, or the call
 * never returns */
void shape_motion_draw_tinted(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              NJS_MOTION *shape, Float frame, Uint32 color);

/* return the WORLD_SIZE_CLASS flags of the class that size falls in.
 * collision/world.h gives each class's bounds */
Sint32 world_size_class_flags(Float size);

/* set the color of the first count vertices of vlist's first chunk, an
 * NJD_CV_D8 chunk. count must lie from 0 to the chunk's own count, which the
 * call doesn't read */
void vlist_set_colors(Sint32 *vlist, int count, Uint32 color);

/* pulse player's pad count times, interval steps apart. each pulse is
 * vibration_start(player, power, freq, 0). a task named VibSlightExec, at task
 * level 2, makes the pulses. it pulses on its first step, and ends interval - 1
 * steps after its last pulse. an interval of 0 counts as 1. where the heap
 * can't give the task, nothing pulses.
 *
 * count must be 1 or more, interval 0 or more, and power 0 or more. with a
 * count of 0 and an interval above 1, the task pulses once. with a count of 0
 * and an interval of 0 or 1, it pulses every step forever. the task keeps
 * power in a Uint16, so a negative power reaches vibration_start as 65536
 * plus power */
void vib_slight_create(int player, int power, int freq, int interval,
                       int count);

#endif
