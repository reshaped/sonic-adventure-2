#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/ocean_util.h"
#include "stg23/platform_textures.h"
#include "stg23/platforms.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define PLINTH_KINDS 5
#define KIND(work) ((int)njAbs((work)->scl.z) % PLINTH_KINDS)
#define SCALE(work) ((work)->scl.x + 1.0F)
#define HALF_HEIGHT(work) (heights[KIND(work)] * SCALE(work) * 0.5F)

static void plinth_destroy(struct task *task);
static void plinth_step(struct task *task);
static void plinth_draw(struct task *task);

KEPT static NJS_CNK_MODEL *models[PLINTH_KINDS] = {
#include "stg23/plinth/models.inc"
};
/* the kinds' texture lists, then a null that ends them */
NJS_TEXLIST *base_plinth_texlists[PLINTH_KINDS + 1] = {
#include "stg23/plinth/base_plinth_texlists.inc"
};
static const char *kind_names[PLINTH_KINDS] = {
    " 1.0m", " 6.0m", "11.0m", "16.0m", "21.0m",
};
static Float heights[PLINTH_KINDS] = {10.0F, 60.0F, 110.0F, 160.0F, 210.0F};
/* clang-format off */
static struct collision_shape shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F}, 25.0F, 0.0F, 25.0F, 0, {0, 0, 0},
};
/* clang-format on */

void base_plinth_init(struct task *task)
{
    /* @hack type: box as an array of one, where one collision_shape passed by
     * its address would do */
    struct collision_shape box[1];

    if (!placement_try_leave(task)) {
        task->draw = plinth_draw;
        task->step = plinth_step;
        task->destructor = plinth_destroy;
        box[0] = shape;
        box[0].radius *= SCALE(task->work);
        box[0].size_y = HALF_HEIGHT(task->work);
        box[0].size_z *= SCALE(task->work);
        box[0].center.y = HALF_HEIGHT(task->work);
        collision_init(task, box, 1, COLLISION_OBJECT);
    }
}

static void plinth_destroy(struct task *task)
{
}

static void plinth_step(struct task *task)
{
    /* @hack spelling: plinth, a copy of task, where task would do */
    struct task *plinth = task;

    if (!placement_try_leave(plinth)) {
        collision_register(plinth);
    }
}

static void plinth_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.z = SCALE(task->work);
    scale.y = 1.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(base_plinth_texlists[KIND(task->work)]);
    ocean_model_draw_scaled_v(models[KIND(task->work)], &scale);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
