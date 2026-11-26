/* the setup of a holdable object, for common_object.c's holdable_init and
 * space_pickup.c's pickups. HOLDABLE_SETUP gives its linkage, type and name.
 * the includer has included holdable_motion_impl.h, and defines a
 * holdable_draw of its own. shared/common_object.h gives holdable_init's
 * contract */
#ifndef SHARED_HOLDABLE_SETUP_H
#define SHARED_HOLDABLE_SETUP_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/switch.h"
#include "system/task.h"

#define HOLDABLE_LEVELS                                                        \
    (SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(0) | SHAPE_HURT |               \
     SHAPE_HURTS_ENEMIES | SHAPE_HURTS_OBJECTS)

/* the child's mode is ATTACK_OFF while a player holds its parent */
static void holdable_collider_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!work->mode) {
        collision_register(task);
    }
}

#define HOLDABLE_SHAPE_COUNT 2
#define FLOOR_BOUNCE 0.3F
#define BOUNCY_FLOOR_BOUNCE 0.6F
/* the outer cylinder touches only the players */
#define OUTER_FLAGS                                                            \
    (SHAPE_IGNORE_ALL_BUT_PLAYERS | SHAPE_UNTURNED | SHAPE_NO_OWN_HITS)

/* a holdable object's shapes. the players can't push its outer cylinder, and
 * an attack can hurt its inner one */
/* clang-format off */
static struct collision_shape holdable_shapes[HOLDABLE_SHAPE_COUNT] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, OUTER_FLAGS,
     {0.0F, 5.0F, 0.0F}, 10.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, 0, HOLDABLE_LEVELS, SHAPE_UNTURNED | SHAPE_HURTABLE,
     {0.0F, 4.5F, 0.0F}, 2.5F, 6.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* holdable_shapes for HOLDABLE_PUSHES, whose inner cylinder also pushes all
 * but pickups */
/* clang-format off */
static struct collision_shape holdable_pushing_shapes[HOLDABLE_SHAPE_COUNT] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, OUTER_FLAGS,
     {0.0F, 5.0F, 0.0F}, 10.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     HOLDABLE_LEVELS, SHAPE_UNTURNED | SHAPE_HURTABLE, {0.0F, 4.5F, 0.0F}, 2.5F,
     6.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the shape of the object's attack, at its child, which doesn't touch the
 * players */
/* clang-format off */
static struct collision_shape holdable_collider_shapes[] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), HOLDABLE_LEVELS,
     SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_UNK_9 | SHAPE_UNTURNED | SHAPE_HURTABLE,
     {0.0F, 4.5F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

HOLDABLE_SETUP(struct task *task, NJS_TEXLIST *texlist, NJS_CNK_MODEL *model,
               Sint32 hold_turn, int mode)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    struct task *child;
#ifdef NONMATCHING
    /* the shapes this side's collision_init copies */
    const struct collision_shape *shapes;
#endif

    if ((mode & OBJECT_MAY_LEAVE) && placement_try_leave(task)) {
        return;
    }
    work->mode = mode;
    if (texlist != NULL && model != NULL) {
        task->draw = holdable_draw;
    }
    task->extra.p = model;
    task->unk_3c.p = texlist;
    task->step = holdable_step;
    task->destructor = holdable_destroy;
    motion->hold_turn = hold_turn;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: collision_init
     * copies the table's own count. it ends the setup where the record or the
     * body can't be made */
    shapes =
        (mode & HOLDABLE_PUSHES) ? holdable_pushing_shapes : holdable_shapes;
    if (!collision_init(task, shapes, HOLDABLE_SHAPE_COUNT, COLLISION_ITEM)) {
        goto incomplete;
    }
    body_init(task);
    if (task->force.p == NULL) {
        goto incomplete;
    }
#else
    /* @bug memory: collision_init copies nine shapes from a table of two, and
     * so reads past the table's end. in stg13's image, holdable_init loads
     * the count 9 at 0x8c53430e and 0x8c534312 */
    if (mode & HOLDABLE_PUSHES) {
        collision_init(task, holdable_pushing_shapes, 9, COLLISION_ITEM);
    } else {
        collision_init(task, holdable_shapes, 9, COLLISION_ITEM);
    }
    body_init(task);
#endif
    if (task->force.p != NULL) {
        BODY(task)->floor_bounce =
            (mode & HOLDABLE_BOUNCY) ? BOUNCY_FLOOR_BOUNCE : FLOOR_BOUNCE;
        BODY(task)->flags |= BODY_RESTS;
    }
    /* @bug memory: reads and writes the body's flags through null where
     * body_init found no room. in stg13's image, holdable_init tests
     * task->force for the null at 0x8c534322, but not before this |=, which
     * reads the flags through it at 0x8c5343de. holdable_step's call of
     * body_step at 0x8c534114 and holdable_draw's call of njMultiMatrix at
     * 0x8c534264 read through the null too */
    if (mode & HOLDABLE_ROLLS) {
        BODY(task)->flags |= BODY_ROLLS;
    }
    if (!(mode & HOLDABLE_NO_PRESSER)) {
        switch_presser_add(&work->pos);
    }
    child = task_create_child(TASK_WORK, holdable_collider_step, task);
    if (child != NULL) {
        child->work->pos = work->pos;
        collision_init(child, holdable_collider_shapes, 1, COLLISION_ATTACK);
        child->work->id = WORK_ID_ATTACK;
    }
#ifdef NONMATCHING
    if (child == NULL || child->work->collision == NULL) {
        task_destroy(task);
    }
    return;

incomplete:
    /* the model isn't the task's to free. holdable_destroy mustn't run, since
     * pos isn't among the switch pressers yet */
    task->extra.p = NULL;
    task->destructor = NULL;
    task_destroy(task);
#endif
}

#endif
