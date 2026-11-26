/* AIRCONT, the platform of Meteor Herd and Mad Space: a solid floor with a
 * pulsing light. its light draws with DRAW_CONTROL_SHADOW set, as its floor
 * does, where FRSTAGE's light draws without it */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "shared/common_object.h"
#include "shared/frstage.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* platform_collision.h's placement with no fence, whose three angles all
 * turn the platform */
#define FENCES 0
/* the platform collides only while a player lies within NEAR_RANGE of pos */
#define NEAR_RANGE 120.0F
/* the angle the light's pulse advances a step: a cycle takes about 85 steps */
#define PULSE_RATE 0x300

NJS_TEXLIST *frstage_texlists[] = {&frstage_texlist, NULL};
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

#include "shared/platform_collision.h"

static void frstage_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(&frstage_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njCnkEasyDrawModel(frstage_floor->model);
    model_draw_tinted(
        frstage_light->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
