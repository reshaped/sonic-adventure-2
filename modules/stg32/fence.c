/* the fence, FENCE in Meteor Herd's object list: a rail whose light pulses */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg32/fence.h"
#include "stg32/scenery_shapes.h"
#include "system/game_state.h"
#include "system/task.h"

#define FENCE_SHAPES 1
/* the light's color runs from bright_color to dim_color and back with the
 * sine of game_state.steps * PULSE_RATE */
#define PULSE_RATE 0x300

NJS_TEXLIST *herd_fence_textures[] = {&herd_fence_texlist, NULL};

/* every fence's collision record uses this shape in place, so the fences
 * share it */
/* clang-format off */
static struct collision_shape rail_shape[FENCE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 10.0F, 0.0F},
     60.0F, 10.0F, 2.5F, 0, {0, 0, 0}},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

static void fence_destroy(struct task *task);
static void fence_step(struct task *actor);
static void fence_draw(struct task *task);

void herd_fence_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = fence_draw;
        task->step = fence_step;
        task->destructor = fence_destroy;
        collision_init_in_place(task, rail_shape, FENCE_SHAPES,
                                COLLISION_OBJECT);
    }
}

static void fence_destroy(struct task *task)
{
}

static void fence_step(struct task *actor)
{
    /* @hack spelling: task, a copy of actor, where the parameter alone would
     * do */
    struct task *task = actor;

    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void fence_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *light;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&herd_fence_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    object = &herd_fence_object;
    njCnkEasyDrawModel(object->model);
    light = object->child;
    model_draw_tinted(
        light->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    njPopMatrixEx();
}
