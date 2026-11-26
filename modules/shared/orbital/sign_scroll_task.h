/* the init and callbacks of a sign whose texture scrolls. shared/sign_end_e.c
 * compiles them for SIGNENDE, and stg30/sign_trick.c for Final Rush's
 * SIGNTRICK, under SIGN_TRICK. the includer defines scroll_u_speed and
 * scroll_v_speed, and SIGN_INIT, SIGN_OBJECT, SIGN_TEXLIST and SIGN_ANIM as its
 * own names */
#ifndef SHARED_ORBITAL_SIGN_SCROLL_TASK_H
#define SHARED_ORBITAL_SIGN_SCROLL_TASK_H
#include <ninja.h>
#include <sg_xpt.h>
#include "draw/texture_anim.h"
#include "object/set.h"
#include "system/game_state.h"
#include "system/task.h"

static void sign_scroll_destroy(struct task *task);
static void sign_scroll_step(struct task *task);
static void sign_scroll_draw(struct task *task);

void SIGN_INIT(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = sign_scroll_draw;
        task->step = sign_scroll_step;
        task->destructor = sign_scroll_destroy;
    }
}

static void sign_scroll_destroy(struct task *task)
{
}

static void sign_scroll_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * captions, which SHC still places in this source's constants. its local
     * and draws are a stand-in that gives the step the image's saved
     * registers */
    if (0) {
        NJS_CNK_MODEL *model = SIGN_OBJECT.model;

        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
#ifndef SIGN_TRICK
        (void)"<- TEX";
#endif
        njCnkEasyDrawModel(model);
        njCnkEasyDrawModel(model);
    }
}

static void sign_scroll_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

#ifdef SIGN_TRICK
    njSetTexture(&SIGN_TEXLIST);
#else
    if (work->scl.x >= 1.0F) {
        njSetTexture(&sign_end_e_2_texlist);
    } else {
        njSetTexture(&SIGN_TEXLIST);
    }
#endif
    object = &SIGN_OBJECT;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    texture_anim_scroll(object->model, &SIGN_ANIM,
                        (game_state.steps * scroll_u_speed) & (SCROLL_WRAP - 1),
                        (game_state.steps * scroll_v_speed) &
                            (SCROLL_WRAP - 1));
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

#endif
