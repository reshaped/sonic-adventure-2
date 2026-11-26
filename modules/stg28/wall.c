/* the wall, WALL in Pyramid Cave's object list: a solid wall with an overlay
 * that pulses. it draws see-through while it hides the current camera's
 * player */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/tint_pulse.h"
#include "stg28/cave_util.h"
#include "stg28/wall.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/wall/text.inc"
#include "stg28/models/wall_mesh.h"

#define TYPES 2
#define WIDTHS 2
#define TYPE(work) ((int)njAbs((work)->scl.x) % TYPES)
#define WIDTH(work) ((int)njAbs((work)->scl.z) % WIDTHS)

/* the wall's shape in the collision world collides only while a player lies
 * within REACH of its pos */
#define REACH 113.4F
/* the wall's color while it hides the current camera's player */
#define SEE_THROUGH_COLOR 0x65ffffff
#define SEE_THROUGH_CONTROL NJD_CONTROL_3D_CNK_CONSTANT_ATTR
/* a line runs from the current camera to its player. the wall hides the
 * player where the line crosses the wall's plane. the plane runs along the x
 * and y axes as the wall's ang.y turns them. it lies HIDE_DEPTH along -z from
 * the wall's pos. across, the crossing must lie within half the wall's width
 * plus HIDE_MARGIN of the pos. it must lie between HIDE_BOTTOM and HIDE_TOP
 * above the pos. the wall is MODEL_WIDTH wide at a width scale of 1 */
#define HIDE_DEPTH 10.0F
#define HIDE_MARGIN 5.0F
#define HIDE_BOTTOM (-10.0F)
#define HIDE_TOP 60.0F
#define MODEL_WIDTH 100.0F

static void wall_destroy(struct task *task);
static void wall_step(struct task *task);
static void wall_draw(struct task *task);
static Bool wall_hides_player(struct task *task);

static NJS_CNK_MODEL *wall_models[TYPES] = {
#include "stg28/wall/wall_models.inc"
};
static NJS_CNK_MODEL *pulse_models[TYPES] = {
#include "stg28/wall/pulse_models.inc"
};
NJS_TEXLIST *cave_wall_texlists[TYPES + 1] = {
#include "stg28/wall/cave_wall_texlists.inc"
};
static char *wall_type_names[TYPES] = {
#include "stg28/wall/wall_type_names.inc"
};
static Float width_scales[WIDTHS] = {0.8F, 1.0625F};
/* the box, at a width scale of 1 */
/* clang-format off */
static struct collision_shape wall_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000, {0.0F, 32.5F, -15.0F}, 37.5F, 17.5F, 5.0F, 0,
    {0, 0, QUARTER_TURN},
};
/* clang-format on */

void cave_wall_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    /* @hack type: shapes an array of one, where a single shape would do */
    struct collision_shape shapes[1];

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = wall_draw;
    task->step = wall_step;
    task->destructor = wall_destroy;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c502ef2). wall_destroy
     * tests task->extra for null (TST at 0x8c50300a) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the wall, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_wall_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[1] = object->scl[2] = 1.0F;
    /* @hack spelling: task->work read again, where work would do */
    object->scl[0] = width_scales[WIDTH(task->work)];
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SIZE_CLASS(1) | WORLD_SOLID, task, object);
    shapes[0] = wall_shape;
    shapes[0].radius *= width_scales[WIDTH(task->work)];
    collision_init(task, shapes, 1, COLLISION_OBJECT);
}

static void wall_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void wall_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
}

static void wall_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    int step;
    Uint32 color;

    scale.y = scale.z = 1.0F;
    /* @hack spelling: task->work read again for the width and the type, where
     * work would do */
    scale.x = width_scales[WIDTH(task->work)];
    step = game_state.steps % TINT_PULSE_STEPS;
    if (step < TINT_PULSE_RISE) {
        color = TINT_PULSE_RISE_ALPHA(step) | TINT_PULSE_COLORS;
    } else {
        color = TINT_PULSE_FALL_ALPHA(step) | TINT_PULSE_COLORS;
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(cave_wall_texlists[TYPE(task->work)]);
    if (wall_hides_player(task)) {
        draw_control_save();
        draw_attr_save();
        draw_control_set(SEE_THROUGH_CONTROL);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        cave_model_draw_tinted_scaled_v(wall_models[TYPE(task->work)],
                                        SEE_THROUGH_COLOR, &scale);
        draw_attr_restore();
        draw_control_restore();
    } else {
        cave_model_draw_scaled_v(wall_models[TYPE(task->work)], &scale);
    }
    model_draw_tinted(pulse_models[TYPE(task->work)], color);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static Bool wall_hides_player(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 player;
    NJS_POINT3 camera;
    Float zero;
    Float crossing;
    Float x;
    Float y;
    Float width;
    /* @hack redundant: player_z a register variable, where a plain local would
     * do */
    register Float player_z;
    Float camera_y;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    /* @hack spelling: zero, a local that carries 0, where the constant would
     * do */
    zero = 0.0F;
    njTranslate(NULL, zero, zero, HIDE_DEPTH);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[camera_current_index]->pos, &player);
    njCalcPoint(NULL, &camera_views[camera_current_index]->pos, &camera);
    njPopMatrixEx();
    player_z = player.z;
    if (player_z == camera.z || player_z * camera.z > 0.0F) {
        return FALSE;
    }
    crossing = -camera.z / (player_z - camera.z);
    /* @hack spelling: x set from the camera's x and then moved, where one
     * statement would do */
    x = camera.x;
    x = x + ((player.x - x) * crossing);
    /* @hack spelling: camera_y, a copy of the camera's y, where camera.y
     * would do */
    camera_y = camera.y;
    y = camera_y + ((player.y - camera_y) * crossing);
    /* @hack spelling: task->work read again, where work would do */
    width = MODEL_WIDTH * width_scales[WIDTH(task->work)];
    return x > -(width * 0.5F) - HIDE_MARGIN &&
           x < (width * 0.5F) + HIDE_MARGIN && y > HIDE_BOTTOM && y < HIDE_TOP;
}
