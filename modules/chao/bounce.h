/* the landing a garden egg, fruit, ball or animal makes by its own code */
#ifndef CHAO_BOUNCE_H
#define CHAO_BOUNCE_H
#include "collision/surface.h"
#include "object/mover.h"
#include "system/angle.h"
#include "system/task.h"

/* the floor is probed from BOUNCE_PROBE_HEIGHT above the object, once
 * floor_calls passes BOUNCE_PROBE_AFTER: every fifth call */
#define BOUNCE_PROBE_HEIGHT 4.0F
#define BOUNCE_PROBE_AFTER 3
/* a landing turns a fall back up at BOUNCE_REBOUND of its speed. it scales the
 * speed across x and z by BOUNCE_DRAG */
#define BOUNCE_REBOUND 0.5F
#define BOUNCE_DRAG 0.85F

/* land the object where it has fallen below mover->floor_y.
 * MOVER_WORK_ON_FLOOR says whether this call landed it */
#pragma inline(garden_bounce_on_floor)
static void garden_bounce_on_floor(struct task *object)
{
    struct mover *mover = object->motion.p;
    struct task_work *work = object->work;
    struct angles angles;

    if (mover->floor_calls++ > BOUNCE_PROBE_AFTER) {
        mover->floor_calls = 0;
        mover->floor_y =
            surface_probe_floor(work->pos.x, work->pos.y + BOUNCE_PROBE_HEIGHT,
                                work->pos.z, &angles);
    }
    work->flags &= ~MOVER_WORK_ON_FLOOR;
    if (work->pos.y < mover->floor_y) {
        if (mover->spd.y < 0.0F) {
            mover->spd.y *= -BOUNCE_REBOUND;
        }
        mover->spd.x *= BOUNCE_DRAG;
        mover->spd.z *= BOUNCE_DRAG;
        work->pos.y = mover->floor_y;
        work->flags |= MOVER_WORK_ON_FLOOR;
    }
}

#endif
