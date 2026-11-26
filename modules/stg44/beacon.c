/* the beacon, SL in Mad Space's object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg44/lights.h"
#include "stg44/props.h"
#include "stg44/scenery_shapes.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"

#define LIGHTS 3
/* the lamp's color runs from bright_color to dim_color and back with the
 * sine of game_state.steps * PULSE_RATE */
#define PULSE_RATE 0x280
#define BEACON_SHAPES 1

/* whether mad_space_sl_build compiled all three models */
static Bool built = FALSE;
static struct model_build models[LIGHTS] = {
    {MODEL_BUILD_ALONE, &mad_space_sl_first_texlist, &mad_space_sl_first_object,
     NULL},
    {MODEL_BUILD_ALONE, &mad_space_sl_second_texlist,
     &mad_space_sl_second_object, NULL},
    {MODEL_BUILD_ALONE, &mad_space_sl_third_texlist, &mad_space_sl_third_object,
     NULL},
};
NJS_TEXLIST *mad_space_sl_textures[LIGHTS + 1] = {
#include "stg44/beacon/mad_space_sl_textures.inc"
};
KEPT static NJS_CNK_OBJECT *housings[LIGHTS] = {
#include "stg44/beacon/housings.inc"
};
/* clang-format off */
static struct collision_shape post[BEACON_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_HURTABLE,
     {0.0F, 5.0F, 0.0F}, 10.0F, 5.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

static void beacon_destroy(struct task *task);
static void beacon_draw(struct task *task);
static void beacon_step(struct task *actor);

Uint8 *mad_space_sl_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, models, LIGHTS) == LIGHTS) {
        built = TRUE;
    }
    return buffer;
}

void mad_space_sl_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->step = beacon_step;
        task->draw = beacon_draw;
        task->destructor = beacon_destroy;
        collision_init(task, post, BEACON_SHAPES, COLLISION_OBJECT);
    }
}

static void beacon_destroy(struct task *task)
{
}

static void beacon_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 kind = (Uint32)work->scl.x % LIGHTS;
    NJS_CNK_OBJECT *housing;
    NJS_CNK_OBJECT *lamp;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- MODEL(0-2)";
    }
    njSetTexture(mad_space_sl_textures[kind]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    housing = housings[kind];
    if (built) {
        njDirectDrawObject(models[kind].built);
    } else {
        njCnkEasyDrawModel(housing->model);
    }
    lamp = housing->child;
    model_draw_tinted(
        lamp->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    njPopMatrixEx();
}

static void beacon_step(struct task *actor)
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
