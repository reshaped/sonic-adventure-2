#include <ninja.h>
#include "chaostgkinder/character_art.h"
#include "system/port.h"
#include <sg_xpt.h>

#include "chaostgkinder/idle_motion.h"
#include "chaostgkinder/objects.h"
#include "system/task.h"

/* each idle motion's frames a step, and the frame it ends past. under
 * IDLE_REPEAT_SECOND the second motion plays up to ROCK_TOP. it swings back
 * to ROCK_BOTTOM and up again three times, at ROCK_RATE. it then plays up to
 * HOLD_FRAME, holds it HOLD_FRAMES, and plays on to SECOND_END. it plays at
 * SECOND_RATE where it doesn't swing */
#define FIRST_RATE 0.2F
#define FIRST_END 8.0F
#define SECOND_RATE 0.15F
#define SECOND_END 20.0F
#define THIRD_RATE 0.1F
#define THIRD_END 20.0F
#define ROCK_TOP 9.0F
#define ROCK_BOTTOM 8.0F
#define ROCK_RATE 0.05F
#define HOLD_FRAME 13.0F
#define HOLD_FRAMES 90

KEPT static NJS_MOTION *motions[IDLE_MOTIONS] = {
#include "chaostgkinder/principal/motions.inc"
};

static void draw_principal(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&kinder_object_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njCnkEasyDrawMotion(&kinder_principal_model, motions[work->ang.x],
                        work->scl.x);
    njPopMatrixEx();
}

static void principal_destroy(struct task *task)
{
}

void kinder_principal_init(struct task *task)
{
    struct task_work *work = task->work;
    Bool finished = FALSE;

    switch (work->mode) {
    case IDLE_START:
        task->draw = draw_principal;
        task->destructor = principal_destroy;
        finished = TRUE;
        /* fall through */
    case IDLE_FIRST:
        if ((work->scl.x += FIRST_RATE) > FIRST_END) {
            finished = TRUE;
        }
        break;
    case IDLE_SECOND:
        if ((work->scl.x += SECOND_RATE) > SECOND_END) {
            finished = TRUE;
        }
        break;
    case IDLE_THIRD:
        if ((work->scl.x += THIRD_RATE) > THIRD_END) {
            finished = TRUE;
        }
        break;
    case IDLE_REPEAT_SECOND:
        /* ang.y counts the steps, and ang.z the frames held */
        switch (work->ang.y) {
        case 0:
            if ((work->scl.x += SECOND_RATE) > ROCK_TOP) {
                work->scl.x = ROCK_TOP;
                ++work->ang.y;
            }
            break;
        case 2:
        case 4:
        case 6:
            if ((work->scl.x += ROCK_RATE) > ROCK_TOP) {
                work->scl.x = ROCK_TOP;
                ++work->ang.y;
            }
            break;
        case 1:
        case 3:
        case 5:
            /* @hack spelling: the sum with -ROCK_RATE, where subtracting
             * ROCK_RATE would do */
            if ((work->scl.x += -ROCK_RATE) < ROCK_BOTTOM) {
                work->scl.x = ROCK_BOTTOM;
                ++work->ang.y;
            }
            break;
        case 7:
            if ((work->scl.x += SECOND_RATE) > HOLD_FRAME) {
                work->scl.x = HOLD_FRAME;
                work->ang.z = 0;
                ++work->ang.y;
            }
            break;
        case 8:
            if (++work->ang.z >= HOLD_FRAMES) {
                ++work->ang.y;
            }
            break;
        /* @hack redundant: case 9, which default already takes */
        case 9:
        default:
            if ((work->scl.x += SECOND_RATE) > SECOND_END) {
                finished = TRUE;
            }
            break;
        }
        break;
    default:
        finished = TRUE;
        break;
    }
    if (finished) {
        choose_idle_mode(work);
        work->ang.y = 0;
        work->scl.x = 0.0F;
    }
}
