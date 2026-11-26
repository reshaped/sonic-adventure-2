/* the step and the destructor of both kinds of electric pillar object.
 * electric_pillar.c and tall_pillar.c each include this, and so define a step
 * and a destructor of their own */
#ifndef STG34_PILLAR_IMPL_H
#define STG34_PILLAR_IMPL_H
#include "collision/collision.h"
#include "object/set.h"
#include "system/task.h"

static void pillar_destroy(struct task *task)
{
}

static void pillar_step(struct task *task)
{
    /* @hack spelling: pillar, a copy of task, where task would do */
    struct task *pillar = task;
    /* @hack spelling: an arm that only returns, where one call of
     * collision_register under !placement_try_leave(pillar) would do */
    if (placement_try_leave(pillar)) {
        return;
    } else {
        collision_register(pillar);
    }
}

#endif
