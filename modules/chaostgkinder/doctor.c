#include <ninja.h>
#include "chaostgkinder/character_art.h"
#include "system/port.h"
#include <sg_xpt.h>

#include "chaostgkinder/idle_motion.h"
#include "chaostgkinder/objects.h"
#include "system/task.h"

/* each idle motion's frames a step, and the frame it ends past. under
 * IDLE_REPEAT_SECOND the second motion swings up to ROCK_TOP and back to
 * ROCK_BOTTOM three times. it then swings up again and plays on to
 * SECOND_END, all at SECOND_RATE */
#define FIRST_RATE 0.3F
#define FIRST_END 8.0F
#define SECOND_RATE 0.1F
#define SECOND_END 16.0F
#define THIRD_RATE 0.3F
#define THIRD_END 16.0F
#define ROCK_TOP 7.0F
#define ROCK_BOTTOM 0.0F

KEPT static NJS_MOTION *motions[IDLE_MOTIONS] = {
#include "chaostgkinder/doctor/motions.inc"
};

static void draw_doctor(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&kinder_object_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawMotion(&kinder_doctor_model, motions[work->ang.x],
                        work->scl.x);
    njPopMatrixEx();
}

static void doctor_destroy(struct task *task)
{
}

void kinder_doctor_init(struct task *task)
{
    struct task_work *work = task->work;
    Bool finished = FALSE;

    switch (work->mode) {
    case IDLE_START:
        task->draw = draw_doctor;
        task->destructor = doctor_destroy;
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
        /* ang.z counts the steps: seven swings, then the play on */
        switch (work->ang.z) {
        case 0:
        case 2:
        case 4:
        case 6:
            if ((work->scl.x += SECOND_RATE) > ROCK_TOP) {
                work->scl.x = ROCK_TOP;
                ++work->ang.z;
            }
            break;
        case 1:
        case 3:
        case 5:
            /* @hack spelling: the sum with -SECOND_RATE, where subtracting
             * SECOND_RATE would do */
            if ((work->scl.x += -SECOND_RATE) < ROCK_BOTTOM) {
                work->scl.x = ROCK_BOTTOM;
                ++work->ang.z;
            }
            break;
        /* @hack redundant: case 7, which default already takes */
        case 7:
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
        work->ang.z = 0;
        work->scl.x = 0.0F;
    }
}
