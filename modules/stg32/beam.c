/* the beam, BLINE in Meteor Herd's object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "system/game_state.h"
#include "system/task.h"

#define MIN_SPEED 0.1F

NJS_TEXLIST *herd_beam_textures[] = {&herd_beam_texlist, NULL};
static void beam_destroy(struct task *task);
static void beam_step(struct task *task);
static void beam_draw(struct task *task);

void herd_beam_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = beam_draw;
        task->step = beam_step;
        task->destructor = beam_destroy;
        work->mode = 0;
    }
}

static void beam_destroy(struct task *task)
{
}

static void beam_step(struct task *task)
{
    struct task_work *work;
    /* @hack redundant: a block no path reaches, which reads task->work */
    if (0) {
        work = task->work;
    }
    if (!placement_try_leave(task)) {
        /* @hack redundant: a block no path reaches, which reads the work's
         * mode and submode. it is kept for its strings, which SHC still
         * places in this source's constants */
        if (0) {
            (void)(work->mode || work->submode);
            (void)"<- ANGLE X";
            (void)"<- ANGLE Y";
            (void)"<- ANGLE Z";
            (void)"<- SIZE";
            (void)"<- SPD";
            (void)"<- LEN(-1~1)";
        }
    }
}

static void beam_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_MODEL *model;
    Float speed;
    int period;
    int phase;

    speed = njAbs(work->scl.y) + MIN_SPEED;
    period = (int)(njAbs(work->scl.x) / speed);
    /* @bug arithmetic: takes game_state.steps modulo a period of 0 where
     * |scl.x| is less than the speed (the call of _modlu at 0x8c504054) */
    phase = game_state.steps % period;
    njSetTexture(&herd_beam_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    model = herd_beam_object->model;
    njTranslate(NULL, phase * speed, 0.0F, 0.0F);
    njScale(NULL, 1.0F, 1.0F, work->scl.z + 1.0F);
    njTranslate(NULL, -model->center.x, -model->center.y, -model->center.z);
    njCnkEasyDrawModel(model);
    njPopMatrixEx();
}
