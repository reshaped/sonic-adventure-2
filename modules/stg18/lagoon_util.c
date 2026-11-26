/* helpers Dry Lagoon's objects share */
#include <mathf.h>
#include <nj_direct.h> /* IWYU pragma: keep -- completes NJS_DIRECT_HEAD */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "stg18/lagoon_util.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the ranges of lagoon_dust_behind's random turns about y and about x, in
 * degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* a land block mask the switches on lagoon_camera_blocks take, though it
 * never returns it */
#define OTHER_BLOCKS 0x2000
/* the bounds of the places lagoon_camera_blocks tells apart */
#define SOUTH_Z 460.0F
#define SOUTH_EAST_X 610.0F
#define WEST_X (-250.0F)
#define EAST_X 750.0F
#define LOW_WEST_X 137.0F
#define LOW_SOUTH_Z 1100.0F
#define LOW_Y 120.0F
#define HALL_WEST_X 137.0F
#define HALL_EAST_X 263.0F
#define HALL_NORTH_Z 1453.0F
#define HALL_UPPER_Z 1200.0F
#define HALL_UPPER_Y 190.0F

#define LOW_WATER 0.0F
#define MIDDLE_WATER 180.0F
#define HIGH_WATER 245.0F

/* half the width and height of the screen, which is 640 by 480 */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F
/* lagoon_view_dips_below's view. VIEW_HALF_ANGLE is half its angle across, in
 * degrees. VIEW_DEPTH is the z of the point it tests, in front of the camera. a
 * camera more than ABOVE_DEPTHS times |VIEW_DEPTH| above height can't have the
 * point below it */
#define VIEW_HALF_ANGLE 35.0F
#define VIEW_DEPTH (-3.0F)
#define ABOVE_DEPTHS 3.0F

/* the debug models' blend, and the flags their constant attr gives each
 * strip: translucent, unlit and without its specular part */
#define SHAPE_BLEND (NJD_FBS_SA | NJD_FBD_ISA)
#define SHAPE_ATTR_OR (NJD_FST_UA | NJD_FST_IS | NJD_FST_IL)
/* the constant material's alpha and each of its colors, a translucent
 * gray */
#define SHAPE_SHADE 0.3F
/* lagoon_sphere_draw blends its sphere onto what lies behind it */
#define SPHERE_CONTROL_SET                                                     \
    (DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE)
#define SPHERE_BLEND (NJD_FBS_ONE | NJD_FBD_ISA)
#define CONE_RISE 0.66667F

/* a VibSlightExec task keeps lagoon_vib_slight_create's parameters in its
 * work, and the steps it has run in its unk_3c */
#define PLAYER(work) ((work)->tick)
#define POWER(work) ((work)->timer)
#define FREQ(work) ((work)->ang.x)
#define INTERVAL(work) ((work)->ang.y)
#define PULSES(work) ((work)->ang.z)
#define STEPS(task) ((task)->unk_3c.l)
/* task_create's level for a VibSlightExec task */
#define VIB_SLIGHT_LEVEL 2

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* no header declares these, and no code of the image calls them. they stay
 * external: the image holds their code, and SHC leaves out a static function
 * nothing calls */
/* make a puff with dust_create, of speed spd and size scale. it lies at pos
 * plus an offset of (0, 0, radius). the offset first turns about x by a random
 * 0 to 60 degrees. it then turns about y by a random 120 to 240 degrees. it
 * last takes the current matrix's turn, as current_matrix_angles_xyz reads
 * it.
 *
 * Ninja's matrix stack must have room for one more matrix, unchecked. the
 * call leaves the current matrix as it found it */
void lagoon_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                        Float scale);
/* draw model as lagoon_model_draw_scaled and lagoon_model_draw_scaled_v do,
 * but with model_draw_tinted */
void lagoon_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                     Float scale);
void lagoon_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                       NJS_VECTOR *scale);
/* draw a debug model of camera/camera.h in a translucent gray, at the current
 * matrix. lagoon_box_draw draws DEBUG_MODEL_BOX centered, with scale's x, y
 * and z for its half sizes. lagoon_cylinder_draw draws DEBUG_MODEL_CYLINDER
 * standing on the origin, scale->y high, with scale's x and z for its radii.
 * lagoon_sphere_draw draws DEBUG_MODEL_SPHERE of radius scale.
 * lagoon_cone_draw draws DEBUG_MODEL_CONE at twice scale, turned half a turn
 * about x. it raises the cone by CONE_RISE of its scaled height.
 *
 * each call puts back _nj_control_3d_flag_ and njSetConstantAttr's masks. it
 * leaves njSetCnkBlendMode's blend as it set it. lagoon_sphere_draw leaves the
 * constant material at 0, and the rest leave it gray */
void lagoon_box_draw(NJS_VECTOR *scale);
void lagoon_cone_draw(NJS_VECTOR *scale);
void lagoon_cylinder_draw(NJS_VECTOR *scale);
void lagoon_sphere_draw(Float scale);
/* NOLINTEND(misc-use-internal-linkage) */

Bool rouge_attacking(struct task *task)
{
    int slot = player_slot_of(task);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    int mode;

    if (record->character != PLAYER_ROUGE) {
        return FALSE;
    }
    mode = work->mode;
    return mode == PLAYER_MODE_HUNTER_GLIDE ||
           mode == PLAYER_MODE_HUNTER_ATTACK_1 ||
           mode == PLAYER_MODE_HUNTER_ATTACK_2 ||
           mode == PLAYER_MODE_HUNTER_ATTACK_3 ||
           mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1 ||
           mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2 ||
           mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3 ||
           mode == PLAYER_MODE_HUNTER_SPIRAL || mode == PLAYER_MODE_HUNTER_DIVE;
}

void lagoon_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void lagoon_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                        Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[12];
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

void lagoon_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
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

void lagoon_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
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

void lagoon_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale)
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

void lagoon_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale)
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

void lagoon_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
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

void lagoon_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
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

Sint32 lagoon_camera_blocks(void)
{
    NJS_POINT3 pos;

    pos = camera_views[camera_current_index]->pos;
    if (pos.z < SOUTH_Z) {
        if (pos.x > SOUTH_EAST_X) {
            return SOUTH_EAST_BLOCKS;
        }
        return SOUTH_WEST_BLOCKS;
    } else if (pos.x < WEST_X) {
        return WEST_BLOCKS;
    } else if (pos.x > EAST_X) {
        return EAST_BLOCKS;
    }
    if ((pos.x < LOW_WEST_X && pos.z < LOW_SOUTH_Z) || pos.y < LOW_Y) {
        return LOW_BLOCKS;
    }
    /* @hack redundant: the test that z is HALL_UPPER_Z or more, which the
     * test before it settles */
    if (pos.x > HALL_WEST_X && pos.x < HALL_EAST_X && pos.z < HALL_NORTH_Z &&
        (pos.z < HALL_UPPER_Z ||
         (pos.z >= HALL_UPPER_Z && pos.y < HALL_UPPER_Y))) {
        return HALL_BLOCKS;
    }
    return MIDDLE_BLOCKS;
}

void lagoon_land_clip(void)
{
    switch (lagoon_camera_blocks()) {
    case SOUTH_WEST_BLOCKS:
        game_state.land_block_mask[0] = SOUTH_WEST_BLOCKS;
        break;
    case OTHER_BLOCKS:
        game_state.land_block_mask[0] = OTHER_BLOCKS;
        break;
    case WEST_BLOCKS:
        game_state.land_block_mask[0] = WEST_BLOCKS;
        break;
    case EAST_BLOCKS:
        game_state.land_block_mask[0] = EAST_BLOCKS;
        break;
    case SOUTH_EAST_BLOCKS:
        game_state.land_block_mask[0] = SOUTH_EAST_BLOCKS;
        break;
    case LOW_BLOCKS:
        game_state.land_block_mask[0] = LOW_BLOCKS;
        break;
    case HALL_BLOCKS:
        game_state.land_block_mask[0] = HALL_BLOCKS;
        break;
    case MIDDLE_BLOCKS:
        game_state.land_block_mask[0] = MIDDLE_BLOCKS;
        /* @hack spelling: a return, where the switch's end would do */
        return;
    }
}

Float lagoon_water_level(void)
{
    switch (lagoon_camera_blocks()) {
    case SOUTH_WEST_BLOCKS:
    case SOUTH_EAST_BLOCKS:
        return LOW_WATER;
    case OTHER_BLOCKS:
    case MIDDLE_BLOCKS:
    case LOW_BLOCKS:
    case HALL_BLOCKS:
        return MIDDLE_WATER;
    case EAST_BLOCKS:
    default:
        return HIGH_WATER;
    }
}

void lagoon_box_draw(NJS_VECTOR *scale)
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
    njSetCnkBlendMode(SHAPE_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, SHAPE_ATTR_OR);
    constant_material_set(SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE);
    njPushMatrixEx();
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_BOX]);
    njPopMatrixEx();
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void lagoon_cone_draw(NJS_VECTOR *scale)
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
    radius = &debug_models[DEBUG_MODEL_CONE]->model->r;
    saved = *radius;
    *radius *= largest;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetCnkBlendMode(SHAPE_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, SHAPE_ATTR_OR);
    constant_material_set(SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, size.y * CONE_RISE, 0.0F);
    njRotateX(NULL, HALF_TURN);
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_CONE]);
    njPopMatrixEx();
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void lagoon_cylinder_draw(NJS_VECTOR *scale)
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
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetCnkBlendMode(SHAPE_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, SHAPE_ATTR_OR);
    constant_material_set(SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, size.y * 0.5F, 0.0F);
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_CYLINDER]);
    njPopMatrixEx();
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void lagoon_sphere_draw(Float scale)
{
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius = &debug_models[DEBUG_MODEL_SPHERE]->model->r;
    Float saved = *radius;

    /* lagoon_model_draw_scaled scales r by scale again, so the draw's clip
     * test takes r times scale squared */
    *radius *= scale;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(SPHERE_CONTROL_SET);
    njSetCnkBlendMode(SPHERE_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, SHAPE_ATTR_OR);
    constant_material_set(SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE, SHAPE_SHADE);
    njPushMatrixEx();
    lagoon_model_draw_scaled(debug_models[DEBUG_MODEL_SPHERE]->model, scale);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

Angle lagoon_angle_turn(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

Sint32 lagoon_size_class_flags(Float size)
{
    /* @hack spelling: the tests chained with else, though each branch
     * returns, where tests of their own would do */
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

void lagoon_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point)
{
    Float sine;
    Float cosine;

    sine = njSin(ang);
    cosine = njCos(ang);
    point->x = (x * cosine) + (z * sine);
    point->z = (z * cosine) - (x * sine);
}

Angle lagoon_heading(Float x, Float z)
{
    /* @hack type: the turn masked to its full word before its cast, where the
     * cast alone would do */
    return (Sint16)((QUARTER_TURN - (Angle)(atan2f(z, x) * ANGLE_PER_RADIAN)) &
                    0xffffffff);
}

Bool lagoon_view_dips_below(Float height)
{
    struct camera_place *camera = camera_views[camera_current_index];
    NJS_POINT3 edge;
    NJS_POINT3 point;
    Float distance;
    Float half_height;

    if (height > camera->pos.y) {
        return FALSE;
    }
    /* @hack spelling: the bound worked out from VIEW_DEPTH and ABOVE_DEPTHS at
     * each call, where its value would do */
    if (camera->pos.y > height + (njAbs(VIEW_DEPTH) * ABOVE_DEPTHS)) {
        return FALSE;
    }
    distance =
        SCREEN_HALF_WIDTH / njTan((Angle)(VIEW_HALF_ANGLE * ANGLE_PER_DEGREE));
    half_height = njAbs(VIEW_DEPTH) * SCREEN_HALF_HEIGHT / distance;
    edge.x = 0.0F;
    edge.y = -half_height;
    edge.z = VIEW_DEPTH;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&camera->pos);
    /* @hack spelling: the camera's place read again through
     * camera_views, where camera's would do */
    njRotateX(NULL, camera_views[camera_current_index]->ang.x);
    njCalcPoint(NULL, &edge, &point);
    njPopMatrixEx();
    return height > point.y;
}

static void lagoon_vib_slight_step(struct task *task)
{
    if (STEPS(task) % INTERVAL(task->work) == 0) {
        vibration_start(PLAYER(task->work), POWER(task->work), FREQ(task->work),
                        VIBRATION_UNTIMED);
    }
    /* @hack spelling: the increment inside the test, where a statement of its
     * own would do */
    if (++STEPS(task) / INTERVAL(task->work) == PULSES(task->work)) {
        task_kill(task);
    }
}

void lagoon_vib_slight_create(int player, int power, int freq, int interval,
                              int count)
{
    struct task *task;

    task = task_create(TASK_WORK, VIB_SLIGHT_LEVEL, lagoon_vib_slight_step,
                       "VibSlightExec");
    if (task != NULL) {
        PLAYER(task->work) = player;
        POWER(task->work) = power;
        FREQ(task->work) = freq;
        PULSES(task->work) = count;
        /* @hack redundant: the steps set to 0, which task_create gave them
         * already */
        STEPS(task) = 0;
        INTERVAL(task->work) = interval;
        /* @hack spelling: the work's interval read back for the test, where
         * interval tested would do */
        if (INTERVAL(task->work) == 0) {
            INTERVAL(task->work) = 1;
        }
    }
}
