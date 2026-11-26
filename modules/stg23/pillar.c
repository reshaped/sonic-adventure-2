#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/ocean_util.h"
#include "stg23/scenery.h"
#include "stg23/scenery_textures.h"
#include "system/task.h"

#define SCALE(work) ((work)->scl.x + 1.0F)

static void pillar_destroy(struct task *task);
static void pillar_step(struct task *task);
static void pillar_draw(struct task *task);

NJS_TEXLIST *base_pillar_texlists[] = {&base_pillar_texlist, NULL};

/* clang-format off */
static struct collision_shape shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000, {0.0F, 125.0F, 0.0F}, 35.0F, 125.0F, 35.0F, 0, {0, 0, 0},
};
/* clang-format on */

void base_pillar_init(struct task *task)
{
    /* @hack type: box as an array of one, where one collision_shape passed by
     * its address would do */
    struct collision_shape box[1];

    if (!placement_try_leave(task)) {
        task->draw = pillar_draw;
        task->step = pillar_step;
        task->destructor = pillar_destroy;
        box[0] = shape;
        box[0].radius *= SCALE(task->work);
        box[0].size_y *= SCALE(task->work);
        box[0].size_z *= SCALE(task->work);
        box[0].center.y *= SCALE(task->work);
        collision_init(task, box, 1, COLLISION_OBJECT);
    }
}

static void pillar_destroy(struct task *task)
{
}

static void pillar_step(struct task *task)
{
    /* @hack spelling: pillar, a copy of task, where task would do */
    struct task *pillar = task;

    if (!placement_try_leave(pillar)) {
        collision_register(pillar);
    }
}

static void pillar_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&base_pillar_texlist);
    ocean_model_draw_scaled(&base_pillar_model, SCALE(task->work));
    njPopMatrixEx();
}
