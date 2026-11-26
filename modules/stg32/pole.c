/* the pole, POLE in Meteor Herd's object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "system/game_state.h"
#include "system/task.h"

#define POLE_SHAPES 1
/* the head turns game_state.steps << TURN_SHIFT. the lamp's color runs from
 * bright_color to dim_color and back with the sine of game_state.steps *
 * PULSE_RATE */
#define TURN_SHIFT 8
#define PULSE_RATE 0x300

NJS_TEXLIST *herd_pole_textures[] = {&herd_pole_texlist, NULL};
/* every pole's collision record uses this shape in place, so the poles
 * share it */
/* clang-format off */
static struct collision_shape pole_shape[POLE_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 20.0F, 0.0F}, 10.0F, 20.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

static void pole_destroy(struct task *task);
static void pole_step(struct task *task);
static void pole_draw(struct task *task);

void herd_pole_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = pole_draw;
    task->step = pole_step;
    task->destructor = pole_destroy;
    work->mode = 0;
    collision_init_in_place(task, pole_shape, POLE_SHAPES, COLLISION_OBJECT);
}

static void pole_destroy(struct task *task)
{
}

static void pole_step(struct task *task)
{
    /* @hack redundant: a read of task->work that the step never uses */
    struct task_work *work = task->work;
    (void)work;
    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void pole_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *lamp;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&herd_pole_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    object = herd_pole_object;
    njCnkEasyDrawModel(object->model);
    njRotateY(NULL, game_state.steps << TURN_SHIFT);
    object = object->child;
    njCnkEasyDrawModel(object->model);
    lamp = object->child;
    model_draw_tinted(
        lamp->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    njPopMatrixEx();
}
