/* the spray, EffectShibukiExec: a splash of water, drawn as 32 models, one a
 * frame */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "stg18/lagoon_util.h"
#include "stg18/spray.h"
#include "system/task.h"

#define SPRAY_LEVEL 1
#define SPRAY_FRAMES 32
#define LAST_FRAME (SPRAY_FRAMES - 1)

#include "stg18/spray/text.inc"
#include "stg18/spray_art.h"

static NJS_CNK_MODEL *spray_models[32] = {
#include "stg18/spray/spray_models.inc"
};

NJS_TEXLIST *spray_texlists[33] = {
#include "stg18/spray/spray_texlists.inc"
};

static void spray_destroy(struct task *task);
static void spray_step(struct task *task);
static void spray_draw(struct task *task);

struct task *spray_create(NJS_POINT3 *pos, Float scale)
{
    struct task *task;

    task = task_create(TASK_WORK, SPRAY_LEVEL, spray_step, SPRAY_TASK_NAME);
    if (task != NULL) {
        struct task_work *work = task->work;

        task->draw = spray_draw;
        task->destructor = spray_destroy;
        work->pos = *pos;
        work->scl.x = work->scl.y = work->scl.z = scale;
        /* @hack redundant: scl.x stored again, through task->work */
        task->work->scl.x = scale;
        /* @hack spelling: timer cleared through task->work, where work
         * would do */
        task->work->timer = 0;
    }
    return task;
}

static void spray_destroy(struct task *task)
{
}

static void spray_step(struct task *task)
{
    struct task_work *work = task->work;

    if (++work->timer >= LAST_FRAME) {
        task_kill(task);
    }
}

static void spray_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: frame read through task->work, where work would do */
    int frame = task->work->timer;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njSetTexture(spray_texlists[frame]);
    /* @hack spelling: the scale read through task->work, where work would
     * do */
    lagoon_model_draw_scaled(spray_models[frame], task->work->scl.x);
    njPopMatrixEx();
}
