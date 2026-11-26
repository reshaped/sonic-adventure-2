/* the light bar, LIGHTBAR in a stage's object list: a bobbing bar of seven
 * lights. the lit light runs along the bar, and the light it leaves fades */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/light_bar.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"

/* the bar's lights: the model's first child and that child's siblings */
#define LIGHTS 7
/* the lit light moves on every 1 << LIGHT_SHIFT steps of the game. the light
 * it leaves fades from bright_color to dim_color over the FADE_STEPS steps
 * after the move */
#define LIGHT_SHIFT 4
#define LIGHT_PHASE ((1 << LIGHT_SHIFT) - 1)
#define FADE_STEPS 8
/* the height of the bar's shape above pos, before the bob */
#define SHAPE_HEIGHT 19.5F

#include "shared/light_bar/text.inc"
#include "shared/orbital/light_bar.h"

NJS_TEXLIST *light_bar_texlists[2] = {
#include "shared/light_bar/light_bar_texlists.inc"
};
/* a cylinder along the bar, which the step bobs with the model */
#define LIGHT_BAR_SHAPES 1
/* clang-format off */
static struct collision_shape light_bar_shape = {
    0, SHAPE_CYLINDER2, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    0, 0, {0.0F, SHAPE_HEIGHT, 0.0F}, 10.0F, 70.0F, 0.0F, 0,
    {0, 0, QUARTER_TURN},
};
/* clang-format on */
/* the bob moves the bar up to bob_height either way of pos. its phase turns
 * bob_rate a step of the game */
static Float bob_height = 5.0F;
static Angle bob_rate = 0x180;
/* the unlit lights draw tinted dim_color, and the lit light untinted. the
 * light the lit one leaves blends from bright_color to dim_color */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff303030;

static void light_bar_destroy(struct task *task);
static void light_bar_step(struct task *task);
static void light_bar_draw(struct task *task);

void light_bar_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = light_bar_draw;
    task->step = light_bar_step;
    task->destructor = light_bar_destroy;
    collision_init(task, &light_bar_shape, LIGHT_BAR_SHAPES, COLLISION_OBJECT);
}

static void light_bar_destroy(struct task *task)
{
}

static void light_bar_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the test of the collision record behind an else, where
     * a test of its own after the return would do */
    if (placement_try_leave(task)) {
        return;
    } else if (work->collision != NULL) {
        work->collision->shapes->center.y =
            (njSin(game_state.steps * bob_rate) * bob_height) + SHAPE_HEIGHT;
    }
    collision_register(task);
}

static void light_bar_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    int fading;
    int lit;
    int phase;
    Float fade;
    int i;

    /* @hack redundant: a block no path reaches, kept for its two strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- DIRECTION";
    }
    njSetTexture(&light_bar_texlist);
    object = &light_bar_object;
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x,
                work->pos.y + (njSin(game_state.steps * bob_rate) * bob_height),
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njCnkEasyDrawModel(object->model);
    object = object->child;
    /* @hack spelling: phase set to the steps inside the expression that
     * picks the fading light, where a statement of its own would do */
    fading = (phase = game_state.steps, (Uint32)phase >> LIGHT_SHIFT) % LIGHTS;
    lit = (fading + 1) % LIGHTS;
    phase &= LIGHT_PHASE;
    if (phase < FADE_STEPS) {
        fade = 1.0F - ((Float)phase * (1.0F / FADE_STEPS));
    } else {
        fade = 0.0F;
    }
    if (work->scl.x >= 1.0F) {
        for (i = LIGHTS - 1; i >= 0 && object != NULL;
             i--, object = object->sibling) {
            njTranslateEx((NJS_VECTOR *)object->pos);
            if (i == fading) {
                model_draw_tinted(object->model,
                                  color_blend(dim_color, bright_color, fade));
            } else if (i == lit) {
                njCnkEasyDrawModel(object->model);
            } else {
                model_draw_tinted(object->model, dim_color);
            }
            matrix_load_parent();
        }
    } else {
        for (i = 0; i < LIGHTS && object != NULL;
             i++, object = object->sibling) {
            njTranslateEx((NJS_VECTOR *)object->pos);
            if (i == fading) {
                model_draw_tinted(object->model,
                                  color_blend(dim_color, bright_color, fade));
            } else if (i == lit) {
                njCnkEasyDrawModel(object->model);
            } else {
                model_draw_tinted(object->model, dim_color);
            }
            matrix_load_parent();
        }
    }
    njPopMatrix(2);
}
