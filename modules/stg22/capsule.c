/* CAPSULE in Crazy Gadget's object list: two capsules that rise
 * CAPSULE_TRAVEL, half a cycle apart, and start again. a cycle lasts scl.x
 * steps, truncated, and at least 1. ang.y turns them */
#include "stg22/fixture_models.h"
#include <sg_xpt.h>
#include <ninja.h>

#include "object/set.h"
#include "stg22/prop_textures.h"
#include "stg22/scenery.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"

#define CAPSULE_TRAVEL 180.0F

NJS_TEXLIST *gadget_capsule_texlists[] = {&gadget_capsule_texlist, NULL};

static void capsule_destroy(struct task *task);
static void capsule_step(struct task *task);
static void capsule_draw(struct task *task);

void gadget_capsule_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = capsule_draw;
        task->step = capsule_step;
        task->destructor = capsule_destroy;
    }
}

static void capsule_destroy(struct task *task)
{
}

static void capsule_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- ROT Y";
        (void)"<- SPD";
    }
}

static void capsule_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame;
    int period;
    Float first;
    Float second;

    njSetTexture(&gadget_capsule_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    period = (int)work->scl.x;
    if (period <= 0) {
        period = 1;
    }
    frame = game_state.steps;
    first = (Float)(frame % period) / period;
    second = (Float)((frame + (period / 2)) % period) / period;
    njTranslate(NULL, 0.0F, first * CAPSULE_TRAVEL, 0.0F);
    njCnkEasyDrawModel(gadget_capsule_object->model);
    matrix_load_parent();
    njTranslate(NULL, 0.0F, second * CAPSULE_TRAVEL, 0.0F);
    njCnkEasyDrawModel(gadget_capsule_object->model);
    njPopMatrix(2);
}
