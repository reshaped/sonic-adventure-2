/* the idle motions of the doctor and the principal. the work's mode is an
 * idle mode, ang.x the motion it plays, and scl.x the motion's frame */
#ifndef CHAOSTGKINDER_IDLE_MOTION_H
#define CHAOSTGKINDER_IDLE_MOTION_H
#include <sg_xpt.h>
#include <stdlib.h>

#include "system/task.h"
#include "system/util.h"

enum kinder_idle_mode {
    IDLE_START,
    IDLE_FIRST,
    IDLE_SECOND,
    IDLE_THIRD,
    IDLE_REPEAT_SECOND
};

/* the motions the idle modes play, by their index in an occupant's
 * IDLE_MOTIONS */
enum kinder_idle_motion {
    IDLE_MOTION_FIRST,
    IDLE_MOTION_SECOND,
    IDLE_MOTION_THIRD,
    IDLE_MOTIONS
};

/* choose the next idle mode at random, and set ang.x to its motion. each
 * of IDLE_FIRST, IDLE_SECOND and IDLE_THIRD comes three times in ten, and
 * IDLE_REPEAT_SECOND once in ten. IDLE_REPEAT_SECOND plays the second motion
 * too, in a pattern of the doctor's or the principal's own. a choice below
 * IDLE_FIRST_BELOW takes IDLE_FIRST, and so on down the cut-offs */
#define IDLE_FIRST_BELOW 0.3F
#define IDLE_SECOND_BELOW 0.6F
#define IDLE_THIRD_BELOW 0.9F
#pragma inline(choose_idle_mode)
static void choose_idle_mode(struct task_work *work)
{
    Float choice = rand() * RAND_SCALE;

    if (choice < IDLE_FIRST_BELOW) {
        work->mode = IDLE_FIRST;
        work->ang.x = IDLE_MOTION_FIRST;
    } else if (choice < IDLE_SECOND_BELOW) {
        work->mode = IDLE_SECOND;
        work->ang.x = IDLE_MOTION_SECOND;
    } else if (choice < IDLE_THIRD_BELOW) {
        work->mode = IDLE_THIRD;
        work->ang.x = IDLE_MOTION_THIRD;
    } else {
        work->mode = IDLE_REPEAT_SECOND;
        work->ang.x = IDLE_MOTION_SECOND;
    }
}

#endif
