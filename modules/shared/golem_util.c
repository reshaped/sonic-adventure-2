/* helpers the golem fights' objects share: turns, scaled draws, dust puffs
 * and the rumble. stg18/lagoon_util.c and stg25/chamber_util.c hold most of
 * the same code under their own names. golem_sonic_homing_or_bounce is
 * stg28/cave_util.c's cave_sonic_combo_or_bounce with other modes */
#include <mathf.h>
#include <nj_direct.h> /* IWYU pragma: keep -- completes NJS_DIRECT_HEAD */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/golem_util.h"
#include "shared/vertex_colors.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the dust puffs' random turns, in degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* half the width and height of the screen, which is 640 by 480 */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F

#define NEAR_Z 0.001F

#define ALPHA_BLEND (NJD_FBS_SA | NJD_FBD_ISA)
#define STRIP_FLAGS (NJD_FST_UA | NJD_FST_IS | NJD_FST_IL)
/* the debug draws' constant material, a translucent gray */
#define DEBUG_SHADE 0.3F
/* the cylinder and the sphere blend onto what lies behind them */
#define BLEND_CONTROL_SET                                                      \
    (DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE)
#define SPHERE_BLEND (NJD_FBS_ONE | NJD_FBD_ISA)

/* golem_rumble's power is rumble_power while pos lies within RUMBLE_NEAR of
 * player 0. beyond that, it is rumble_power times RUMBLE_NEAR over the
 * distance, cut toward 0 */
#define RUMBLE_NEAR 10.0F
static int rumble_power = 18;
static int rumble_freq = 20;
static int rumble_time = 1;

/* no header declares these, and no call to them is left in the image:
 * golem_on_screen takes golem_point_to_screen inline. they are external only
 * because SHC leaves out a static function with no call left to it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* whether task's player is a Sonic that homes in or bounces. the player is
 * the one player_slot_of finds for task, which must find one. its work and
 * record must not be null, unchecked */
Bool golem_sonic_homing_or_bounce(struct task *task);
/* draw model scaled, as shared/golem_util.h's scaled draws do. the plain
 * calls scale by scale along each axis. the _v call scales by scale's x, y
 * and z. golem_direct_draw_scaled draws a build with njDirectDrawObject, as
 * golem_direct_draw_scaled_v does. the _tinted calls draw with
 * model_draw_tinted */
void golem_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale);
void golem_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                    Float scale);
void golem_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                      NJS_VECTOR *scale);
/* draw object with njCnkEasyDrawObject, model's first vertex chunk tinted as
 * model_draw_tinted tints it, and each strip given NJD_FST_UA. the tinted copy
 * lies at WORK_AREA, over what was there. where color is COLOR_NONE or the
 * chunk isn't NJD_CV_D8 with no flags, object draws as it is. the call puts
 * back the control bits and the strip flags it changed */
void golem_object_draw_tinted(NJS_CNK_OBJECT *object, NJS_CNK_MODEL *model,
                              Uint32 color);
/* put in *sx and *sy where point lands on camera 0's screen, as
 * view_to_screen puts it. the call takes point through the current matrix,
 * and leaves it as it was */
void golem_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy);
/* whether golem_point_to_screen puts point on camera 0's screen. on the
 * screen means less than SCREEN_HALF_WIDTH across and SCREEN_HALF_HEIGHT up
 * or down from the middle. the call doesn't test z, so a point behind the
 * camera can pass */
Bool golem_on_screen(NJS_POINT3 *point);
/* draw a debug model of camera/camera.h in a translucent gray, under the
 * current matrix. golem_sphere_draw draws a sphere of radius scale, blended
 * onto what lies behind it. golem_box_draw draws a box that reaches scale's x,
 * y and z to each side of the origin.
 *
 * each call puts back the control bits and the strip flags it changed. it
 * leaves njSetCnkBlendMode's blend as it set it, and the current matrix as it
 * found it. golem_sphere_draw leaves the constant material at 0, and
 * golem_box_draw leaves it gray. Ninja's matrix stack must have room for one
 * more matrix, unchecked */
void golem_sphere_draw(Float scale);
void golem_box_draw(NJS_VECTOR *scale);
/* NOLINTEND(misc-use-internal-linkage) */

Bool golem_sonic_homing_or_bounce(struct task *task)
{
    int slot = player_slot_of(task);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    int mode;

    if (record->character != PLAYER_SONIC) {
        return FALSE;
    }
    mode = work->mode;
    return mode == PLAYER_MODE_SONIC_HOMING || mode == PLAYER_MODE_SONIC_BOUNCE;
}

void golem_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void golem_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                       Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    struct angles ang;

    offset.x = 0.0F;
    offset.y = 0.0F;
    offset.z = radius;
    njPushMatrixEx();
    njRotateY(NULL, (Angle)((((Float)rand() * (TURN_Y_SPREAD * RAND_SCALE)) +
                             TURN_Y_MIN) *
                            ANGLE_PER_DEGREE));
    njRotateX(NULL, (Angle)((Float)rand() *
                            (TILT_MAX * ANGLE_PER_DEGREE * RAND_SCALE)));
    current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateXYZ(NULL, ang.x, ang.y, ang.z);
    njCalcPoint(NULL, &offset, &point);
    dust_create(&point, spd, scale);
    njPopMatrixEx();
}

static void golem_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

void golem_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

void golem_simple_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkSimpleDrawModel(model);
    *radius = saved;
}

void golem_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njDirectDrawObject(model);
    *radius = saved;
}

void golem_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    njDirectDrawObject(model);
    *radius = saved;
}

void golem_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                    Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    model_draw_tinted(model, color);
    *radius = saved;
}

void golem_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                      NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    model_draw_tinted(model, color);
    *radius = saved;
}

void golem_object_draw_tinted(NJS_CNK_OBJECT *object, NJS_CNK_MODEL *model,
                              Uint32 color)
{
    Sint32 *vlist = model->vlist;
    Sint32 *copy;

    if (color != COLOR_NONE && (Uint16)*vlist == NJD_CV_D8) {
        copy = (Sint32 *)WORK_AREA;
        vlist_copy_shaded(copy, vlist, color, VLIST_UNREAD_MASK);
        model->vlist = copy;
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        njCnkEasyDrawObject(object);
        draw_attr_restore();
        draw_control_restore();
        model->vlist = vlist;
    } else {
        njCnkEasyDrawObject(object);
    }
}

void golem_shape_motion_draw_scaled_v(NJS_CNK_OBJECT *object,
                                      NJS_MOTION *motion, NJS_MOTION *shape,
                                      Float frame, NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &object->model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    njCnkEasyDrawShapeMotion(object, motion, shape, frame);
    *radius = saved;
}

/* put point, in the camera's space, on camera 0's screen, in *sx and *sy from
 * the screen's middle. a z of 0 first becomes NEAR_Z, in point itself */
static void view_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    Float view = camera_view_angle(0);
    Float distance = SCREEN_HALF_WIDTH / njTan((Angle)(view * 0.5F));

    if (point->z == 0.0F) {
        point->z = NEAR_Z;
    }
    /* @hack spelling: x over -z, negated, where x over z would do */
    *sx = -(point->x / -point->z * distance);
    *sy = point->y / -point->z * distance;
}

#pragma inline(point_to_view)
static void point_to_view(NJS_POINT3 *from, NJS_POINT3 *to)
{
    njCalcPoint(NULL, from, to);
}

#pragma inline(golem_point_to_screen)
void golem_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    NJS_POINT3 view;

    point_to_view(point, &view);
    view_to_screen(&view, sx, sy);
}

Bool golem_on_screen(NJS_POINT3 *point)
{
    Float sx;
    Float sy;

    golem_point_to_screen(point, &sx, &sy);
    if (njAbs(sx) < SCREEN_HALF_WIDTH && njAbs(sy) < SCREEN_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

#pragma inline(golem_angle_turn)
Angle golem_angle_turn(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

Angle golem_heading(Float x, Float z)
{
    /* @hack type: the turn masked to its full word before its cast, where the
     * cast alone would do */
    return (Sint16)((QUARTER_TURN - (Angle)(atan2f(z, x) * ANGLE_PER_RADIAN)) &
                    0xffffffff);
}

void golem_sphere_draw(Float scale)
{
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius = &debug_models[DEBUG_MODEL_SPHERE]->model->r;
    Float saved = *radius;

    /* golem_model_draw_scaled scales r by scale again, so the draw's clip
     * test takes r times scale squared */
    *radius *= scale;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(BLEND_CONTROL_SET);
    njSetCnkBlendMode(SPHERE_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    constant_material_set(DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE);
    njPushMatrixEx();
    golem_model_draw_scaled(debug_models[DEBUG_MODEL_SPHERE]->model, scale);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void golem_cylinder_draw(NJS_VECTOR *scale)
{
    NJS_VECTOR size;
    Float largest;
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius;
    Float saved;

    size = *scale;
    largest = size.x > size.y ? size.x : size.y;
    largest = size.z > largest ? size.z : largest;
    radius = &debug_models[DEBUG_MODEL_CYLINDER]->model->r;
    saved = *radius;
    *radius *= largest;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(BLEND_CONTROL_SET);
    njSetCnkBlendMode(ALPHA_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    constant_material_set(DEBUG_SHADE, 1.0F, 1.0F, 1.0F);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, size.y * 0.5F, 0.0F);
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_CYLINDER]);
    njPopMatrixEx();
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void golem_box_draw(NJS_VECTOR *scale)
{
    NJS_VECTOR size;
    Float largest;
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius;
    Float saved;

    size.x = scale->x * 2.0F;
    size.y = scale->y * 2.0F;
    size.z = scale->z * 2.0F;
    largest = size.x > size.y ? size.x : size.y;
    largest = size.z > largest ? size.z : largest;
    radius = &debug_models[DEBUG_MODEL_BOX]->model->r;
    saved = *radius;
    *radius *= largest;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetCnkBlendMode(ALPHA_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    constant_material_set(DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE);
    njPushMatrixEx();
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_BOX]);
    njPopMatrixEx();
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void golem_angle_approach(Angle *ang, Angle target, int step)
{
    int turn;

    turn = golem_angle_turn(*ang, target);
    if (njAbs((Float)turn) <= (Float)step) {
        *ang = target;
    } else {
        *ang += turn > 0 ? step : -step;
    }
}

Float golem_weighted_mean(Float from, Float to, Float to_weight,
                          Float from_weight)
{
    return ((from * from_weight) + (to * to_weight)) /
           (to_weight + from_weight);
}

Sint32 golem_size_class_flags(Float size)
{
    /* @hack spelling: the tests chained with else, where each return ending
     * its branch would do */
    if (size > WORLD_SIZE_CLASS_1_MAX) {
        return WORLD_SIZE_CLASS(0);
    } else if (size > WORLD_SIZE_CLASS_2_MAX) {
        return WORLD_SIZE_CLASS(1);
    } else if (size > WORLD_SIZE_CLASS_3_MAX) {
        return WORLD_SIZE_CLASS(2);
    } else {
        return WORLD_SIZE_CLASS(3);
    }
}

void golem_dust_smoke_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                             Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    struct angles ang;

    offset.x = 0.0F;
    offset.y = 0.0F;
    offset.z = radius;
    njPushMatrixEx();
    njRotateY(NULL, (Angle)((((Float)rand() * (TURN_Y_SPREAD * RAND_SCALE)) +
                             TURN_Y_MIN) *
                            ANGLE_PER_DEGREE));
    njRotateX(NULL, (Angle)((Float)rand() *
                            (TILT_MAX * ANGLE_PER_DEGREE * RAND_SCALE)));
    current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateXYZ(NULL, ang.x, ang.y, ang.z);
    njCalcPoint(NULL, &offset, &point);
    dust_smoke_create(&point, spd, scale);
    njPopMatrixEx();
}

void golem_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point)
{
    Float sine;
    Float cosine;

    sine = njSin(ang);
    cosine = njCos(ang);
    point->x = (x * cosine) + (z * sine);
    point->z = (z * cosine) - (x * sine);
}

void golem_rumble(NJS_POINT3 *pos, Float size, int steps)
{
    Float distance;
    int power;

    distance = njDistanceP2P(pos, &player_work[0]->pos);
    power = rumble_power;
    if (distance > 1.0F) {
        size /= distance;
    }
    if (distance > RUMBLE_NEAR) {
        power = power / (distance * (1.0F / RUMBLE_NEAR));
    }
    camera_shake(0, steps, size);
    vibration_start(0, power, rumble_freq, rumble_time);
}
