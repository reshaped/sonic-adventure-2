/* the task of a piece of scenery that only draws. SCENERY_INIT names its init,
 * and the includer defines scenery_draw. the init gives the task its step,
 * draw and destructor, unless placement_try_leave finds it out of range */
#ifndef STG23_SCENERY_IMPL_H
#define STG23_SCENERY_IMPL_H

#include "object/set.h"
#include "system/task.h"

static void scenery_destroy(struct task *task);
static void scenery_step(struct task *task);
static void scenery_draw(struct task *task);

void SCENERY_INIT(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = scenery_draw;
        task->step = scenery_step;
        task->destructor = scenery_destroy;
    }
}

static void scenery_destroy(struct task *task)
{
}

static void scenery_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

#endif
