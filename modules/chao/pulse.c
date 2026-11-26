#include <ninja.h>

#include "system/task.h"
#include "chao/garden.h"
#include "system/angle.h"

#define PULSE_STEP 0x1b0

void fun_0c500be0(struct task *task)
{
}

void garden_pulse_scale(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case 0:
        work->ang.x = 0;
        work->mode++;
        /* fall through */
    case 1:
        work->scl.x = (njSin(work->ang.x) + 1.0F) * 0.5F;
        work->ang.x += PULSE_STEP;
        work->ang.x &= ANGLE_MASK;
        break;
    }
}
