#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "stg23/oil_derrick.h"
#include "system/task.h"
#include "system/util.h"

/* the motion's frame, an integer, since unk_3c is a Sint32 */
#define FRAME(task) ((task)->unk_3c.l)
#define DERRICK_KINDS 1
#define SCALE(work) ((work)->scl.x + 1.0F)
#define KIND(task) ((int)njAbs((task)->work->scl.z) % DERRICK_KINDS)

static void derrick_destroy(struct task *task);
static void derrick_step(struct task *task);
static void derrick_draw(struct task *task);

static NJS_CNK_OBJECT *models[DERRICK_KINDS] = {&oil_derrick_object};
NJS_TEXLIST *oil_derrick_texlists[] = {&oil_derrick_texlist, NULL};
static NJS_MOTION *motions[DERRICK_KINDS] = {&oil_derrick_motion};
static NJS_MOTION *shapes[DERRICK_KINDS] = {&oil_derrick_shape};
static const char *kind_names[DERRICK_KINDS] = {""};

void oil_derrick_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = derrick_draw;
        task->step = derrick_step;
        task->destructor = derrick_destroy;
        /* @hack redundant: KIND's pick from a table of one, where
         * oil_derrick_motion would do */
        FRAME(task) =
            (Float)rand() * (motions[KIND(task)]->nbFrame - 1) * RAND_SCALE;
    }
}

static void derrick_destroy(struct task *task)
{
}

static void derrick_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        FRAME(task) += task->work->scl.y + 1.0F;
        /* @hack redundant: KIND's picks from a table of one, where
         * oil_derrick_shape would do */
        if ((Uint32)FRAME(task) >= shapes[KIND(task)]->nbFrame) {
            FRAME(task) -= shapes[KIND(task)]->nbFrame;
            if ((Uint32)FRAME(task) >= shapes[KIND(task)]->nbFrame) {
                FRAME(task) = 0;
            }
        }
    }
}

static void derrick_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack redundant: KIND's picks from tables of one, where
     * &oil_derrick_texlist, &oil_derrick_object, &oil_derrick_motion and
     * &oil_derrick_shape would do */
    njSetTexture(oil_derrick_texlists[KIND(task)]);
    /* @hack spelling: task->work read again for scl.x, where work would do */
    njScale(NULL, SCALE(task->work), SCALE(task->work), SCALE(task->work));
    njCnkEasyDrawShapeMotion(models[KIND(task)], motions[KIND(task)],
                             shapes[KIND(task)], (Float)FRAME(task));
    njPopMatrixEx();
}
