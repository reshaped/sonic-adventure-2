/* FRSTAGE and FRSTAGE_FAR, the platform of Final Rush and Final Chase: a
 * solid floor with a pulsing light. it carries up to four fences. the set
 * editor labels their nibbles of ang.x SAKU ON/OFF: saku is Japanese for a
 * fence */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "shared/common_object.h"
#include "shared/frstage.h"
#include "shared/orbital/platform.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

/* fence n stands where nibble n of ang.x isn't 0, nibble 0 the lowest */
#define FENCES 4
#define FENCE_BITS 0xf
#define FENCE_SHIFT 4
/* the platform collides only while a player lies within NEAR_RANGE of pos */
#define NEAR_RANGE 80.0F
/* the angle the light's pulse advances a step: a cycle takes about 85 steps */
#define PULSE_RATE 0x300

NJS_TEXLIST *frstage_texlists[] = {&frstage_texlist, NULL};
KEPT static NJS_CNK_OBJECT *fences[FENCES] = {
#include "shared/frstage/fences.inc"
};
static NJS_OBJECT *fence_collisions[FENCES] = {
#include "shared/frstage/fence_collisions.inc"
};
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

#include "shared/platform_collision.h"

static void frstage_draw(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int shift;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- SAKU ON/OFF";
        (void)"<- ANGLE Y";
    }
    njSetTexture(&frstage_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(frstage_floor->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    model_draw_tinted(
        frstage_light->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    shift = 0;
    for (i = 0; i < FENCES; i++) {
        if ((work->ang.x >> shift) & FENCE_BITS) {
            njRotateY(NULL, fences[i]->ang[1]);
            njCnkEasyDrawModel(fences[i]->model);
            matrix_load_parent();
        }
        shift += FENCE_SHIFT;
    }
    njPopMatrix(2);
}
