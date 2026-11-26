/* PILLAR3 in a stage's object list: three posts under a beam, which stand in
 * the players' way */
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/ocean_util.h"
#include "shared/pillar3.h"
#include "system/task.h"

#define PILLAR3_SHAPES 4
#define SCALE(work) ((work)->scl.x + 1.0F)

NJS_TEXLIST *pillar3_texlists[] = {&pillar3_texlist, NULL};
/* the shapes at a scale of 1: a box across the top, then the three posts */
/* clang-format off */
static struct collision_shape pillar3_shapes[PILLAR3_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 137.5F, 0.0F}, 14.0F, 17.5F,
     100.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 60.0F, 0.0F}, 15.0F, 60.0F,
     0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 60.0F, 85.0F}, 15.0F, 60.0F,
     0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 60.0F, -85.0F}, 15.0F, 60.0F,
     0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void pillar3_destroy(struct task *task);
static void pillar3_step(struct task *task);
static void pillar3_draw(struct task *task);

void pillar3_init(struct task *task)
{
    struct collision_shape shapes[PILLAR3_SHAPES];
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = pillar3_draw;
    task->step = pillar3_step;
    task->destructor = pillar3_destroy;
    for (i = 0; i < PILLAR3_SHAPES; i++) {
        shapes[i] = pillar3_shapes[i];
        shapes[i].radius *= SCALE(task->work);
        shapes[i].size_y *= SCALE(task->work);
        shapes[i].size_z *= SCALE(task->work);
        shapes[i].center.x *= SCALE(task->work);
        shapes[i].center.y *= SCALE(task->work);
        shapes[i].center.z *= SCALE(task->work);
    }
    collision_init(task, shapes, PILLAR3_SHAPES, COLLISION_OBJECT);
}

static void pillar3_destroy(struct task *task)
{
}

static void pillar3_step(struct task *task)
{
    /* @hack redundant: work, a local nothing reads */
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void pillar3_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&pillar3_texlist);
    /* @hack spelling: task->work read again, where work would do */
    ocean_model_draw_scaled(&pillar3_model, SCALE(task->work));
    njPopMatrixEx();
}
