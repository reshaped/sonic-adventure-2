/* the container, MDCONTBOX in Meteor Herd's object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg32/meteor_gate.h"
#include "stg32/scenery_shapes.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the container's node in the collision world */
#define NODE(task) ((NJS_OBJECT *)(task)->motion.p)
/* the node takes the size class that its model's radius plus CLASS_MARGIN
 * fits */
#define CLASS_MARGIN 15.0F
/* the work keeps WORLD_ADDED only while a player lies within NEAR_RANGE of
 * pos. world_collect_near_players passes over the node otherwise */
#define NEAR_RANGE 80.0F
#define CONTAINER_SHAPES 1
#define PIECES 9
#define BREAK_SPEED 0.3F
#define BREAK_SPREAD 1.2F
#define BREAK_LIFT 0.8F
#define BREAK_SCORE 20
#define LAMP_PERIOD 16
#define LAMP_REST 12
#define LAMP_FADE 3

NJS_TEXLIST *meteor_container_textures[] = {
    &meteor_container_texlist,
    &meteor_container_side_texlist,
    &meteor_container_corner_texlist,
    &meteor_container_lid_texlist,
    NULL,
};

/* the pieces the container breaks into: four sides, four corners and the
 * lid */
#define SIDE_HEIGHT 29.999996F
#define CORNER_HEIGHT 19.999998F
#define SIDE(x, z, yaw)                                                        \
    {&meteor_container_side_texlist,                                           \
     &meteor_container_side,                                                   \
     {x, SIDE_HEIGHT, z},                                                      \
     {0, yaw, 0}}
#define CORNER(x, z, yaw)                                                      \
    {&meteor_container_corner_texlist,                                         \
     &meteor_container_corner,                                                 \
     {x, CORNER_HEIGHT, z},                                                    \
     {0, yaw, 0}}
/* clang-format off */
static struct debris_part pieces[PIECES] = {
    SIDE(0.0F, -28.0F, HALF_TURN),
    SIDE(-28.0F, 0.0F, -QUARTER_TURN),
    SIDE(28.0F, 0.0F, QUARTER_TURN),
    SIDE(0.0F, 28.0F, 0),
    CORNER(27.0F, 27.0F, 0),
    CORNER(27.0F, -27.0F, QUARTER_TURN),
    CORNER(-27.0F, -27.0F, HALF_TURN),
    CORNER(-27.0F, 27.0F, -QUARTER_TURN),
    {&meteor_container_lid_texlist, &meteor_container_lid, {0.0F, 59.0F, 0.0F},
     {0, 0, 0}},
};
static struct collision_shape trigger_shape[CONTAINER_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 20.0F, 0.0F},
     65.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

static void container_destroy(struct task *task);
static void container_step(struct task *task);
static void container_draw(struct task *task);

void meteor_container_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    /* @hack type: flags a Uint32, where the Sint32 world_add takes would do */
    Uint32 flags;
    Float reach;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return;
    }
    task->draw = container_draw;
    task->step = container_step;
    task->destructor = container_destroy;
    work->mode = 0;
    *NODE(task) = meteor_container_collision;
    object = NODE(task);
    object->child = NULL;
    object->sibling = NULL;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    reach = object->model->r + CLASS_MARGIN;
    if (reach <= WORLD_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SIZE_CLASS_2_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SIZE_CLASS_3_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(2);
        } else {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    collision_init(task, trigger_shape, CONTAINER_SHAPES, COLLISION_OBJECT);
}

static void container_destroy(struct task *task)
{
    if (NODE(task)->model) {
        world_remove(task, NODE(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void container_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 origin;
    NJS_VECTOR lift;

    if (placement_try_leave(task)) {
        return;
    }
    if (collision_first_with_id(task, SHAPE_ID_BREAKER)) {
        origin = work->pos;
        lift.x = 0.0F;
        lift.y = BREAK_LIFT;
        lift.z = 0.0F;
        debris_break_flags(&origin, work->ang.y, pieces, PIECES, BREAK_SPEED,
                           BREAK_SPREAD, NO_FLOOR, &lift, DEBRIS_FADE_COLORS);
        score_add(BREAK_SCORE);
        if (task->placement) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        collision_register(task);
    }
    if (player_near(&work->pos, NEAR_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void container_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: color a Sint32, where the Uint32 color_blend returns would
     * do */
    Sint32 color;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&meteor_container_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkModDrawObject(&meteor_container_shadow);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(meteor_container_housing->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    color = color_blend(bright_color, dim_color,
                        blink_level(LAMP_PERIOD, LAMP_REST, LAMP_FADE));
    model_draw_tinted(meteor_container_lamp->model, color);
    njPopMatrixEx();
}
