/* helpers Egg Quarters' objects share: hunters' attacks, turns about y, model
 * draws and the water's height. stg25/chamber_util.c and stg28/cave_util.c
 * hold most of the same code under their own names */
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
#include "stg26/quarters_area.h"
#include "stg26/quarters_util.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

/* quarters_dust_behind's random turns, in degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* half the width and height of the screen, which is 640 by 480 */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F
#define NEAR_Z 0.001F
/* half the width and height of the screen's middle, which quarters_in_view
 * tests */
#define MIDDLE_HALF_WIDTH 240.0F
#define MIDDLE_HALF_HEIGHT 160.0F

/* quarters_view_dips_below's view. VIEW_HALF_ANGLE is half its angle across,
 * in degrees. VIEW_DEPTH is the z of the point it tests, in front of the
 * camera. a camera more than ABOVE_DEPTHS times |VIEW_DEPTH| above height
 * can't have the point below it */
#define VIEW_HALF_ANGLE 35.0F
#define VIEW_DEPTH (-3.0F)
#define ABOVE_DEPTHS 3.0F

/* each pool's box in x and z, and the height of its water. west is toward -x,
 * and south toward -z, as stg26/quarters_area.h has them */
#define POOL_WEST_X (-45.0F)
#define POOL_EAST_X 45.0F
#define UPPER_SOUTH_Z 230.0F
#define UPPER_NORTH_Z 290.0F
#define UPPER_LEVEL 60.0F
#define LOWER_SOUTH_Z 120.0F
#define LOWER_NORTH_Z UPPER_SOUTH_Z
#define LOWER_LEVEL 10.0F

/* a vertex chunk's second word holds its count of vertices in its high
 * halfword */
#define COUNT_SHIFT 16

/* the debug draws' constant material, a translucent gray */
#define DEBUG_SHADE 0.3F
#define SPHERE_CONTROL_SET                                                     \
    (DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE)

/* the bounds of the areas quarters_area_at tells apart. a bound in x stands
 * for its negation too, but FAR_SPLIT_X */
#define FAR_NORTH_Z 1500.0F
#define FAR_SPLIT_X 30.0F
#define NORTH_Z 1120.0F
#define NORTH_SIDE_X 200.0F
#define SIDE_X 600.0F
#define INNER_X 260.0F
#define INNER_SPLIT_Z 800.0F
#define CENTER_SOUTH_Z 325.0F

/* no header declares these, and no call to them is left in the image:
 * quarters_on_screen and quarters_in_view take quarters_point_to_screen
 * inline. they are external only because SHC leaves out a static function
 * with no call left to it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* make a puff with dust_create, of speed spd and size scale, at pos plus an
 * offset. the offset is (0, 0, radius), turned about x by a random 0 to 60
 * degrees. it then turns about y by a random 120 to 240 degrees. it last
 * takes the current matrix's turn, as current_matrix_angles_xyz reads it.
 *
 * Ninja's matrix stack must have room for one more matrix, unchecked. the
 * call leaves the current matrix as it found it */
void quarters_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                          Float scale);
/* draw model as quarters_model_draw_scaled does, but with model_draw_tinted */
void quarters_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                       Float scale);
/* put in *sx and *sy where point lands on camera 0's screen, as
 * view_to_screen puts it. the call takes point through the current matrix,
 * and leaves it as it was */
void quarters_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy);
/* whether point lies on camera 0's screen, where quarters_point_to_screen
 * puts it. quarters_on_screen asks whether it lies less than
 * SCREEN_HALF_WIDTH across and SCREEN_HALF_HEIGHT up or down from the middle.
 * quarters_in_view asks the same of MIDDLE_HALF_WIDTH and MIDDLE_HALF_HEIGHT.
 * neither tests z, so a point behind the camera can pass */
Bool quarters_on_screen(NJS_POINT3 *point);
Bool quarters_in_view(NJS_POINT3 *point);
/* draw a debug model of camera/camera.h in a translucent gray, under the
 * current matrix. quarters_sphere_draw draws a sphere of radius scale,
 * blended onto what lies behind it. quarters_cylinder_draw draws the cylinder
 * scaled by scale's x, y and z, and raised by half scale's y.
 *
 * each call puts back the control bits and the strip flags it changed. it
 * leaves njSetCnkBlendMode's blend at ALPHA_BLEND, and the current matrix as
 * it found it. the sphere leaves the constant material at 0, and the cylinder
 * leaves it gray. Ninja's matrix stack must have room for one more matrix,
 * unchecked */
void quarters_sphere_draw(Float scale);
void quarters_cylinder_draw(NJS_VECTOR *scale);
/* NOLINTEND(misc-use-internal-linkage) */

Bool quarters_hunter_attacking(struct task *task)
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

void quarters_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void quarters_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
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

void quarters_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
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

void quarters_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
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

void quarters_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
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

void quarters_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale)
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

void quarters_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale)
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

void quarters_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
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

#pragma inline(quarters_point_to_screen)
void quarters_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    NJS_POINT3 view;

    point_to_view(point, &view);
    view_to_screen(&view, sx, sy);
}

Bool quarters_on_screen(NJS_POINT3 *point)
{
    Float sx;
    Float sy;

    quarters_point_to_screen(point, &sx, &sy);
    if (njAbs(sx) < SCREEN_HALF_WIDTH && njAbs(sy) < SCREEN_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

Bool quarters_in_view(NJS_POINT3 *point)
{
    Float sx;
    Float sy;

    quarters_point_to_screen(point, &sx, &sy);
    if (njAbs(sx) < MIDDLE_HALF_WIDTH && njAbs(sy) < MIDDLE_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

Angle quarters_angle_turn(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

Float quarters_water_level(void)
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
    return QUARTERS_NO_WATER;
}

/* copy src's first vertex chunk, an NJD_CV chunk, to dst: its two header
 * words, then its vertices. each vertex's x and z turn as if its y were 0.
 * the turn is by -roll about z, then by -pitch about x. the vertex's y then
 * adds to the turned y. the copy stops at the last vertex, and leaves out the
 * word after it */
static void vlist_copy_tilted(Sint32 *dst, Sint32 *src, Angle pitch, Angle roll)
{
    Sint32 word;
    /* @hack redundant: count a register variable, where a plain local would
     * do */
    register int count;
    /* @hack type: from and to Float pointers that step a coordinate at a
     * time, where NJS_POINT3 pointers that step a vertex would do */
    Float *to;
    Float *from;
    NJS_POINT3 floor;
    NJS_POINT3 tilted;
    Float height;

    *dst++ = *src++;
    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    word = *dst++ = *src++;
    count = (Uint16)(word >> COUNT_SHIFT);
    to = (Float *)dst;
    from = (Float *)src;
    while (count--) {
        floor.x = *from++;
        height = *from++;
        floor.z = *from++;
        floor.y = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateX(NULL, -pitch);
        njRotateZ(NULL, -roll);
        njCalcPoint(NULL, &floor, &tilted);
        njPopMatrixEx();
        *to++ = tilted.x;
        *to++ = tilted.y + height;
        *to++ = tilted.z;
    }
}

void quarters_modifier_draw_tilted(NJS_CNK_MODEL *model, Angle pitch,
                                   Angle roll)
{
    Sint32 *vlist = model->vlist;

    if ((Uint16)*vlist == NJD_CV) {
        /* @hack spelling: copy, a local for WORK_AREA, where WORK_AREA passed
         * and stored in place would do */
        Sint32 *copy = (Sint32 *)WORK_AREA;

        vlist_copy_tilted(copy, vlist, pitch, roll);
        model->vlist = copy;
        njCnkModDrawModel(model);
        model->vlist = vlist;
    } else {
        njCnkModDrawModel(model);
    }
}

void quarters_sphere_draw(Float scale)
{
    /* @hack spelling: radius, a pointer to the model's r, where the model's r
     * read and written in place would do */
    Float *radius = &debug_models[DEBUG_MODEL_SPHERE]->model->r;
    Float saved = *radius;

    *radius *= scale;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(SPHERE_CONTROL_SET);
    njSetCnkBlendMode(PREMULTIPLIED_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    constant_material_set(DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE);
    njPushMatrixEx();
    quarters_model_draw_scaled(debug_models[DEBUG_MODEL_SPHERE]->model, scale);
    njPopMatrixEx();
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

void quarters_cylinder_draw(NJS_VECTOR *scale)
{
    /* @hack spelling: size, a copy of *scale, where scale read in place would
     * do */
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
    njSetCnkBlendMode(ALPHA_BLEND);
    njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
    constant_material_set(DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE, DEBUG_SHADE);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, size.y * 0.5F, 0.0F);
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_CYLINDER]);
    njPopMatrixEx();
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

Uint32 quarters_world_size_class(Float radius)
{
    /* @hack spelling: the tests chained with else, where each return ending
     * its branch would do */
    if (radius > WORLD_SIZE_CLASS_1_MAX) {
        return WORLD_SIZE_CLASS(0);
    } else if (radius > WORLD_SIZE_CLASS_2_MAX) {
        return WORLD_SIZE_CLASS(1);
    } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
        return WORLD_SIZE_CLASS(2);
    } else {
        return WORLD_SIZE_CLASS(3);
    }
}

Bool quarters_view_dips_below(Float height)
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

enum quarters_area quarters_area_at(NJS_POINT3 *pos)
{
    /* @hack spelling: the tests chained with else, where each return ending
     * its branch would do */
    if (pos->z >= FAR_NORTH_Z) {
        if (pos->x < FAR_SPLIT_X) {
            return QUARTERS_FAR_NORTHWEST;
        } else {
            return QUARTERS_FAR_NORTHEAST;
        }
    } else if (pos->z >= NORTH_Z) {
        if (pos->x < -NORTH_SIDE_X) {
            return QUARTERS_NORTHWEST;
        } else if (pos->x >= NORTH_SIDE_X) {
            return QUARTERS_NORTHEAST;
        } else {
            return QUARTERS_NORTH;
        }
    } else if (pos->x < -SIDE_X) {
        return QUARTERS_WEST;
    } else if (pos->x >= SIDE_X) {
        return QUARTERS_EAST;
    } else if (pos->x < -INNER_X) {
        if (pos->z >= INNER_SPLIT_Z) {
            return QUARTERS_INNER_NORTHWEST;
        } else {
            return QUARTERS_INNER_SOUTHWEST;
        }
    } else if (pos->x >= INNER_X) {
        if (pos->z >= INNER_SPLIT_Z) {
            return QUARTERS_INNER_NORTHEAST;
        } else {
            return QUARTERS_INNER_SOUTHEAST;
        }
    } else {
        if (pos->z >= CENTER_SOUTH_Z) {
            return QUARTERS_CENTER;
        } else {
            return QUARTERS_SOUTH;
        }
    }
}
