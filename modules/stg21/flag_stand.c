#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/ocean_util.h"
#include "stg21/flag_stand.h"
#include "system/task.h"

NJS_TEXLIST *flag_stand_texlists[] = {&flag_stand_texlist, NULL};

/* clang-format off */
static struct collision_shape flag_stand_shape = {
    0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000, {0.0F, 50.0F, 0.0F}, 10.0F, 50.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */

static void flag_stand_destroy(struct task *task);
static void flag_stand_step(struct task *task);
static void flag_stand_draw(struct task *task);

void flag_stand_init(struct task *task)
{
    /* @hack type: shape, an array of one, where a struct and its address would
     * do */
    struct collision_shape shape[1];

    if (!placement_try_leave(task)) {
        task->draw = flag_stand_draw;
        task->step = flag_stand_step;
        task->destructor = flag_stand_destroy;
        shape[0] = flag_stand_shape;
        /* @hack spelling: the scale read through task at each use, where one
         * local would do */
        shape[0].radius *= task->work->scl.x + 1.0F;
        shape[0].size_y *= task->work->scl.x + 1.0F;
        shape[0].center.y *= task->work->scl.x + 1.0F;
        collision_init(task, shape, 1, COLLISION_OBJECT);
    }
}

static void flag_stand_destroy(struct task *task)
{
}

static void flag_stand_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (!placement_try_leave(self)) {
        collision_register(self);
    }
}

static void flag_stand_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&flag_stand_texlist);
    /* @hack spelling: the scale read through task, where work would do */
    ocean_model_draw_scaled(&flag_stand_model, task->work->scl.x + 1.0F);
    njPopMatrixEx();
}
