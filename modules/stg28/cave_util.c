/* helpers Pyramid Cave's objects share: Sonic's combo, turns about y, model
 * draws and distances across x and z. shared/canyon_util.c,
 * stg18/lagoon_util.c and stg25/chamber_util.c hold most of the same code
 * under their own names */
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
#include "stg28/cave_util.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

#ifdef NONMATCHING
/* port_view_aspect, the window this side's test of the screen takes */
#include "engine/dc/port.h"
#endif

/* cave_dust_behind's random turns, in degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* half the width and height of the screen, which is 640 by 480.
 * view_to_screen gives a point whose z is 0 a z of NEAR_Z */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F
#define NEAR_Z 0.001F

/* each pool's box in x and z, and the height of its water, with the values
 * stg26/quarters_util.c gives Egg Quarters' pools. west is toward -x, and
 * south toward -z */
#define POOL_WEST_X (-45.0F)
#define POOL_EAST_X 45.0F
#define UPPER_SOUTH_Z 230.0F
#define UPPER_NORTH_Z 290.0F
#define UPPER_LEVEL 60.0F
#define LOWER_SOUTH_Z 120.0F
#define LOWER_NORTH_Z UPPER_SOUTH_Z
#define LOWER_LEVEL 10.0F
/* what cave_water_level returns outside both pools */
#define NO_WATER (-20000.0F)

/* a vertex chunk's second word holds its count of vertices in its high
 * halfword */
#define COUNT_SHIFT 16

/* the debug box's constant material, a translucent gray */
#define DEBUG_SHADE 0.3F

/* no header declares these, and no call to them is left in the image:
 * cave_on_screen takes cave_point_to_screen inline. they are external only
 * because SHC leaves out a static function with no call left to it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* make a puff with dust_create, of speed spd and size scale. it lies at pos
 * plus an offset of (0, 0, radius). the offset first turns about x by a
 * random 0 to 60 degrees. it then turns about y by a random 120 to 240
 * degrees. it last takes the current matrix's turn, as
 * current_matrix_angles_xyz reads it.
 *
 * Ninja's matrix stack must have room for one more matrix, unchecked. the
 * call leaves the current matrix as it found it */
void cave_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                      Float scale);
/* put in *sx and *sy where point lands on camera 0's screen, as view_to_screen
 * puts it. the call takes point through the current matrix, and returns whether
 * its z is then 0 or more. point keeps its value */
Bool cave_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy);
/* a copy of quarters_water_level, with Egg Quarters' pools. nothing in stg28
 * calls it. it returns the level of the first pool whose box holds player 0's
 * x and z. outside both boxes it returns NO_WATER. a box holds x and z above
 * its low bounds and at or below its high bounds. player_work[0] must not be
 * null, unchecked */
Float cave_water_level(void);
/* draw model with njCnkModDrawModel. where its vertex list starts with an
 * NJD_CV chunk, a copy of that chunk stands for the list. vlist_copy_turned
 * writes the copy, turned by ax and az, at WORK_AREA, over what was there. it
 * ends the copy at its last vertex. the words past the copy are what WORK_AREA
 * held. Ninja's matrix stack must have room for one more matrix, unchecked */
void cave_modifier_draw_turned(NJS_CNK_MODEL *model, Angle ax, Angle az);
/* draw debug_models[DEBUG_MODEL_BOX] of camera/camera.h in a translucent
 * gray, under the current matrix. the box reaches scale's x, y and z to each
 * side of the origin.
 *
 * the call puts back the control bits and the strip flags it changed. it
 * leaves njSetCnkBlendMode's blend as it set it, and the constant material at
 * 0. it leaves the current matrix as it found it. Ninja's matrix stack must
 * have room for one more matrix, unchecked */
void cave_box_draw(NJS_VECTOR *scale);
/* NOLINTEND(misc-use-internal-linkage) */

Bool cave_sonic_combo_or_bounce(struct task *task)
{
    int slot = player_slot_of(task);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    int mode;

    if (record->character != PLAYER_SONIC) {
        return FALSE;
    }
    mode = work->mode;
    return mode == PLAYER_MODE_SONIC_COMBO_1 ||
           mode == PLAYER_MODE_SONIC_COMBO_2 ||
           mode == PLAYER_MODE_SONIC_COMBO_3 ||
           mode == PLAYER_MODE_SONIC_COMBO_WAIT ||
           mode == PLAYER_MODE_SONIC_COMBO_BACK ||
           mode == PLAYER_MODE_SONIC_COMBO_RUNNING_1 ||
           mode == PLAYER_MODE_SONIC_COMBO_RUNNING_2 ||
           mode == PLAYER_MODE_SONIC_COMBO_RUNNING_3 ||
           mode == PLAYER_MODE_SONIC_BOUNCE;
}

Bool cave_sonic_in_combo(struct task *task)
{
    int slot = player_slot_of(task);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    int mode;

    if (record->character != PLAYER_SONIC) {
        return FALSE;
    }
    mode = work->mode;
    return mode == PLAYER_MODE_SONIC_COMBO_1 ||
           mode == PLAYER_MODE_SONIC_COMBO_2 ||
           mode == PLAYER_MODE_SONIC_COMBO_3 ||
           mode == PLAYER_MODE_SONIC_COMBO_WAIT ||
           mode == PLAYER_MODE_SONIC_COMBO_BACK ||
           mode == PLAYER_MODE_SONIC_COMBO_RUNNING_1 ||
           mode == PLAYER_MODE_SONIC_COMBO_RUNNING_2 ||
           mode == PLAYER_MODE_SONIC_COMBO_RUNNING_3;
}

void cave_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void cave_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
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

void cave_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
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

void cave_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
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

void cave_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
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

void cave_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
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
 * the screen's middle. a z of 0 first becomes NEAR_Z, in point itself. the call
 * returns whether z is then above 0 */
static Bool view_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    Float view = camera_view_angle(0);
    Float distance = SCREEN_HALF_WIDTH / njTan((Angle)(view * 0.5F));

    if (point->z == 0.0F) {
        point->z = NEAR_Z;
    }
    /* @hack spelling: x over -z, negated, where x over z would do */
    *sx = -(point->x / -point->z * distance);
    *sy = point->y / -point->z * distance;
    return point->z > 0.0F;
}

#pragma inline(point_to_view)
static void point_to_view(NJS_POINT3 *from, NJS_POINT3 *to)
{
    njCalcPoint(NULL, from, to);
}

#pragma inline(cave_point_to_screen)
Bool cave_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    NJS_POINT3 view;

    point_to_view(point, &view);
    return view_to_screen(&view, sx, sy);
}

Bool cave_on_screen(NJS_POINT3 *point)
{
    Float sx;
    Float sy;
    /* @hack spelling: front, a local for the call's result, where the call
     * tested in place would do */
    Bool front;

    front = cave_point_to_screen(point, &sx, &sy);
#ifdef NONMATCHING
    /* this side takes the port's window, which 16:9 widens outside a demo
     * (engine/dc/port.h) */
    sx *= port_view_aspect;
#endif
    if (front && njAbs(sx) < SCREEN_HALF_WIDTH &&
        njAbs(sy) < SCREEN_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

Angle cave_angle_turn(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

void cave_model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
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

Float cave_water_level(void)
{
    NJS_POINT3 pos;

    pos = player_work[0]->pos;
    /* @hack spelling: the bound before pos.z in each test, where pos.z before
     * the bound would do */
    if (pos.x > POOL_WEST_X && pos.x <= POOL_EAST_X && UPPER_SOUTH_Z < pos.z &&
        pos.z <= UPPER_NORTH_Z) {
        return UPPER_LEVEL;
    }
    if (pos.x > POOL_WEST_X && pos.x <= POOL_EAST_X && LOWER_SOUTH_Z < pos.z &&
        pos.z <= LOWER_NORTH_Z) {
        return LOWER_LEVEL;
    }
    return NO_WATER;
}

/* copy src's first vertex chunk, an NJD_CV chunk, to dst: its two header
 * words, then its vertices. each vertex keeps its y. its x and z turn as if
 * its y were 0. the turn is by az about z, then by ax about x. the copy ends
 * at the last vertex, without the word that follows it. Ninja's matrix stack
 * must have room for one more matrix, unchecked */
static void vlist_copy_turned(Sint32 *dst, Sint32 *src, Angle ax, Angle az)
{
    Sint32 word;
    Float *from;
    Float *to;
    NJS_POINT3 point;
    NJS_POINT3 turned;
    Float y;
    /* @hack redundant: count a register variable, where a plain local would
     * do */
    register int count;

    *dst++ = *src++;
    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    word = *dst++ = *src++;
    count = (Uint16)(word >> COUNT_SHIFT);
    to = (Float *)dst;
    from = (Float *)src;
    while (count--) {
        point.x = *from++;
        y = *from++;
        point.z = *from++;
        point.y = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateX(NULL, ax);
        njRotateZ(NULL, az);
        njCalcPoint(NULL, &point, &turned);
        njPopMatrixEx();
        *to++ = turned.x;
        *to++ = y;
        *to++ = turned.z;
    }
}

void cave_modifier_draw_turned(NJS_CNK_MODEL *model, Angle ax, Angle az)
{
    Sint32 *vlist = model->vlist;

    if ((Uint16)*vlist == NJD_CV) {
        /* @hack spelling: copy, a local for WORK_AREA, where WORK_AREA passed
         * and stored in place would do */
        Sint32 *copy = (Sint32 *)WORK_AREA;

        vlist_copy_turned(copy, vlist, ax, az);
        model->vlist = copy;
        njCnkModDrawModel(model);
        model->vlist = vlist;
    } else {
        njCnkModDrawModel(model);
    }
}

Float cave_xz_length_sq(NJS_POINT3 *point)
{
    return (point->x * point->x) + (point->z * point->z);
}

Float cave_xz_distance(NJS_POINT3 *from, NJS_POINT3 *to)
{
    Float dx = to->x - from->x;
    Float dz = to->z - from->z;

    return njSqrt((dx * dx) + (dz * dz));
}

void cave_box_draw(NJS_VECTOR *scale)
{
    NJS_VECTOR size;
    Float saved;
    Float largest;
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius;

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

void cave_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point)
{
    Float sine;
    Float cosine;

    sine = njSin(ang);
    cosine = njCos(ang);
    point->x = (x * cosine) + (z * sine);
    point->z = (z * cosine) - (x * sine);
}
