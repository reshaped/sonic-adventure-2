/* MACHINE's panels, or DITCH's where ENGINE_DITCH is set, as ditch.c compiles
 * this file. each object moves two copies of one panel to and fro, a
 * PANEL_STEP at a time. scl.x holds the first copy's place, and scl.y the
 * second's */
#include <ninja.h>

#include "effect/particle.h"
#include "object/set.h"
#include "sg_xpt.h"
#include "shared/common_object.h"
#include "stg24/machinery.h"
#include "stg24/panel_art.h"
#include "stg24/machinery_textures.h"
#include "system/cache.h"
#include "system/task.h"

/* a copy moving toward - that passes LAST_POSITION jumps to RETURN_POSITION,
 * and turns. one moving toward + that passes 0 jumps to -PANEL_STEP, and
 * turns */
#ifdef ENGINE_DITCH
#define PANEL_STEP 9.4F
#define LAST_POSITION (-50.0F)
#define RETURN_POSITION (-37.6F)
#define PANEL_INIT engine_ditch_init
#define PANEL_TEXTURES engine_ditch_texlist
#else
#include "draw/texture_anim.h"
#include "system/game_state.h"
/* the set editor's labels for the machine's ang.y and scl.z. scl.z scrolls
 * the belt's texture each step */
static const char editor_rotation[12] = "<- ROT Y";
static const char editor_belt_speed[12] = "<- BELT SPD";

#define PANEL_STEP 12.0F
#define LAST_POSITION (-45.0F)
#define RETURN_POSITION (-24.0F)
#define PANEL_INIT engine_machine_init
#define PANEL_TEXTURES engine_machine_texlist
#endif
/* the bits of the work's submode under which each copy returns toward + */
#define FIRST_RETURNING 1
#define SECOND_RETURNING 2
/* a copy has passed 0 once its place lies past PAST_ZERO */
#define PAST_ZERO 0.1F
#define OPAQUE_BLACK 0xff000000

/* the copies move on the step that finds the timer past timer_limit, so every
 * timer_limit + 2 steps. at a move, the first copy has no tint. it then
 * darkens evenly, to black at timer_limit / FADE_RATE on the timer */
#define FADE_RATE 2.0F
static int timer_limit = 10;

static void panel_destroy(struct task *task);
static void panel_step(struct task *task);
static void panel_draw(struct task *task);

void PANEL_INIT(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = panel_draw;
        task->step = panel_step;
        task->destructor = panel_destroy;
        work->scl.x = 0.0F;
        work->scl.y = -PANEL_STEP;
    }
}

static void panel_destroy(struct task *task)
{
}

#define ADVANCE_PANEL(position, returning)                                     \
    do {                                                                       \
        if (work->submode & (returning)) {                                     \
            (position) += PANEL_STEP;                                          \
            if ((position) > PAST_ZERO) {                                      \
                (position) = -PANEL_STEP;                                      \
                work->submode &= ~(returning);                                 \
            }                                                                  \
        } else {                                                               \
            (position) -= PANEL_STEP;                                          \
            if ((position) < LAST_POSITION) {                                  \
                (position) = RETURN_POSITION;                                  \
                work->submode |= (returning);                                  \
            }                                                                  \
        }                                                                      \
    } while (0)

static void panel_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
#ifdef ENGINE_DITCH
        /* @hack redundant: a block no path reaches, kept for its strings, the
         * set editor's labels, and for the frame its calls give the step */
        if (0) {
            njPrintC(NJM_LOCATION(17, 16), "<- ROT X");
            njPrintC(NJM_LOCATION(17, 17), "<- ROT Y");
            njPrintC(NJM_LOCATION(17, 18), "<- ROT Z");
        }
#endif
        if (work->timer > timer_limit) {
            work->timer = 0;
            ADVANCE_PANEL(work->scl.x, FIRST_RETURNING);
            ADVANCE_PANEL(work->scl.y, SECOND_RETURNING);
        } else {
            work->timer++;
        }
    }
}

static void panel_draw(struct task *task)
{
    struct task_work *work = task->work;
#ifndef ENGINE_DITCH
    NJS_CNK_OBJECT *base;
#endif
    NJS_CNK_OBJECT *object;
    Float fade;

    fade = 1.0F - ((Float)work->timer / (Float)timer_limit * FADE_RATE);
    if (fade > 1.0F) {
        fade = 1.0F;
    }
    if (fade < 0.0F) {
        fade = 0.0F;
    }
#ifdef ENGINE_DITCH
    object = &engine_ditch_object;
#else
    base = &engine_machine_object;
#endif
    njSetTexture(&PANEL_TEXTURES);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
#ifdef ENGINE_DITCH
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njPushMatrixEx();
    njTranslate(NULL, object->pos[0], object->pos[1],
                object->pos[2] + work->scl.y);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    njTranslate(NULL, object->pos[0], object->pos[1],
                object->pos[2] + work->scl.x);
#else
    njPushMatrixEx();
    object = base->child;
    texture_anim_scroll(
        object->model, &engine_machine_scroll,
        ((int)work->scl.z * game_state.steps) & (SCROLL_WRAP - 1), 0);
    njCnkEasyDrawModel(object->model);
    object = object->sibling;
    njTranslate(NULL, object->pos[0] - work->scl.y, object->pos[1],
                object->pos[2]);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    njTranslate(NULL, object->pos[0] - work->scl.x, object->pos[1],
                object->pos[2]);
#endif
    model_draw_tinted(object->model,
                      color_blend(OPAQUE_BLACK, COLOR_NONE, fade));
    njPopMatrix(2);
}
