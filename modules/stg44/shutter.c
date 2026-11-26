/* the shutter, SHUT in Mad Space's object list: a gate that vanishes once
 * broken */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg44/scenery_shapes.h"
#include "stg44/shutter.h"
#include "hud/hud.h"
#include "system/task.h"

/* the shutter's node in the collision world */
#define NODE(task) ((NJS_OBJECT *)(task)->motion.p)
#define SHUTTER_FLAGS (WORLD_SOLID | WORLD_MOVING)
/* the node takes the size class that its model's radius plus CLASS_MARGIN
 * fits */
#define CLASS_MARGIN 15.0F
/* the work keeps WORLD_ADDED only while the shutter is whole and a player lies
 * within NEAR_RANGE of pos. world_collect_near_players passes over the node
 * otherwise */
#define NEAR_RANGE 120.0F
#define SHUTTER_SHAPES 1
#define BREAK_SCORE 20
/* in the work's mode: whether the shutter is broken */
#define BROKEN(work) ((work)->mode)

NJS_TEXLIST *mad_space_shutter_textures[] = {
    &mad_space_shutter_texlist,
    NULL,
};
/* clang-format off */
static struct collision_shape door_shape[SHUTTER_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 40.0F, 0.0F},
     30.0F, 40.0F, 5.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static void shutter_destroy(struct task *task);
static void shutter_step(struct task *task);
static void shutter_draw(struct task *task);

void mad_space_shutter_init(struct task *task)
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
    task->draw = shutter_draw;
    task->step = shutter_step;
    task->destructor = shutter_destroy;
    BROKEN(work) = FALSE;
    *NODE(task) = *mad_space_shutter_collision;
    flags = SHUTTER_FLAGS;
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
    reach = object->model->r + CLASS_MARGIN;
    if (reach <= WORLD_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    collision_init(task, door_shape, SHUTTER_SHAPES, COLLISION_OBJECT);
    if (task->placement) {
        BROKEN(work) = placement_get_state(task);
    }
}

static void shutter_destroy(struct task *task)
{
    struct task_work *work = task->work;
    if (task->placement) {
        placement_set_state(task, BROKEN(work));
    }
    if (NODE(task)->model) {
        world_remove(task, NODE(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void shutter_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        /* @hack spelling: the empty block and the else, where an if of
         * !BROKEN(work) alone would do */
        if (BROKEN(work)) {
        } else {
            NODE(task)->model = mad_space_shutter_collision->model;
            if (collision_first_with_id(task, SHAPE_ID_BREAKER)) {
                BROKEN(work) = TRUE;
                score_add(BREAK_SCORE);
            }
        }
        if (player_near(&work->pos, NEAR_RANGE) && !BROKEN(work)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        collision_register(task);
    }
}

#define SHUTTER_TEXLIST mad_space_shutter_texlist
#define SHUTTER_FIRST_LIGHT mad_space_shutter_first_light
#define SHUTTER_SECOND_LIGHT mad_space_shutter_second_light
#define SHUTTER_HOUSING mad_space_shutter_housing
#include "shared/meteor_shutter_lights.h"
