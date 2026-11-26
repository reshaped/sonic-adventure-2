/* the fruit a race tree drops. a racer eats it in WHOLE_FRUIT bites, and the
 * fruit tells the racer once the last is eaten */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/bounce.h"
#include "chao/carry.h"
#include "chao/fruit.h"
#include "chao/movement.h"
#include "chao/world.h"
#include "chaostgrace/props.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "system/task.h"
#include "system/util.h"

/* a fruit's ang.z: the bites left */
#define FRUIT_BITES(work) ((work)->ang.z)
#define WHOLE_FRUIT 4
/* a bite comes after more than BITE_STEPS steps of PARTNER_USE. each bite
 * sets the fruit's cylinder and carry radius to BITE_RADIUS for each bite
 * left */
#define BITE_STEPS 60
#define BITE_RADIUS 0.5F
/* the fruit draws at BITE_SCALE for each bite left, and BITE_SCALE more. it
 * stands FRUIT_LIFT above its place, and FRUIT_RISE more for each unit of
 * its scale */
#define BITE_SCALE 0.3F
#define FRUIT_LIFT 0.8F
#define FRUIT_RISE 0.55F

enum { FRUIT_SPHERE, FRUIT_CYLINDER, FRUIT_SHAPES };
/* clang-format off */
static const struct collision_shape fruit_shapes[FRUIT_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_ENEMIES | PUSH_OBJECTS, SHAPE_GUARD_LEVEL(3), 0,
     {0.0F, 1.2F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {(Sint8)GARDEN_FRUIT_HIT, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), 0, {0.0F, -2.0F, 0.0F}, 4.2F, 5.0F, 0.0F, 0,
     {0, 0, 0}}};
/* clang-format on */

void race_fruit_bounce(struct task *task)
{
    garden_bounce_on_floor(task);
}

#define ITEM_REST fruit_rest
#define ITEM_MOVE fruit_move
#include "chaostgrace/item_motion_impl.h"

static void fruit_drop(struct task *task)
{
    struct task_work *work = task->work;
    ITEM_BEGIN(work);
    mover_step(task);
    if (work->timer++ > ITEM_DROP_STEPS) {
        ITEM_PHASE(work, ITEM_MOVING);
    }
}

static void fruit_eat(struct task *task)
{
    struct task_work *work = task->work;
    int command = garden_take_command(task);
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        work->submode++;
        work->timer = 0;
        break;
    }
    /* @hack spelling: a one-case switch on command around the test of the
     * bite's steps, where one && of the two would do */
    switch (command) {
    case PARTNER_USE:
        if (work->timer++ > BITE_STEPS) {
            struct garden_entry *entry =
                garden_partner_in_category(task, GARDEN_OBJECT_CHAO);
            work->timer = 0;
            FRUIT_BITES(work)--;
            /* @hack redundant: a test of entry, whose branch is empty */
            if (entry) {
            }
            collision_get_shape(task, FRUIT_CYLINDER)->radius =
                FRUIT_BITES(work) * BITE_RADIUS;
            garden_set_carry_radius(task, FRUIT_BITES(work) * BITE_RADIUS);
            if (FRUIT_BITES(work) == 0) {
                garden_command_partner(task, PARTNER_USED_UP);
                task_kill(task);
            }
        }
        break;
    }
    /* @hack redundant: a test of garden_follow_chao's result, whose branch
     * is empty, where the call alone would do */
    if (garden_follow_chao(task)) {
    }
}

static void fruit_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ITEM_STILL:
        fruit_rest(task);
        break;
    case ITEM_DROPPED:
        fruit_drop(task);
        break;
    case ITEM_MOVING:
        fruit_move(task);
        break;
    case ITEM_HELD:
        fruit_eat(task);
        break;
    }
    collision_register(task);
}

static void fruit_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale = BITE_SCALE + (FRUIT_BITES(work) * BITE_SCALE);
    njSetTexture(&race_fruit_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njTranslate(NULL, 0.0F, FRUIT_LIFT + (scale * FRUIT_RISE), 0.0F);
    if (FRUIT_BITES(work) > 0) {
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        njScale(NULL, scale, scale, scale);
        if (garden_hit_kind(task, FRUIT_CYLINDER, GARDEN_SHADE_HIT)) {
            draw_control_set(DRAW_CONTROL_SHADOW);
        }
        njCnkEasyDrawObject(&race_fruit_model[0]);
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrixEx();
        ITEM_DRAW_SHADOW(task, FRUIT_CYLINDER, scale);
    }
    njPopMatrix(1);
}

static void fruit_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side clears the tree's link to the fruit as it dies */
    race_forget_tree_fruit(task);
#endif
    garden_unregister_object(task);
}

static void fruit_init(struct task *task)
{
    struct task_work *work = task->work;
#ifdef NONMATCHING
    /* this side destroys the fruit where collision_init fails */
    if (!collision_init(task, fruit_shapes, FRUIT_SHAPES,
                        COLLISION_OBJECT_MUTUAL)) {
        task_destroy(task);
        return;
    }
#else
    collision_init(task, fruit_shapes, FRUIT_SHAPES, COLLISION_OBJECT_MUTUAL);
#endif
    fall_set_kind(work, task->motion.p, FALL_KIND_NORMAL);
    task->step = fruit_step;
    task->destructor = fruit_destroy;
    task->draw = fruit_draw;
    ITEM_PHASE(work, ITEM_DROPPED);
#ifdef NONMATCHING
    /* this side destroys the fruit where the garden has no slot for it */
    if (!garden_register_object(GARDEN_OBJECT_FRUIT, task, 0)) {
        task_destroy(task);
        return;
    }
#else
    garden_register_object(GARDEN_OBJECT_FRUIT, task, 0);
#endif
    garden_set_carry_height(task, ITEM_CARRY_HEIGHT);
    garden_set_carry_radius(task, WHOLE_FRUIT * BITE_RADIUS);
    FRUIT_BITES(work) = WHOLE_FRUIT;
}

struct task *race_fruit_create(NJS_POINT3 *position, Angle yaw,
                               NJS_VECTOR *velocity)
{
    struct task *task = task_create(TASK_WORK, 4, fruit_init, "ALO_RaceFruit");
    struct task_work *work;
    struct mover *mover;
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
    /* this side destroys the fruit and returns null where mover_alloc fails. it
     * sets the destructor once the fruit can die cleanly */
    if (!mover) {
        task_destroy(task);
        return NULL;
    }
    task->destructor = fruit_destroy;
#endif
    work->pos = *position;
    work->ang.y = yaw;
    if (velocity) {
        mover->spd = *velocity;
    }
    ITEM_SET_MOVER(task, mover);
    return task;
}
