#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_clear_switch.h"
#include "system/task.h"

/* the plate sinks from 0 to PRESSED_HEIGHT above pos, in core_switch_sink_steps
 * steps of SINK_SPEED */
#define PRESSED_HEIGHT (-3.0F)
#define SINK_SPEED (_builtin_fabsf(PRESSED_HEIGHT / core_switch_sink_steps))

/* @hack spelling: the next mode set by an inlined call, where an assignment
 * would do */
#pragma inline(request_mode)
static void request_mode(struct task *task, int mode)
{
    CLEAR_SWITCH(task)->next_mode = mode;
}

void core_switch_sink(struct task *task)
{
    struct task_work *work = task->work;
    /* the rider's fall this step. the force's speed.y takes it negated */
    Float drop;
    if (CLEAR_SWITCH(task)->height - SINK_SPEED <= PRESSED_HEIGHT) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side. it carries the
         * rider down by the travel left */
        drop = CLEAR_SWITCH(task)->height - PRESSED_HEIGHT;
#else
        /* @bug logic: the last step carries the rider up by the travel left,
         * where the plate sinks. drop takes the travel left negated, by the
         * FNEG at 0x2a of the object's code. speed.y takes drop negated
         * again, by the FNEG at 0x64 */
        drop = -CLEAR_SWITCH(task)->height + PRESSED_HEIGHT;
#endif
        CLEAR_SWITCH(task)->height = PRESSED_HEIGHT;
        request_mode(task, SWITCH_DOWN);
    } else {
        drop = SINK_SPEED;
        CLEAR_SWITCH(task)->height -= SINK_SPEED;
    }
    FORCE(task)->speed.y = -drop;
    CLEAR_SWITCH(task)->collision->pos[1] =
        work->pos.y + CLEAR_SWITCH(task)->height;
}

void core_switch_enter_down(struct task *task)
{
}

void core_switch_hold(struct task *task)
{
    FORCE(task)->speed.y = 0.0F;
}
