/* helpers the golem fights' objects share: turns, scaled draws, dust puffs
 * and the rumble */
#ifndef SHARED_GOLEM_UTIL_H
#define SHARED_GOLEM_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void golem_polar_xz(Angle ang, Float radius, Float *x, Float *z);

/* draw model scaled. the _v calls scale by scale's x, y and z, and
 * golem_simple_draw_scaled by scale along each axis. golem_model_draw_scaled_v
 * draws with njCnkEasyDrawModel, and golem_simple_draw_scaled with
 * njCnkSimpleDrawModel. golem_direct_draw_scaled_v draws with
 * njDirectDrawObject a build, as shared/common_object.h's model_build makes
 * one.
 *
 * each call scales the current matrix, as njScale does, and leaves it so. for
 * the draw, model->r is scaled by the largest factor. any clip test in the draw
 * takes the scaled r. the call then puts r back */
void golem_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void golem_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale);
void golem_simple_draw_scaled(NJS_CNK_MODEL *model, Float scale);
/* draw object's shape motion at frame with njCnkEasyDrawShapeMotion, scaled
 * as golem_model_draw_scaled_v scales. the r it scales is object->model's,
 * which must not be null, unchecked */
void golem_shape_motion_draw_scaled_v(NJS_CNK_OBJECT *object,
                                      NJS_MOTION *motion, NJS_MOTION *shape,
                                      Float frame, NJS_VECTOR *scale);

/* draw the debug cylinder of camera/camera.h at scale, under the current
 * matrix, raised by half its scaled height. it draws white at an alpha of 0.3,
 * blended onto what lies behind it. the call puts back the control bits and
 * the strip flags it changed. it leaves the constant material as it set it,
 * and the current matrix as it found it. Ninja's matrix stack must have room
 * for one more matrix, unchecked */
void golem_cylinder_draw(NJS_VECTOR *scale);

/* the short turn from from to to, -0x8000 to 0x7fff */
Angle golem_angle_turn(Angle from, Angle to);
/* the angle about y that turns the z axis toward the point (x, 0, z), -0x8000
 * to 0x7fff */
Angle golem_heading(Float x, Float z);
/* move *ang toward target by step. where the short turn to target is step or
 * less, *ang becomes target. step must not be negative */
void golem_angle_approach(Angle *ang, Angle target, int step);
/* the mean of from and to, from weighted by from_weight and to by to_weight.
 * the weights' sum must not be 0 */
Float golem_weighted_mean(Float from, Float to, Float to_weight,
                          Float from_weight);
/* the WORLD_SIZE_CLASS flags of the class that size falls in, by
 * collision/world.h's bounds */
Sint32 golem_size_class_flags(Float size);
/* make a puff of speed spd and size scale. golem_dust_smoke_behind makes it
 * with dust_smoke_create, and golem_dust_behind with dust_create. it lies at
 * pos plus an offset of (0, 0, radius). the offset first turns about x by a
 * random 0 to 60 degrees. it then turns about y by a random 120 to 240
 * degrees. it last takes the current matrix's turn, as
 * current_matrix_angles_xyz reads it.
 *
 * Ninja's matrix stack must have room for one more matrix, unchecked. the
 * call leaves the current matrix as it found it */
void golem_dust_smoke_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                             Float scale);
void golem_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                       Float scale);
/* turn (x, 0, z) by ang about y, as njRotateY turns a point. point->x and
 * point->z take the result's x and z. point->y keeps its value */
void golem_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point);
/* shake camera 0 for steps steps, and vibrate player 0's pad. both weaken
 * the further pos lies from player 0. the shake's size is size, divided by
 * the distance where that is above 1. the call passes vibration_start a power
 * of 18. where the distance is above 10, the power is 180 over the distance,
 * cut toward 0. player_work[0] must not be null, unchecked */
void golem_rumble(NJS_POINT3 *pos, Float size, int steps);

#endif
