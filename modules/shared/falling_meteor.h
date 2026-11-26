/* meteor_fall_init and its callbacks, for Final Rush's and Final Chase's
 * meteor sources. the includer defines the meteor's model, spins and sparks.
 * the caller of meteor_fall_init sets the work's scl to the meteor's velocity
 * a step. it sets ang.z to the meteor's life in steps. the timer counts the
 * steps, and the meteor ends past its life */
#ifndef SHARED_FALLING_METEOR_H
#define SHARED_FALLING_METEOR_H
#include "effect/burst.h"
#include "effect/particle.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a spark starts FALL_SPARK_LEAD of the meteor's velocity ahead of it. its
 * color blends toward fall_spark_end_color from FADE_START of the meteor's
 * life on. it blends at FADE_RATE for each unit of life */
#define FALL_SPARK_LEAD 0.5F
#define FADE_START 0.5F
#define FADE_RATE 2.0F
#define VELOCITY(work) ((work)->scl)
#define LIFE(work) ((work)->ang.z)

static void meteor_fall_destroy(struct task *task);
static void meteor_fall_draw(struct task *task);
static void meteor_fall_step(struct task *task);

void meteor_fall_init(struct task *task)
{
    task->step = meteor_fall_step;
    task->draw = meteor_fall_draw;
    task->destructor = meteor_fall_destroy;
}

static void meteor_fall_destroy(struct task *task)
{
}

static void meteor_fall_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&meteor_fall_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(meteor_fall_object->model);
    njPopMatrixEx();
}

static void meteor_fall_step(struct task *task)
{
    struct task_work *work = task->work;
    struct particle *spark;
    NJS_POINT3 pos;
    Float age;
    Float fade;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    work->pos.x += VELOCITY(work).x;
    work->pos.y += VELOCITY(work).y;
    work->pos.z += VELOCITY(work).z;
#else
    /* @hack spelling: the sum's components walked by pointers, where named
     * components would do */
    Float *at = &work->pos.x;
    Float *by = &VELOCITY(work).x;

    *at++ += *by++;
    *at++ += *by++;
    *at += *by;
#endif
    if (RANDOM_UNIT() < FALL_SPARK_CHANCE) {
        pos.x = work->pos.x + (VELOCITY(work).x * FALL_SPARK_LEAD);
        pos.y = work->pos.y + (VELOCITY(work).y * FALL_SPARK_LEAD);
        pos.z = work->pos.z + (VELOCITY(work).z * FALL_SPARK_LEAD);
        spark = burst_create_fast(
            &pos, &VELOCITY(work),
            fall_spark_size + (RANDOM_UNIT() * fall_spark_size_range), 0.0F);
        if (spark != NULL) {
            age = (Float)work->timer / (Float)LIFE(work);
            if (age > FADE_START) {
                fade = (age - FADE_START) * FADE_RATE;
            } else {
                fade = 0.0F;
            }
            spark->color =
                color_blend(fall_spark_color, fall_spark_end_color, fade);
        }
    }
    work->ang.x += DEGREES(spin_x);
    work->ang.y += DEGREES(spin_y);
    if (work->timer++ > LIFE(work)) {
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    }
}

#endif
