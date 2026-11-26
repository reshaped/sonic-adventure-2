/* the shutter, METEOSHUT in Meteor Herd's object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg32/meteor_gate.h"
#include "stg32/scenery_shapes.h"
#include "system/task.h"

/* the shutter's node in the collision world */
#define NODE(task) ((NJS_OBJECT *)(task)->motion.p)
#define SHUTTER_FLAGS (WORLD_SOLID | WORLD_MOVING)
/* the node takes the size class that its model's radius plus CLASS_MARGIN
 * fits */
#define CLASS_MARGIN 15.0F
/* the work keeps WORLD_ADDED only while a player lies within NEAR_RANGE of
 * pos. world_collect_near_players passes over the node otherwise */
#define NEAR_RANGE 120.0F
#define SHUTTER_SHAPES 1
/* in the work's mode: whether a large meteor has broken the shutter */
#define BROKEN(work) ((work)->mode)

NJS_TEXLIST *meteor_shutter_textures[] = {
    &meteor_shutter_texlist,
    &meteor_shutter_broken_texlist,
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

void meteor_shutter_init(struct task *task)
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
    *NODE(task) = *meteor_shutter_collision;
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
    flags = SHUTTER_FLAGS;
    reach = object->model->r + CLASS_MARGIN;
    if (reach <= WORLD_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SIZE_CLASS_2_MAX) {
            flags = SHUTTER_FLAGS | WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SIZE_CLASS_3_MAX) {
            flags = SHUTTER_FLAGS | WORLD_SIZE_CLASS(2);
        } else {
            flags = SHUTTER_FLAGS | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    collision_init(task, door_shape, SHUTTER_SHAPES, COLLISION_OBJECT);
}

static void shutter_destroy(struct task *task)
{
    if (NODE(task)->model) {
        world_remove(task, NODE(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void shutter_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (BROKEN(work)) {
        NODE(task)->model = meteor_shutter_broken_collision->model;
    } else {
        NODE(task)->model = meteor_shutter_collision->model;
        if (collision_first_with_id(task, SHAPE_ID_BREAKER)) {
            BROKEN(work) = TRUE;
        }
    }
    if (player_near(&work->pos, NEAR_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
}

#define SHUTTER_TEXLIST meteor_shutter_texlist
#define SHUTTER_FIRST_LIGHT meteor_shutter_first_light
#define SHUTTER_SECOND_LIGHT meteor_shutter_second_light
#define SHUTTER_HOUSING meteor_shutter_housing
#define SHUTTER_BROKEN_TEXLIST meteor_shutter_broken_texlist
#define SHUTTER_BROKEN meteor_shutter_broken
#include "shared/meteor_shutter_lights.h"
