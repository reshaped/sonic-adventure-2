/* the sign's init and callbacks, which shared/sign.c compiles for Prison
 * Lane's SIGNA and stg11/sign.c for Iron Gate's. the includer defines the
 * SIGN_EDITOR_ captions */
#ifndef SHARED_PRISON_MACHINERY_SIGN_TASK_H
#define SHARED_PRISON_MACHINERY_SIGN_TASK_H

#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/sign.h"
#include "system/task.h"

/* the panel's pulse, in steps: blink_light_gray's period, rest and fade */
#define PULSE_PERIOD 80
#define PULSE_REST 40
#define PULSE_FADE 8

/* SIGN_VIEW_TEST(work) opens the draw, and may return from it. Iron Gate's
 * culls the sign, and Prison Lane's does nothing */
#ifndef SIGN_VIEW_TEST
#define SIGN_VIEW_TEST(work) ((void)0)
#endif

/* the sign's texture list and object, which the art defines below this body.
 * the object's child is the panel */
static NJS_TEXLIST sign_texlist;
static NJS_CNK_OBJECT sign_object;

static void sign_destroy(struct task *task);
static void sign_step(struct task *task);
static void sign_draw(struct task *task);

void sign_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = sign_draw;
    task->step = sign_step;
    task->destructor = sign_destroy;
}

static void sign_destroy(struct task *task)
{
}

/* sign_init gives the sign no collision record, so the step's
 * collision_register does nothing */
static void sign_step(struct task *task)
{
    /* @hack redundant: the work read into a local nothing uses */
    struct task_work *work = task->work;

    /* @hack spelling: collision_register in an else, where a call after the
     * test would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        collision_register(task);
    }
}

static void sign_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *panel;

    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)SIGN_EDITOR_ROT_X;
        (void)SIGN_EDITOR_ROT_Y;
        (void)SIGN_EDITOR_ROT_Z;
    }
    SIGN_VIEW_TEST(work);
    njSetTexture(&sign_texlist);
    object = &sign_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(object->model);
    panel = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)panel->pos);
    model_draw_tinted(panel->model,
                      blink_light_gray(PULSE_PERIOD, PULSE_REST, PULSE_FADE));
    njPopMatrix(1);
}

#endif
