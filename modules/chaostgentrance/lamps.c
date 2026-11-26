#include <ninja.h>
#include <sg_xpt.h>

#include "chao/model.h"
#include "chaostgentrance/fixture_art.h"
#include "chaostgentrance/fixture_textures.h"
#include "chaostgentrance/entrance.h"
#include "system/task.h"

static struct task *lamp_manager;

static void lamp_step(struct task *task)
{
}

static void lamp_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&entrance_object_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njCnkEasyDrawObject(&entrance_lamp_model);
    njPopMatrixEx();
}

static void lamp_destroy(struct task *task)
{
}

void entrance_lamps_reset(void)
{
    lamp_manager = NULL;
}

static void animate_lamps(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: a switch of one case, where an if would do */
    switch (work->mode) {
    case 0:
        work->tick = ENTRANCE_TEX_BLUELIGHT01;
        work->mode++;
        break;
    }
    if (work->timer++ > 0) {
        work->timer = 0;
        work->tick++;
        if (work->tick > ENTRANCE_TEX_BLUELIGHT16) {
            work->tick = ENTRANCE_TEX_BLUELIGHT01;
        }
        GARDEN_ANIMATE_TEXTURE(&entrance_lamp_glow, work->tick);
    }
}

void entrance_lamp_create(Float x, Float y, Float z)
{
    struct task *task =
        task_create(TASK_WORK, 2, lamp_step, "ALO_EntranceLampExecutor");
    struct task_work *work;
    if (task) {
        work = task->work;
        work->pos.x = x;
        work->pos.y = y;
        work->pos.z = z;
        task->draw = lamp_draw;
        task->destructor = lamp_destroy;
        if (!lamp_manager) {
            lamp_manager = task_create(TASK_WORK, 2, animate_lamps,
                                       "ALO_EntranceLampManager");
        }
    }
}
