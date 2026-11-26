#include <ninja.h>
#include "chaostgkinder/character_art.h"
#include <stdlib.h>

#include "chaostgkinder/objects.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the prop swings its pivots out a half turn and back, SWING_STEP a frame.
 * it then rests from REST_FRAMES to REST_FRAMES + REST_SPREAD frames, at
 * random. a pivot's angle is PIVOT_REST at the start of a swing */
enum prop_mode { PROP_START, PROP_OUT, PROP_BACK, PROP_REST };
#define SWING_STEP 0x1000
#define REST_FRAMES 240
#define REST_SPREAD 480.0F
#define PIVOT_REST (3 * QUARTER_TURN)

static void draw_prop(struct task *task)
{
    struct task_work *work = task->work;

    kinder_prop_pivot_a.evalflags &= ~NJD_EVAL_UNIT_ANG;
    kinder_prop_pivot_a.ang[0] = work->ang.x + PIVOT_REST;
    kinder_prop_pivot_b.evalflags &= ~NJD_EVAL_UNIT_ANG;
    kinder_prop_pivot_b.ang[0] = work->ang.x + PIVOT_REST;
    njSetTexture(&kinder_object_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkSimpleDrawObject(&kinder_principal_prop);
    njPopMatrixEx();
}

static void prop_destroy(struct task *task)
{
}

void kinder_principal_prop_init(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case PROP_START:
        task->draw = draw_prop;
        task->destructor = prop_destroy;
        work->mode = PROP_OUT;
        work->ang.x = 0;
        break;
    case PROP_OUT:
        if ((work->ang.x += SWING_STEP) > HALF_TURN) {
            work->ang.x = HALF_TURN;
            work->mode = PROP_BACK;
        }
        break;
    case PROP_BACK:
        if ((work->ang.x -= SWING_STEP) < 0) {
            work->ang.x = 0;
            work->timer =
                REST_FRAMES + (int)(rand() * (REST_SPREAD * RAND_SCALE));
            work->mode = PROP_REST;
        }
        break;
    case PROP_REST:
        if (--work->timer == 0) {
            work->mode = PROP_OUT;
        }
        break;
    }
}
