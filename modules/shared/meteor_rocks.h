/* the steps of METEO_MOVE and METEO_SET, Final Rush's and Final Chase's
 * rocks. stg30/meteor.c and stg40/meteor.c include them, and each stage's
 * meteor.h states what they do. the includer defines ROCK, and the rocks'
 * models, texture lists, shapes and spins */
#ifndef SHARED_METEOR_ROCKS_H
#define SHARED_METEOR_ROCKS_H
#include "draw/light.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

static void meteor_destroy(struct task *task);
static void meteor_draw(struct task *task);
static void meteor_move_step(struct task *task);
static void meteor_set_step(struct task *task);

void meteor_move_init(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 rock;

    if (!placement_try_leave(task)) {
        task->step = meteor_move_step;
        task->draw = meteor_draw;
        task->destructor = meteor_destroy;
        rock = ROCK(work);
        collision_init(task, &meteor_shape, 1, COLLISION_OBJECT);
        if (work->collision != NULL) {
            work->collision->shapes->radius =
                meteor_objects[rock]->model->r * ROCK_SHAPE_SCALE;
            /* @hack spelling: rocks 0 and 3 tested in two ifs, where one
             * test of either would do */
            if (rock == 0) {
                work->collision->shapes->radius += ROCK_SHAPE_MARGIN;
            }
            if (rock == 3) {
                work->collision->shapes->radius += ROCK_SHAPE_MARGIN;
            }
            collision_fit_radius(task);
        }
        work->ang.x = RANDOM_TURN();
        work->ang.y = RANDOM_TURN();
        work->ang.z = RANDOM_TURN();
    }
}

void meteor_set_init(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 rock;

    if (!placement_try_leave(task)) {
        task->step = meteor_set_step;
        task->draw = meteor_draw;
        task->destructor = meteor_destroy;
        rock = ROCK(work);
        collision_init(task, &STILL_METEOR_SHAPE, 1, COLLISION_OBJECT);
        if (work->collision != NULL) {
            work->collision->shapes->radius =
                meteor_objects[rock]->model->r * ROCK_SHAPE_SCALE;
            /* @hack spelling: rocks 0 and 3 tested in two ifs, where one
             * test of either would do */
            if (rock == 0) {
                work->collision->shapes->radius += ROCK_SHAPE_MARGIN;
            }
            if (rock == 3) {
                work->collision->shapes->radius += ROCK_SHAPE_MARGIN;
            }
            collision_fit_radius(task);
        }
    }
}

static void meteor_destroy(struct task *task)
{
}

static void meteor_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 rock;

#ifdef FINAL_RUSH_METEOR
    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- ROTSPD X";
        (void)"<- ROTSPD Y";
        (void)"<- MODEL(0-3)";
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
#endif
    light_apply(LIGHT_DEBRIS);
    rock = ROCK(work);
    njSetTexture(meteor_texlists[rock]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njCnkEasyDrawModel(meteor_objects[rock]->model);
    njPopMatrixEx();
    light_apply(light_after_draw);
}

static void meteor_move_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: collision_register and the spin in an else of their
     * own, where statements after the test would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        collision_register(task);
        work->ang.x += DEGREES(ROCK_SPIN_X + work->scl.y);
        work->ang.y += DEGREES(ROCK_SPIN_Y + work->scl.z);
    }
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void meteor_set_step(register struct task *task)
{
    /* @hack spelling: collision_register in an else of its own, where a call
     * after the test would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        collision_register(task);
    }
}

#endif
