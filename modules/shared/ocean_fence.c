/* the fence, FENCE in Sand Ocean's and Hidden Base's object lists: a row of
 * panels */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/ocean_fence.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define FENCE_KINDS 2
#define KIND(work) ((work)->ang.z % FENCE_KINDS)
/* the panels lie PANEL_LENGTH apart along x, toward -x for a negative count. a
 * panel's box is centered PANEL_HALF_HEIGHT above the work's pos */
#define COUNT(work) ((int)(work)->scl.y)
#define PANEL_LENGTH 50.0F
#define PANEL_HALF_HEIGHT 20.0F
#define PANEL_HALF_DEPTH 5.0F
#define FENCE_SHAPES 1

KEPT static NJS_CNK_MODEL *fence_models[FENCE_KINDS] = {
#include "shared/ocean_fence/fence_models.inc"
};
NJS_TEXLIST *ocean_fence_texlists[3] = {
#include "shared/ocean_fence/ocean_fence_texlists.inc"
};
static char *fence_names[FENCE_KINDS] = {"BRIGHT", "DARK  "};
/* one panel's box, which ocean_fence_init stretches over the row */
/* clang-format off */
static struct collision_shape fence_shape = {
    0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, PANEL_HALF_HEIGHT, 0.0F},
    PANEL_LENGTH / 2, PANEL_HALF_HEIGHT, PANEL_HALF_DEPTH, 0, {0, 0, 0},
};
/* clang-format on */

static void fence_destroy(struct task *task);
static void fence_step(struct task *task);
static void fence_draw(struct task *task);

void ocean_fence_init(struct task *task)
{
    /* @hack spelling: shape, an array of one, where a struct and its address
     * would do */
    struct collision_shape shape[1];

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = fence_draw;
    task->step = fence_step;
    task->destructor = fence_destroy;
    shape[0] = fence_shape;
    if (_builtin_fabsf((Float)COUNT(task->work)) > 1.0F) {
        /* @bug logic: puts the box a panel's length further toward -x than the
         * row for a count below -1. the draw lays such a row toward -x, by its
         * test at 0x20a of the object's code. the row's middle then lies at
         * 25 * (count + 1), but the FMAC at 0x8a moves the box to
         * 25 * (count - 1) */
        shape[0].center.x += (COUNT(task->work) - 1) * (PANEL_LENGTH / 2);
        shape[0].radius *= _builtin_fabsf((Float)COUNT(task->work));
    }
    collision_init(task, shape, FENCE_SHAPES, COLLISION_OBJECT);
}

static void fence_destroy(struct task *task)
{
}

static void fence_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (!placement_try_leave(self)) {
        collision_register(self);
    }
}

static void fence_draw(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for the kind and the count,
     * where work would do */
    njSetTexture(ocean_fence_texlists[KIND(task->work)]);
    if (COUNT(task->work) == 0) {
        njCnkEasyDrawModel(fence_models[KIND(task->work)]);
    } else {
        for (i = 0; i < _builtin_fabsf((Float)COUNT(task->work)); i++) {
            njCnkEasyDrawModel(fence_models[KIND(task->work)]);
            if (COUNT(task->work) > 0) {
                njTranslate(NULL, PANEL_LENGTH, 0.0F, 0.0F);
            } else {
                njTranslate(NULL, -PANEL_LENGTH, 0.0F, 0.0F);
            }
        }
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
