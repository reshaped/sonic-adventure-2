/* SIGNEND's init and callbacks. stg30/sign_end.c compiles them for Final
 * Rush, under FINAL_RUSH_SIGN, and stg40/sign_end.c for Final Chase.
 * stg43/sign_end.c compiles them for Cosmic Wall, under COSMIC_WALL_SIGN. the
 * includer defines the SIGN_END_EDITOR_ captions that the draw names */
#ifndef SHARED_ORBITAL_SIGN_END_TASK_H
#define SHARED_ORBITAL_SIGN_END_TASK_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/sign_end_art.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"

/* the sign's shape, which stays at pos while the model bobs */
static struct collision_shape sign_end_shape = {
#include "shared/orbital/sign_end_task/sign_end_shape.inc"
};

/* the model bobs bob_height above and below pos, and its child, the face,
 * pulses between bright_color and dim_color. bob_rate and pulse_rate are the
 * Angles each wave turns through in a step of game_state.steps */
static Uint32 bright_color = {
#include "shared/orbital/sign_end_task/bright_color.inc"
};

static Uint32 dim_color = {
#include "shared/orbital/sign_end_task/dim_color.inc"
};

static Float bob_height = {
#include "shared/orbital/sign_end_task/bob_height.inc"
};

static Angle bob_rate = {
#include "shared/orbital/sign_end_task/bob_rate.inc"
};

static Angle pulse_rate = {
#include "shared/orbital/sign_end_task/pulse_rate.inc"
};

/* scl.x picks the sign. a sign whose scl.x is above 0 draws
 * sign_end_2_object. one whose scl.x is 1 or more plays SECOND_SOUND of the
 * stage's sound list, not FIRST_SOUND. Cosmic Wall's sign has one model, and
 * turns its face by FACE_TURNS */
#ifdef FINAL_RUSH_SIGN
#define FIRST_SOUND 2
#define SECOND_SOUND 3
#else
#define FIRST_SOUND 10
#define SECOND_SOUND 11
#endif
/* the sign plays its sound each time its timer passes SOUND_STEPS, for
 * SOUND_DURATION + 1 sound updates. Cosmic Wall's sign plays none */
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0
#define SOUND_STEPS 30
#define SOUND_DURATION 20
/* Cosmic Wall's face turns about z by scl.x quarter turns */
#define FACE_TURNS(work) ((Angle)(work)->scl.x)
#define SIGN_END_SHAPES 1

static void sign_end_destroy(struct task *task);
static void sign_end_step(struct task *task);
static void sign_end_draw(struct task *task);

void sign_end_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = sign_end_draw;
        task->step = sign_end_step;
        task->destructor = sign_end_destroy;
        collision_init(task, &sign_end_shape, SIGN_END_SHAPES,
                       COLLISION_OBJECT);
    }
}

static void sign_end_destroy(struct task *task)
{
}

static void sign_end_step(struct task *task)
{
    /* @hack spelling: sign, a copy of task that Cosmic Wall's image needs,
     * where task would do */
    struct task *sign = task;
#ifndef COSMIC_WALL_SIGN
    struct task_work *work = sign->work;
    int sound;
#endif

    /* @hack spelling: collision_register in an else, where a call after the
     * test would do */
    if (placement_try_leave(sign)) {
        return;
    } else {
        collision_register(sign);
    }
#ifndef COSMIC_WALL_SIGN
    if (work->scl.x >= 1.0F) {
        sound = SECOND_SOUND;
    } else {
        sound = FIRST_SOUND;
    }
    if (work->timer++ > SOUND_STEPS) {
        sound_play_at_for(sound, work, SOUND_PRIORITY, SOUND_VOLUME,
                          SOUND_DURATION, &work->pos);
        work->timer = 0;
    }
#endif
}

static void sign_end_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)SIGN_END_EDITOR_ANGLE_X;
        (void)SIGN_END_EDITOR_ANGLE_Y;
        (void)SIGN_END_EDITOR_ANGLE_Z;
#ifdef COSMIC_WALL_SIGN
        (void)SIGN_END_EDITOR_ROT;
#else
        (void)SIGN_END_EDITOR_MODEL;
#endif
    }
#ifdef COSMIC_WALL_SIGN
    njSetTexture(&sign_end_texlist);
    object = &sign_end_object;
#else
    if (work->scl.x > 0.0F) {
        njSetTexture(&sign_end_2_texlist);
        object = &sign_end_2_object;
    } else {
        njSetTexture(&sign_end_texlist);
        object = &sign_end_object;
    }
#endif
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x,
                work->pos.y + (njSin(game_state.steps * bob_rate) * bob_height),
                work->pos.z);
    njRotateEx(&work->ang.x, 0);
    njCnkEasyDrawModel(object->model);
    object = object->child;
#ifdef COSMIC_WALL_SIGN
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateZ(NULL, FACE_TURNS(work) * QUARTER_TURN);
#endif
    model_draw_tinted(
        object->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * pulse_rate) + 1.0F) * 0.5F));
    njPopMatrixEx();
}

#endif
