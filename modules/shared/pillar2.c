/* PILLAR2 in a stage's object list: a pillar in the players' way, tilted by
 * its angles */
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/ocean_util.h"
#include "shared/pillar2.h"
#include "system/task.h"

#define PILLAR2_SHAPES 1
#define SCALE(work) ((work)->scl.x + 1.0F)

NJS_TEXLIST *pillar2_texlists[] = {&pillar2_texlist, NULL};
/* the pillar's cylinder at a scale of 1 */
/* clang-format off */
static struct collision_shape pillar2_shapes[PILLAR2_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 80.0F, 0.0F}, 15.0F, 80.0F,
     0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void pillar2_destroy(struct task *task);
static void pillar2_step(struct task *task);
static void pillar2_draw(struct task *task);

void pillar2_init(struct task *task)
{
    /* @hack spelling: shape, an array of one, where a struct and its address
     * would do */
    struct collision_shape shape[PILLAR2_SHAPES];
    /* @hack redundant: pos, a point nothing reads, which sizes the frame */
    NJS_POINT3 pos;

    if (!placement_try_leave(task)) {
        task->draw = pillar2_draw;
        task->step = pillar2_step;
        task->destructor = pillar2_destroy;
        shape[0] = pillar2_shapes[0];
        shape[0].radius *= SCALE(task->work);
        shape[0].size_y *= SCALE(task->work);
        shape[0].center.y *= SCALE(task->work);
        collision_init(task, shape, PILLAR2_SHAPES, COLLISION_OBJECT);
    }
}

static void pillar2_destroy(struct task *task)
{
}

static void pillar2_step(struct task *task)
{
    /* @hack redundant: work, a local nothing reads */
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void pillar2_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njSetTexture(&pillar2_texlist);
    /* @hack spelling: task->work read again, where work would do */
    ocean_model_draw_scaled(&pillar2_model, SCALE(task->work));
    njPopMatrixEx();
}
