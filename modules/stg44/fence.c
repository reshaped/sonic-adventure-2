/* the fence, FENCE in Mad Space's object list: a rail whose light scrolls */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "stg44/fence.h"
#include "stg44/scenery_shapes.h"
#include "system/game_state.h"
#include "system/task.h"

#define FENCE_SHAPES 1

static Sint16 rail_uvs[22] = {
#include "stg44/fence/rail_uvs.inc"
};
static struct texture_anim rail_scroll = {
    TEXTURE_ANIM_SCROLL, 1, 1, NULL, rail_uvs, NULL, NULL, NULL, 0, 0,
};
NJS_TEXLIST *mad_space_fence_textures[] = {&mad_space_fence_texlist, NULL};
/* clang-format off */
static struct collision_shape rail[FENCE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 9.0F, 0.0F},
     2.0F, 5.0F, 65.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

static void fence_destroy(struct task *task);
static void fence_step(struct task *actor);
static void fence_draw(struct task *task);

void mad_space_fence_init(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        task->draw = fence_draw;
        task->step = fence_step;
        task->destructor = fence_destroy;
        work->mode = 0;
        collision_init(task, rail, FENCE_SHAPES, COLLISION_OBJECT);
    }
}

static void fence_destroy(struct task *task)
{
}

static void fence_step(struct task *actor)
{
    /* @hack spelling: task, a copy of actor, where actor itself would do */
    struct task *task = actor;

    /* @hack spelling: the else after the return, where an if alone would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        collision_register(task);
    }
}

static void fence_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- UV SPD";
    }
    njSetTexture(&mad_space_fence_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    object = &mad_space_fence_object;
    njCnkEasyDrawModel(object->model);
    object = object->child;
    texture_anim_scroll(
        object->model, &rail_scroll,
        (game_state.steps * (int)work->scl.x) & (SCROLL_WRAP - 1), 0);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}
