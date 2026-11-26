/* ARROW in Egg Quarters' object list: an arrow on the floor, whose light
 * pulses */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/color.h"
#include "object/set.h"
#include "shared/tint_pulse.h"
#include "stg26/arrow.h"
#include "stg26/arrow_textures.h"
#include "stg26/quarters_util.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define COLORS 3
#define COLOR(work) ((int)_builtin_fabsf((work)->scl.z) % COLORS)
/* the arrow scales its models by ARROW_SCALE across and up. along z it scales
 * them by (scl.x + 1) * ARROW_SCALE. its light lies LIGHT_LIFT above it */
#define ARROW_SCALE 10.0F
#define LIGHT_LIFT 0.0005F

static void arrow_destroy(struct task *task);
static void arrow_step(struct task *task);
static void arrow_draw(struct task *task);

KEPT static NJS_CNK_MODEL *base_models[COLORS] = {
#include "stg26/arrow/base_models.inc"
};
KEPT static NJS_CNK_MODEL *light_models[COLORS] = {
#include "stg26/arrow/light_models.inc"
};
NJS_TEXLIST *quarters_arrow_texlists[4] = {
#include "stg26/arrow/quarters_arrow_texlists.inc"
};
static char *color_names[COLORS] = {"RED  ", "GREEN", "BLUE "};

void quarters_arrow_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = arrow_draw;
        task->step = arrow_step;
        task->destructor = arrow_destroy;
    }
}

static void arrow_destroy(struct task *task)
{
}

static void arrow_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, and its
     * return, where the call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void arrow_draw(struct task *task)
{
    /* @hack spelling: task->work read again for scl.x and scl.z, where work
     * would do */
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    int phase;
    Uint32 color;

    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    scale.x = scale.y = ARROW_SCALE;
    scale.z = (task->work->scl.x + 1.0F) * ARROW_SCALE;
    phase = game_state.steps % TINT_PULSE_STEPS;
    /* @hack type: the level converted through int, where a conversion to
     * Uint32 would do */
    if (phase < TINT_PULSE_RISE) {
        color = (int)(phase * TINT_PULSE_RISE_RATE);
    } else {
        color = (int)(((TINT_PULSE_STEPS - 1) - phase) * TINT_PULSE_FALL_RATE);
    }
    color = (color << ALPHA_SHIFT) | (color << RED_SHIFT) |
            (color << GREEN_SHIFT) | color;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(quarters_arrow_texlists[COLOR(task->work)]);
    njPushMatrixEx();
    quarters_model_draw_scaled_v(base_models[COLOR(task->work)], &scale);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njTranslate(NULL, 0.0F, LIGHT_LIFT, 0.0F);
    njFogDisable();
    quarters_model_draw_tinted_scaled_v(light_models[COLOR(task->work)], color,
                                        &scale);
    njFogEnable();
    njPopMatrixEx();
}
