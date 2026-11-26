/* the init and callbacks of a plant that sways without end. stg18/plant2.c
 * compiles them for PLANT2, and stg18/plant3.c for PLANT3. the includer
 * defines the plant's art, sway_motion and sway_shape */
#ifndef STG18_SWAY_PLANT_IMPL_H
#define STG18_SWAY_PLANT_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg18/lagoon_util.h"
#include "system/task.h"
#include "system/util.h"

#define FRAME(work) ((work)->scl.z)
#define SPEED(work) njAbs((work)->scl.y + 1.0F)

static void sway_plant_destroy(struct task *task);
static void sway_plant_step(struct task *task);
static void sway_plant_draw(struct task *task);

void plant2_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's order
     * would set the step first */
    task->draw = sway_plant_draw;
    task->step = sway_plant_step;
    task->destructor = sway_plant_destroy;
    FRAME(task->work) = 0.0F;
}

static void sway_plant_destroy(struct task *task)
{
}

static void sway_plant_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: task->work read at each use, where a local would do */
    FRAME(task->work) += SPEED(task->work);
    if (FRAME(task->work) >= sway_shape->nbFrame) {
        FRAME(task->work) -= sway_shape->nbFrame;
        if (FRAME(task->work) >= sway_shape->nbFrame) {
            FRAME(task->work) = 0.0F;
        }
    }
}

static void sway_plant_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float *radius;
    Float unscaled;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for scl, where work would do */
    njScale(NULL, LAGOON_SCALE(task->work), LAGOON_SCALE(task->work),
            LAGOON_SCALE(task->work));
    njSetTexture(&sway_texlist);
    /* @hack spelling: radius assigned inside the read of unscaled, where a
     * statement of its own would do */
    unscaled = *(radius = &sway_object->model->r);
    *radius = unscaled * LAGOON_SCALE(task->work);
    njCnkEasyDrawShapeMotion(sway_object, &sway_motion, sway_shape,
                             FRAME(task->work));
    *radius = unscaled;
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#endif
