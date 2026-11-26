/* the pole, POLE in Mad Space's object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg44/lights.h"
#include "stg44/scenery_shapes.h"
#include "system/game_state.h"
#include "system/task.h"

#define POLE_SHAPES 1
/* the light stands at one of LIGHT_PLACES places, LIGHT_SPACING apart up the
 * post. it moves to the next each LIGHT_PERIOD steps, up to the top and back.
 * its places repeat every LIGHT_PHASES moves. a copy at the place it left fades
 * out over the first half of the period */
#define LIGHT_PERIOD 21
#define LIGHT_SPACING 3.5F
#define LIGHT_PLACES 3
#define LIGHT_PHASES (2 * (LIGHT_PLACES - 1))

NJS_TEXLIST *mad_space_pole_textures[] = {&mad_space_pole_texlist, NULL};
/* clang-format off */
static struct collision_shape post = {
    0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 50.0F, 0.0F}, 10.0F, 50.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

static void pole_destroy(struct task *task);
static void pole_step(struct task *actor);
static void pole_draw(struct task *task);

void mad_space_pole_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = pole_draw;
        task->step = pole_step;
        task->destructor = pole_destroy;
        work->mode = 0;
        collision_init(task, &post, POLE_SHAPES, COLLISION_OBJECT);
    }
}

static void pole_destroy(struct task *task)
{
}

static void pole_step(struct task *actor)
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

static void pole_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Uint32 steps;
    int frame;
    int from;
    int to;
    Float fade;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(&mad_space_pole_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    object = &mad_space_pole_object;
    njCnkEasyDrawModel(object->model);
    /* @hack spelling: the assignment to steps inside the remainder, where a
     * statement of its own would do */
    frame = (steps = game_state.steps) % LIGHT_PERIOD;
    from = (steps / LIGHT_PERIOD) % LIGHT_PHASES;
    to = (from + 1) % LIGHT_PHASES;
    if (from >= LIGHT_PLACES) {
        from = LIGHT_PHASES - from;
    }
    if (to >= LIGHT_PLACES) {
        to = LIGHT_PHASES - to;
    }
    fade = (0.5F - (frame * (1.0F / LIGHT_PERIOD))) * 2.0F;
    if (fade < 0.0F) {
        fade = 0.0F;
    }
    object = object->child;
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, object->pos[1] + (to * LIGHT_SPACING), 0.0F);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    njTranslate(NULL, 0.0F, object->pos[1] + (from * LIGHT_SPACING), 0.0F);
    model_draw_tinted(object->model,
                      color_blend(COLOR_CLEAR, COLOR_NONE, fade));
    njPopMatrixEx();
}
