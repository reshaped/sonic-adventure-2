/* helpers Death Chamber's objects share: hunters' attacks, turns about y,
 * model draws and the water's height. shared/canyon_util.c,
 * stg18/lagoon_util.c and stg28/cave_util.c hold most of the same code under
 * their own names */
#include <nj_direct.h> /* IWYU pragma: keep -- completes NJS_DIRECT_HEAD */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "stg25/chamber_util.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

/* chamber_dust_behind's random turns, in degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* half the width and height of the screen, which is 640 by 480 */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F
#define NEAR_Z 0.001F
/* half the width and height of the screen's middle, which chamber_in_view
 * tests */
#define MIDDLE_HALF_WIDTH 240.0F
#define MIDDLE_HALF_HEIGHT 160.0F

/* chamber_view_dips_below's view. VIEW_HALF_ANGLE is half its angle across, in
 * degrees. VIEW_DEPTH is the z of the point it tests, in front of the camera. a
 * camera more than ABOVE_DEPTHS times |VIEW_DEPTH| above height can't have the
 * point below it */
#define VIEW_HALF_ANGLE 35.0F
#define VIEW_DEPTH (-3.0F)
#define ABOVE_DEPTHS 3.0F

#define ALPHA_BLEND (NJD_FBS_SA | NJD_FBD_ISA)
#define ADDITIVE_BLEND (NJD_FBS_ONE | NJD_FBD_ONE)
#define STRIP_FLAGS (NJD_FST_UA | NJD_FST_IS | NJD_FST_IL)
/* the debug draws' constant material, a translucent gray */
#define DEBUG_SHADE 0.3F
#define SPHERE_CONTROL_SET                                                     \
    (DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE)
#define SPHERE_BLEND (NJD_FBS_ONE | NJD_FBD_ISA)
/* chamber_cone_draw raises its cone by CONE_RISE times the cone's drawn
 * scale in y */
#define CONE_RISE 0.66667F

/* each pool's box in x and z, and the height of its water. west is toward -x,
 * and south toward -z, as stg25/death_chamber.h has them */
#define POOL_1_WEST_X (-820.0F)
#define POOL_1_EAST_X (-720.0F)
#define POOL_1_SOUTH_Z (-820.0F)
#define POOL_1_NORTH_Z (-720.0F)
#define POOL_1_LEVEL (-45.0F)
#define POOL_2_WEST_X (-1010.0F)
#define POOL_2_EAST_X (-690.0F)
#define POOL_2_SOUTH_Z (-1010.0F)
#define POOL_2_NORTH_Z (-570.0F)
#define POOL_2_LEVEL (-95.0F)
#define POOL_3_WEST_X (-690.0F)
#define POOL_3_EAST_X 320.0F
#define POOL_3_SOUTH_Z (-1010.0F)
#define POOL_3_NORTH_Z 320.0F
#define POOL_3_LEVEL (-10.0F)
#define POOL_4_WEST_X 750.0F
#define POOL_4_EAST_X 850.0F
#define POOL_4_SOUTH_Z (-850.0F)
#define POOL_4_NORTH_Z (-750.0F)
#define POOL_4_LEVEL 10.0F
#define POOL_5_WEST_X 750.0F
#define POOL_5_EAST_X 920.0F
#define POOL_5_SOUTH_Z (-920.0F)
#define POOL_5_NORTH_Z (-750.0F)
#define POOL_5_LEVEL (-40.0F)

/* no header declares these, and no call to them is left in the image:
 * chamber_on_screen and chamber_in_view take chamber_point_to_screen inline.
 * they are external only because SHC leaves out a static function with no
 * call left to it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* make a puff with dust_create, of speed spd and size scale. it lies at pos
 * plus an offset of (0, 0, radius). the offset first turns about x by a
 * random 0 to 60 degrees. it then turns about y by a random 120 to 240
 * degrees. it last takes the current matrix's turn, as
 * current_matrix_angles_xyz reads it.
 *
 * Ninja's matrix stack must have room for one more matrix, unchecked. the
 * call leaves the current matrix as it found it */
void chamber_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                         Float scale);
/* draw model as chamber_model_draw_scaled_v does, but with model_draw_tinted */
void chamber_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                        NJS_VECTOR *scale);
/* put in *sx and *sy where point lands on camera 0's screen, as view_to_screen
 * puts it. the call takes point through the current matrix, and leaves it as it
 * was */
void chamber_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy);
/* whether chamber_point_to_screen puts point on camera 0's screen. on the
 * screen means less than SCREEN_HALF_WIDTH across and SCREEN_HALF_HEIGHT up
 * or down from the middle. chamber_in_view asks the same of
 * MIDDLE_HALF_WIDTH and MIDDLE_HALF_HEIGHT. neither call tests z, so a point
 * behind the camera can pass */
Bool chamber_on_screen(NJS_POINT3 *point);
Bool chamber_in_view(NJS_POINT3 *point);
/* draw a debug model of camera/camera.h in a translucent gray, under the
 * current matrix. chamber_box_draw draws DEBUG_MODEL_BOX centered, with
 * scale's x, y and z for its half sizes. chamber_cone_draw draws
 * DEBUG_MODEL_CONE at twice scale, turned half a turn about x. it raises the
 * cone by CONE_RISE of its scaled height. chamber_sphere_draw draws
 * DEBUG_MODEL_SPHERE of radius scale, blended onto what lies behind it.
 *
 * each call puts back _nj_control_3d_flag_ and njSetConstantAttr's masks. it
 * leaves njSetCnkBlendMode's blend as it set it, and the constant material at
 * 0. it leaves the current matrix as it found it. Ninja's matrix stack must
 * have room for one more matrix, unchecked */
void chamber_box_draw(NJS_VECTOR *scale);
void chamber_cone_draw(NJS_VECTOR *scale);
void chamber_sphere_draw(Float scale);
/* draw model as chamber_model_draw_material does, adding its colors to those
 * drawn already. it leaves the blends as chamber_model_draw_tinted_additive
 * does. unlike model_draw_material_additive of shared/stage_util.h, it sets no
 * constant attribute, so each strip keeps its own flags */
void chamber_model_draw_material_additive(NJS_CNK_MODEL *model, Float alpha,
                                          Float red, Float green, Float blue);
/* whether the current camera's view dips below height. the point it tests
 * is the middle of the view's bottom edge, at a z of VIEW_DEPTH. the view is
 * 2 * VIEW_HALF_ANGLE degrees across a 640 by 480 screen. it turns by the
 * camera's ang.x alone. the call returns FALSE where the camera lies below
 * height.
 *
 * the call leaves the current matrix as it found it. Ninja's matrix stack
 * must have room for one more matrix, unchecked */
Bool chamber_view_dips_below(Float height);
/* NOLINTEND(misc-use-internal-linkage) */

Bool chamber_hunter_attacking(struct task *task)
{
    int slot = player_slot_of(task);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (record->character != PLAYER_KNUCKLES &&
        record->character != PLAYER_ROUGE) {
        return FALSE;
    }
    return work->mode == PLAYER_MODE_HUNTER_GLIDE ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_3 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3 ||
           work->mode == PLAYER_MODE_HUNTER_SPIRAL ||
           work->mode == PLAYER_MODE_HUNTER_DIVE;
}

Bool chamber_hunter_in_combo(struct task *task)
{
    int slot = player_slot_of(task);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (record->character != PLAYER_KNUCKLES &&
        record->character != PLAYER_ROUGE) {
        return FALSE;
    }
    return work->mode == PLAYER_MODE_HUNTER_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_3 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3;
}

void chamber_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void chamber_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
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

void chamber_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
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

void chamber_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
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

void chamber_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale)
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

void chamber_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale)
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

void chamber_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
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

void chamber_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
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

#pragma inline(chamber_point_to_screen)
void chamber_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    NJS_POINT3 view;

    point_to_view(point, &view);
    view_to_screen(&view, sx, sy);
}

Bool chamber_on_screen(NJS_POINT3 *point)
{
    Float sx;
    Float sy;

    chamber_point_to_screen(point, &sx, &sy);
    if (njAbs(sx) < SCREEN_HALF_WIDTH && njAbs(sy) < SCREEN_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

Bool chamber_in_view(NJS_POINT3 *point)
{
    Float sx;
    Float sy;

    chamber_point_to_screen(point, &sx, &sy);
    if (njAbs(sx) < MIDDLE_HALF_WIDTH && njAbs(sy) < MIDDLE_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

Angle chamber_angle_turn(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

void chamber_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point)
{
    Float sine;
    Float cosine;

    sine = njSin(ang);
    cosine = njCos(ang);
    point->x = (x * cosine) + (z * sine);
    point->z = (z * cosine) - (x * sine);
}

void chamber_model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
                                 Float green, Float blue)
{
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
    constant_material_set(alpha, red, green, blue);
    njCnkSimpleDrawModel(model);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

void chamber_box_draw(NJS_VECTOR *scale)
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
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void chamber_cone_draw(NJS_VECTOR *scale)
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
    njSetCnkBlendMode(ALPHA_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    constant_material_set(DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, size.y * CONE_RISE, 0.0F);
    njRotateX(NULL, HALF_TURN);
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_CONE]);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

Float chamber_water_level(void)
{
    NJS_POINT3 pos;

    pos = camera_views[camera_current_index]->pos;
    if (pos.x > POOL_1_WEST_X && pos.x <= POOL_1_EAST_X &&
        pos.z > POOL_1_SOUTH_Z && pos.z <= POOL_1_NORTH_Z) {
        return POOL_1_LEVEL;
    }
    if (pos.x > POOL_2_WEST_X && pos.x <= POOL_2_EAST_X &&
        pos.z > POOL_2_SOUTH_Z && pos.z <= POOL_2_NORTH_Z) {
        return POOL_2_LEVEL;
    }
    /* @hack spelling: the bound before pos.z in its test, where pos.z before
     * the bound would do */
    if (pos.x > POOL_3_WEST_X && pos.x <= POOL_3_EAST_X &&
        POOL_3_SOUTH_Z < pos.z && pos.z <= POOL_3_NORTH_Z) {
        return POOL_3_LEVEL;
    }
    /* @hack spelling: the bound before pos.z in its test, where pos.z before
     * the bound would do */
    if (pos.x > POOL_4_WEST_X && pos.x <= POOL_4_EAST_X &&
        POOL_4_SOUTH_Z < pos.z && pos.z <= POOL_4_NORTH_Z) {
        return POOL_4_LEVEL;
    }
    /* @hack spelling: the bound before pos.z in its test, where pos.z before
     * the bound would do */
    if (pos.x > POOL_5_WEST_X && pos.x <= POOL_5_EAST_X &&
        POOL_5_SOUTH_Z < pos.z && pos.z <= POOL_5_NORTH_Z) {
        return POOL_5_LEVEL;
    }
    return CHAMBER_NO_WATER;
}

void chamber_model_draw_material_additive(NJS_CNK_MODEL *model, Float alpha,
                                          Float red, Float green, Float blue)
{
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE |
                     NJD_CONTROL_3D_CONSTANT_MATERIAL);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    constant_material_set(alpha, red, green, blue);
    njCnkSimpleDrawModel(model);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

void chamber_model_draw_tinted_additive(NJS_CNK_MODEL *model, Uint32 color)
{
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    model_draw_tinted(model, color);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_attr_restore();
    draw_control_restore();
}

Bool chamber_view_dips_below(Float height)
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

void chamber_sphere_draw(Float scale)
{
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius = &debug_models[DEBUG_MODEL_SPHERE]->model->r;
    Float saved = *radius;

    /* chamber_model_draw_scaled scales r by scale again, so the draw's clip
     * test takes r times scale squared */
    *radius *= scale;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(SPHERE_CONTROL_SET);
    njSetCnkBlendMode(SPHERE_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    constant_material_set(DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE);
    njPushMatrixEx();
    chamber_model_draw_scaled(debug_models[DEBUG_MODEL_SPHERE]->model, scale);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}
