/* the neon, NEON in Pyramid Cave's object list: a sign that spins on a stand.
 * its light flickers at random */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "draw/color.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg28/neon.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/neon/text.inc"
#include "stg28/models/neon.h"

/* SHC expands each call of cave_neon_set_next_mode under #pragma inline. the
 * image also holds a copy of the function, which SHC writes only for an
 * external one. no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void cave_neon_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */
#pragma inline(cave_neon_set_next_mode)

enum neon_mode { MODE_STEADY, MODE_FLICKER };
/* MODE_FLICKER's submodes, in the work's submode */
enum neon_flicker {
    FLICKER_DIM,
    FLICKER_RISE,
    FLICKER_DIM_AGAIN,
    FLICKER_RECOVER
};

/* the task's unk_3c holds the mode the neon takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(task) ((task)->unk_3c.l)
#define NO_NEXT_MODE (-1)

/* while the light flickers, its alpha is ALPHA_OPAQUE times the sine of the
 * fade's angle. the angle runs from DIM_ANGLE up to QUARTER_TURN, and
 * FLICKER_RISE stops it at RISE_ANGLE */
#define ALPHA(work) ((work)->tick)
#define FADE_ANGLE(work) ((work)->timer)
#define ALPHA_OPAQUE 255
#define DIM_ANGLE 0x800
#define RISE_ANGLE 0x2000

/* the sign's turn a step swings by SWING of ang.z either way. ang.x counts the
 * steps, and its shift by SWING_SHIFT gives the swing a period of 256 steps */
#define SWING 0.2F
#define SWING_SHIFT 8

#define WHITE 0xffffff

NJS_TEXLIST *cave_neon_texlists[4] = {
#include "stg28/neon/cave_neon_texlists.inc"
};

static void neon_steady(struct task *task);
static void neon_flicker(struct task *task);

typedef void (*neon_mode_handler)(struct task *task);

KEPT static neon_mode_handler neon_modes[2] = {
#include "stg28/neon/neon_modes.inc"
};
/* the chance on each steady step that a flicker starts */
static Float flicker_chance = 0.01F;
/* what the fade's angle gains or loses each step */
static Sint32 fade_speed = 0x800;
/* the chance on each step of FLICKER_RISE that the light dims again */
static Float dim_again_chance = 0.333F;
/* the chance at the end of FLICKER_DIM_AGAIN that the light rises whole,
 * and not partway again */
static Float recover_chance = 0.5F;

static void neon_destroy(struct task *task);
static void neon_step(struct task *task);
static void neon_draw(struct task *task);
static void neon_take_next_mode(struct task *task);

void cave_neon_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = neon_draw;
    task->step = neon_step;
    task->destructor = neon_destroy;
    task->work->mode = MODE_STEADY;
    NEXT_MODE(task) = NO_NEXT_MODE;
    task->work->ang.x = RANDOM_TURN();
    ALPHA(task->work) = ALPHA_OPAQUE;
    FADE_ANGLE(task->work) = QUARTER_TURN;
}

static void neon_destroy(struct task *task)
{
}

static void neon_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    neon_take_next_mode(task);
    neon_modes[task->work->mode](task);
}

static void neon_draw(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    /* @hack spelling: model, a local for the light's model, where the model in
     * each call would do */
    NJS_CNK_MODEL *model;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njSetTexture(&cave_neon_stand_texlist);
    njCnkEasyDrawModel(&cave_neon_stand_model);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cave_neon_sign_texlist);
    njCnkEasyDrawModel(&cave_neon_sign_model);
    njSetTexture(&cave_neon_light_texlist);
    model = &cave_neon_light_model;
    switch (task->work->mode) {
    case MODE_STEADY:
        njCnkEasyDrawModel(model);
        break;
    case MODE_FLICKER:
        model_draw_tinted(model,
                          ((Uint32)ALPHA(task->work) << ALPHA_SHIFT) | WHITE);
        break;
    }
    njPopMatrixEx();
}

static void neon_take_next_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
        task->work->submode = FLICKER_DIM;
    }
}

void cave_neon_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task) = mode;
}

static void neon_steady(struct task *task)
{
    task->work->ang.y +=
        task->work->ang.z *
        (1.0F + (SWING * njCos(task->work->ang.x << SWING_SHIFT)));
    task->work->ang.x++;
    if (rand() * RAND_SCALE < flicker_chance) {
        cave_neon_set_next_mode(task, MODE_FLICKER);
    }
}

static void neon_flicker(struct task *task)
{
    task->work->ang.y +=
        task->work->ang.z *
        (1.0F + (SWING * njCos(task->work->ang.x << SWING_SHIFT)));
    task->work->ang.x++;
    switch (task->work->submode) {
    case FLICKER_DIM:
        FADE_ANGLE(task->work) -= fade_speed;
        if (FADE_ANGLE(task->work) <= DIM_ANGLE) {
            FADE_ANGLE(task->work) = DIM_ANGLE;
            task->work->submode++;
        }
        break;
    case FLICKER_RISE:
        FADE_ANGLE(task->work) += fade_speed;
        if (rand() * RAND_SCALE < dim_again_chance) {
            task->work->submode++;
        } else if (FADE_ANGLE(task->work) >= RISE_ANGLE) {
            FADE_ANGLE(task->work) = RISE_ANGLE;
            task->work->submode++;
        }
        break;
    case FLICKER_DIM_AGAIN:
        FADE_ANGLE(task->work) -= fade_speed;
        if (FADE_ANGLE(task->work) <= DIM_ANGLE) {
            FADE_ANGLE(task->work) = DIM_ANGLE;
            if (rand() * RAND_SCALE < recover_chance) {
                task->work->submode++;
            } else {
                task->work->submode--;
            }
        }
        break;
    case FLICKER_RECOVER:
        FADE_ANGLE(task->work) += fade_speed;
        if (FADE_ANGLE(task->work) >= QUARTER_TURN) {
            FADE_ANGLE(task->work) = QUARTER_TURN;
            cave_neon_set_next_mode(task, MODE_STEADY);
        }
        break;
    }
    ALPHA(task->work) = njSin(FADE_ANGLE(task->work)) * ALPHA_OPAQUE;
}
