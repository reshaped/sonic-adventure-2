/* the scavenger hunt's toys. a racer carries one to its answer lane. once the
 * racer lets it go, the toy vanishes and a new one drops where it spawned */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/carry.h"
#include "chao/world.h"
#include "chaostgrace/hunt_data.h"
#include "chaostgrace/props.h"
#include "chaostgrace/scavenger.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "system/task.h"
#include "system/util.h"

#define ITEM_REST scavenger_item_rest
#define ITEM_MOVE scavenger_item_move
#include "chaostgrace/item_motion_impl.h"

/* a toy is of race_scavenger_target's kind at a chance of TARGET_CHANCE. else
 * a draw below OTHER_KINDS sets it 1 to TOY_KINDS - 1 kinds past the target's
 */
#define TARGET_CHANCE 0.6F
#define OTHER_KINDS 2.999F
#define TOY_LIFT 1.3F
#define DOLL_SCALE 1.8F
#define SHADOW_WIDTH 1.4F
#define TOY_CARRY_RADIUS 1.1F
#define TOY_INDEX(work) ((work)->ang.x)

struct scavenger_art {
    NJS_CNK_OBJECT *model;
    NJS_TEXLIST *textures;
};
static struct scavenger_art toy_art[TOY_KINDS] = {
#include "chaostgrace/hunt_toy/toy_art.inc"
};
/* clang-format off */
static const struct collision_shape toy_shape = {
    0, SHAPE_SPHERE, PUSH_ENEMIES | PUSH_OBJECTS, SHAPE_GUARD_LEVEL(3), 0,
    {0.0F, 1.2F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */

static void scavenger_item_drop(struct task *task)
{
    struct task_work *work = task->work;
    ITEM_BEGIN(work);
    mover_step(task);
    if (work->timer++ > ITEM_DROP_STEPS) {
        ITEM_PHASE(work, ITEM_MOVING);
    }
    if (garden_object_locked(task)) {
        ITEM_PHASE(work, ITEM_HELD);
    }
}

static void scavenger_item_follow(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: mover, a local nothing reads */
    struct mover *mover = task->motion.p;
    (void)mover;
    garden_take_command(task);
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        work->submode++;
        work->timer = 0;
        break;
    }
    if (!garden_follow_chao(task)) {
        task_kill(task);
        race_respawn_scavenger_item(TOY_INDEX(work));
    }
}

static void scavenger_item_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ITEM_STILL:
        scavenger_item_rest(task);
        collision_register(task);
        break;
    case ITEM_DROPPED:
        scavenger_item_drop(task);
        collision_register(task);
        break;
    case ITEM_MOVING:
        scavenger_item_move(task);
        collision_register(task);
        break;
    case ITEM_HELD:
        scavenger_item_follow(task);
        break;
    }
}

static void scavenger_item_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: mover, a local nothing reads */
    struct mover *mover = task->motion.p;
    (void)mover;
    njSetTexture(toy_art[TOY_KIND(work)].textures);
    njPushMatrix(NULL);
    njTranslateEx(&work->pos);
    njTranslate(NULL, 0.0F, TOY_LIFT, 0.0F);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    if (TOY_KIND(work) == TOY_DOLL) {
        njScale(NULL, DOLL_SCALE, DOLL_SCALE, DOLL_SCALE);
    }
    /* @hack spelling: task->work, where work would do */
    njCnkEasyDrawObject(toy_art[TOY_KIND(task->work)].model);
    njPopMatrixEx();
    /* @bug logic: tests the toy's shape 1 for a touch of a GARDEN_SHADE_HIT
     * shape, at 0x0c9c4cba, where scavenger_item_init gives the toy shape 0
     * alone, at 0x0c9c4d92. the test never holds, and the shadow always drops
     * SHADOW_DROP */
    ITEM_DRAW_SHADOW(task, 1, SHADOW_WIDTH);
    njPopMatrix(1);
}

static void scavenger_item_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side clears the toy's slot in race_scavenger_items as it dies */
    race_forget_scavenger_item(task);
#endif
    garden_unregister_object(task);
}

static void scavenger_item_init(struct task *task)
{
    struct task_work *work = task->work;
#ifdef NONMATCHING
    /* this side destroys the toy where collision_init fails */
    if (!collision_init(task, &toy_shape, 1, COLLISION_OBJECT_MUTUAL)) {
        task_destroy(task);
        return;
    }
#else
    collision_init(task, &toy_shape, 1, COLLISION_OBJECT_MUTUAL);
#endif
    fall_set_kind(work, task->motion.p, FALL_KIND_NORMAL);
    task->step = scavenger_item_step;
    task->destructor = scavenger_item_destroy;
    task->draw = scavenger_item_draw;
    ITEM_PHASE(work, ITEM_DROPPED);
#ifdef NONMATCHING
    /* this side destroys the toy where the garden has no slot for it */
    if (!garden_register_object(GARDEN_OBJECT_TOY, task, 0)) {
        task_destroy(task);
        return;
    }
#else
    garden_register_object(GARDEN_OBJECT_TOY, task, 0);
#endif
    garden_set_carry_height(task, ITEM_CARRY_HEIGHT);
    garden_set_carry_radius(task, TOY_CARRY_RADIUS);
}

struct task *race_scavenger_item_create(Uint8 index, NJS_POINT3 *position,
                                        Angle yaw, NJS_VECTOR *velocity)
{
    struct task *task =
        task_create(TASK_WORK, 4, scavenger_item_init, "ALO_RaceKariToy");
    struct task_work *work;
    struct mover *mover;
    Float chance;
    int kind;
#ifdef NONMATCHING
    /* this side returns null where task_create fails. the matching side writes
     * through the null */
    if (!task) {
        return NULL;
    }
#endif
    work = task->work;
    mover = mover_alloc(task);
#ifdef NONMATCHING
    /* this side destroys the toy and returns null where mover_alloc fails. it
     * sets the destructor once the toy can die cleanly */
    if (!mover) {
        task_destroy(task);
        return NULL;
    }
    task->destructor = scavenger_item_destroy;
#endif
    work->pos = *position;
    work->ang.y = yaw;
    if (velocity) {
        mover->spd = *velocity;
    }
    ITEM_SET_MOVER(task, mover);
    chance = (Float)rand() * RAND_SCALE;
    kind = race_scavenger_target;
    if (chance < TARGET_CHANCE) {
        TOY_KIND(work) = kind;
    } else {
        TOY_KIND(work) =
            (kind + (Uint8)((Float)rand() * (OTHER_KINDS * RAND_SCALE)) + 1) %
            TOY_KINDS;
    }
    TOY_INDEX(work) = index;
    return task;
}
