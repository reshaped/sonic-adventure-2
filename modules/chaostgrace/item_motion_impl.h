/* the rest and move steps of the race's fruit and scavenger toys, and what
 * else the two share. the image holds a copy of each step in each object's
 * code. an includer names its copies by defining ITEM_REST and ITEM_MOVE
 * first */
#ifndef CHAOSTGRACE_ITEM_MOTION_IMPL_H
#define CHAOSTGRACE_ITEM_MOTION_IMPL_H
#include <ninja.h>
#include "chao/movement.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "system/task.h"

/* an item's work->mode. a racer carries an ITEM_HELD item */
enum race_item_mode { ITEM_DROPPED, ITEM_STILL, ITEM_MOVING, ITEM_HELD };
/* a dropped item moves after more than ITEM_DROP_STEPS steps */
#define ITEM_DROP_STEPS 10
#define ITEM_PHASE(work, mode_value)                                           \
    do {                                                                       \
        (work)->mode = (mode_value);                                           \
        (work)->submode = 0;                                                   \
    } while (0)
/* a step's first run in a mode moves submode from 0 to 1 */
#define ITEM_BEGIN(work)                                                       \
    /* @hack spelling: a switch on submode, where an if would do */            \
    switch ((work)->submode) {                                                 \
    case 0:                                                                    \
        (work)->submode++;                                                     \
        break;                                                                 \
    }

/* set the mover the race's fruit and toys share */
#define ITEM_SET_MOVER(task, mover)                                            \
    do {                                                                       \
        mover_set_gravity((task), -0.05F);                                     \
        (mover)->probe_height = 3.0F;                                          \
        (mover)->floor_bounce = 0.8F;                                          \
        (mover)->friction = 0.7F;                                              \
        (mover)->hold_turn = 0xd00;                                            \
    } while (0)
/* a racer holds an item ITEM_CARRY_HEIGHT below its hands */
#define ITEM_CARRY_HEIGHT 1.0F

/* draw an item's shadow, width across, where the camera is nearer it than
 * chao_tuning.object_shadow_distance. the shadow drops CONTACT_DROP while
 * the item's shape touches a shape of id GARDEN_SHADE_HIT, and else
 * SHADOW_DROP */
#define CONTACT_DROP (-1.85F)
#define SHADOW_DROP (-1.0F)
#define SHADOW_HEIGHT 0.7F
#define ITEM_DRAW_SHADOW(task, shape, width)                                   \
    do {                                                                       \
        if (((struct garden_entry *)(task)->force.p)->distance <               \
            chao_tuning.object_shadow_distance) {                              \
            if (garden_hit_kind((task), (shape), GARDEN_SHADE_HIT)) {          \
                njTranslate(NULL, 0.0F, CONTACT_DROP, 0.0F);                   \
            } else {                                                           \
                njTranslate(NULL, 0.0F, SHADOW_DROP, 0.0F);                    \
            }                                                                  \
            njScale(NULL, (width), SHADOW_HEIGHT, (width));                    \
            njCnkModDrawModel(&garden_shadow_model);                           \
        }                                                                      \
    } while (0)

static void ITEM_REST(struct task *task)
{
    struct task_work *work = task->work;
    ITEM_BEGIN(work);
    if (collision_pushed(task)) {
        ITEM_PHASE(work, ITEM_MOVING);
    }
    if (garden_object_locked(task)) {
        ITEM_PHASE(work, ITEM_HELD);
    }
}

static void ITEM_MOVE(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR velocity;
    ITEM_BEGIN(work);
    mover_step(task);
    mover_meet_stage(task);
    if (work->flags & FALL_LANDED) {
        /* @hack redundant: velocity, a local whose three stores of 0
         * nothing reads */
        velocity.x = 0.0F;
        velocity.y = 0.0F;
        velocity.z = 0.0F;
        ITEM_PHASE(work, ITEM_STILL);
    }
    if (garden_object_locked(task)) {
        ITEM_PHASE(work, ITEM_HELD);
    }
}

#endif
