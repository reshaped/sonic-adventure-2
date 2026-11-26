/* the shaft of shared/shaft.h. shared/spinner.c compiles this source again
 * for SPINNER, with a model, a motion and textures of its own */
#include <ninja.h>

#include "object/set.h"
#include "shared/shaft.h"
#include "system/task.h"
#include "system/util.h"

#define FRAME(work) ((work)->scl.z)
#define FRAME_STEP(work) ((work)->scl.y + 1.0F)
#define SCALE(work) ((work)->scl.x + 1.0F)

static void shaft_destroy(struct task *task);
static void shaft_step(struct task *task);
static void shaft_draw(struct task *task);

NJS_TEXLIST *shaft_texlists[] = {&shaft_texlist, NULL};

void shaft_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = shaft_draw;
        task->step = shaft_step;
        task->destructor = shaft_destroy;
        FRAME(task->work) = 0.0F;
    }
}

static void shaft_destroy(struct task *task)
{
}

static void shaft_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        FRAME(task->work) += FRAME_STEP(task->work);
        if (FRAME(task->work) >= shaft_motion->nbFrame) {
            FRAME(task->work) -= shaft_motion->nbFrame;
            if (FRAME(task->work) >= shaft_motion->nbFrame) {
                FRAME(task->work) = 0.0F;
            }
        }
    }
}

static void shaft_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for scl, where work would do */
    njScale(NULL, SCALE(task->work), SCALE(task->work), SCALE(task->work));
    njSetTexture(&shaft_texlist);
    /* @hack spelling: task->work read again for the frame, where work would
     * do */
    njCnkEasyDrawMotion(&shaft_object, shaft_motion, FRAME(task->work));
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
