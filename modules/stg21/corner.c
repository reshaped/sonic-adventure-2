#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "stg21/corner.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define CORNER_KINDS 1

KEPT static NJS_CNK_MODEL *corner_models[CORNER_KINDS] = {
#include "stg21/corner/corner_models.inc"
};
NJS_TEXLIST *corner_texlists[] = {&corner_texlist, NULL};
static char *corner_names[CORNER_KINDS] = {"BRIGHT"};

/* clang-format off */
static struct collision_shape corner_shapes[] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {-80.0F, 20.0F, 0.0F}, 85.0F, 20.0F, 5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 20.0F, 75.0F}, 5.0F, 20.0F, 70.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void corner_destroy(struct task *task);
static void corner_step(struct task *task);
static void corner_draw(struct task *task);

void corner_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = corner_draw;
        task->step = corner_step;
        task->destructor = corner_destroy;
        collision_init(task, corner_shapes,
                       sizeof(corner_shapes) / sizeof(corner_shapes[0]),
                       COLLISION_OBJECT);
    }
}

static void corner_destroy(struct task *task)
{
}

static void corner_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (!placement_try_leave(self)) {
        collision_register(self);
    }
}

static void corner_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack redundant: the index ang.z % CORNER_KINDS, where 0, the one kind,
     * would do */
    njSetTexture(corner_texlists[task->work->ang.z % CORNER_KINDS]);
    njCnkEasyDrawModel(corner_models[task->work->ang.z % CORNER_KINDS]);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
