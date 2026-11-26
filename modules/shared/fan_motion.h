/* FAN, the fan of Death Chamber and Egg Quarters. each stage's fan.c defines
 * FAN_INIT, the name of the object list's step, and a fan_draw. the fan starts
 * at a random angle, and turns by ang.z a step */
#ifndef SHARED_FAN_MOTION_H
#define SHARED_FAN_MOTION_H
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the fan's angle about z */
#define FAN_ANGLE(task) ((task)->unk_3c.l)

static void fan_destroy(struct task *task);
static void fan_step(struct task *task);
static void fan_draw(struct task *task);

void FAN_INIT(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = fan_draw;
        task->step = fan_step;
        task->destructor = fan_destroy;
        FAN_ANGLE(task) = (Float)rand() * ((FULL_TURN - 1) * RAND_SCALE);
    }
}

static void fan_destroy(struct task *task)
{
}

static void fan_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        FAN_ANGLE(task) += task->work->ang.z;
    }
}

#endif
