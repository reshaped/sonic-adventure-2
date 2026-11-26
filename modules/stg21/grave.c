#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/ocean_util.h"
#include "stg21/grave.h"
#include "system/task.h"

#define GRAVE_SHAPES 7
/* clang-format off */
#define POST(x, z)                                                             \
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),                      \
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,                                     \
     {x, 67.5F, z}, 5.0F, 12.5F, 0.0F, 0, {0, 0, 0}}
/* clang-format on */

NJS_TEXLIST *grave_texlists[] = {&grave_texlist, NULL};

/* clang-format off */
static struct collision_shape grave_shapes[GRAVE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 27.5F, 0.0F}, 25.0F, 27.5F, 50.0F, 0, {0, 0, 0}},
    POST(20.0F, 45.0F),  POST(20.0F, 0.0F),  POST(20.0F, -45.0F),
    POST(-20.0F, 45.0F), POST(-20.0F, 0.0F), POST(-20.0F, -45.0F),
};
/* clang-format on */

static void grave_destroy(struct task *task);
static void grave_step(struct task *task);
static void grave_draw(struct task *task);

void grave_init(struct task *task)
{
    struct collision_shape shapes[GRAVE_SHAPES];
    int i;

    if (!placement_try_leave(task)) {
        task->draw = grave_draw;
        task->step = grave_step;
        task->destructor = grave_destroy;
        /* @hack spelling: the scale read through task at each use, where one
         * local before the loop would do */
        for (i = 0; i < GRAVE_SHAPES; i++) {
            shapes[i] = grave_shapes[i];
            shapes[i].radius *= task->work->scl.x + 1.0F;
            shapes[i].size_y *= task->work->scl.x + 1.0F;
            shapes[i].size_z *= task->work->scl.x + 1.0F;
            shapes[i].center.x *= task->work->scl.x + 1.0F;
            shapes[i].center.y *= task->work->scl.x + 1.0F;
            shapes[i].center.z *= task->work->scl.x + 1.0F;
        }
        collision_init(task, shapes, GRAVE_SHAPES, COLLISION_OBJECT);
    }
}

static void grave_destroy(struct task *task)
{
}

static void grave_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (!placement_try_leave(self)) {
        collision_register(self);
    }
}

static void grave_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&grave_texlist);
    /* @hack spelling: the scale read through task, where work would do */
    ocean_model_draw_scaled(&grave_model, task->work->scl.x + 1.0F);
    njPopMatrixEx();
}
