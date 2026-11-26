/* the steps, draw and destructor of the Hero and Dark Gardens' placed scenery,
 * which each garden's props.c includes. the includer defines
 * GARDEN_PROP_TEXLIST, the texture list the props draw with. it may define
 * GARDEN_SHADOW_Y, the height above pos that a shadow draws at */
#ifndef SHARED_GARDEN_PROP_H
#define SHARED_GARDEN_PROP_H
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "object/set.h"
#include "system/task.h"

/* the id, push and guard of each prop's shape */
#define GARDEN_PROP_ID (-52)
#define GARDEN_PROP_PUSH PUSH_SOLID(PUSH_ALL_BUT_PICKUPS)
#define GARDEN_PROP_GUARD SHAPE_GUARD_LEVEL(3)
/* the prop's model and its shadow's, or null for none */
#define PROP_MODEL(task) ((NJS_CNK_OBJECT *)(task)->extra.p)
#define PROP_SHADOW(task) ((NJS_CNK_OBJECT *)(task)->force.p)
/* how far a prop's model draws below its shadow, or below pos without one */
#define PROP_DROP 1.0F

static void garden_prop_solid_step(struct task *task)
{
    /* @hack redundant: work, which only blocks no path reaches use */
    struct task_work *work;
    /* @hack redundant: a read of work from task, in a block no path
     * reaches */
    if (0) {
        work = task->work;
    }
    if (!placement_try_leave(task)) {
        /* @hack redundant: a test of work's mode, submode and flags, in a
         * block no path reaches */
        if (0) {
            if (work->mode || work->submode || work->flags) {
            }
        }
        collision_register(task);
    }
}

static void garden_prop_step(struct task *task)
{
    /* @hack redundant: work, which only blocks no path reaches use */
    struct task_work *work;
    /* @hack redundant: a read of work from task, in a block no path
     * reaches */
    if (0) {
        work = task->work;
    }
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a test of work's mode, submode and flags, in a block
     * no path reaches */
    if (0) {
        if (work->mode || work->submode || work->flags) {
        }
    }
}

/* ang turns the model in the order njRotateY, njRotateZ and njRotateX, and
 * the shadow by ang.y alone. the model scales by scl.x + 1 on every axis */
static void garden_prop_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale = work->scl.x + 1.0F;

    njSetTexture(&GARDEN_PROP_TEXLIST);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (PROP_SHADOW(task)) {
#ifdef GARDEN_SHADOW_Y
        njTranslate(NULL, 0.0F, GARDEN_SHADOW_Y, 0.0F);
#endif
        njCnkEasyDrawObject(PROP_SHADOW(task));
    }
    njTranslate(NULL, 0.0F, -PROP_DROP, 0.0F);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawObject(PROP_MODEL(task));
    njPopMatrixEx();
}

static void garden_prop_destroy(struct task *task)
{
    /* the models aren't the task's, so task_destroy mustn't free them */
    task->extra.p = NULL;
    task->force.p = NULL;
}

/* make task a prop that draws model, and shadow unless it is null. update is
 * garden_prop_solid_step for a prop with a collision record, which registers
 * it, or else garden_prop_step */
/* @hack spelling: a macro, where a function setting the same fields would do */
#define GARDEN_PROP_PLACE(task, update, model, shadow)                         \
    do {                                                                       \
        (task)->step = (update);                                               \
        (task)->draw = garden_prop_draw;                                       \
        (task)->destructor = garden_prop_destroy;                              \
        (task)->extra.p = (model);                                             \
        (task)->force.p = (shadow);                                            \
    } while (0)

#endif
