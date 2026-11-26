/* PLANT in Dry Lagoon's object list: a long water plant that sways a frame a
 * step. stg18/plant4.c compiles this source again, over models of its own */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "stg18/lagoon_util.h"
#include "stg18/plant.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define PLANT_FRAMES 49

/* the frame the plant's sway starts from, which plant_init picks */
#define FIRST_FRAME(task) ((task)->unk_3c.l)

static void plant_destroy(struct task *task);
static void plant_step(struct task *task);
static void plant_draw(struct task *task);

void plant_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = plant_draw;
    task->step = plant_step;
    task->destructor = plant_destroy;
    FIRST_FRAME(task) = (Sint32)((Float)rand() * (PLANT_FRAMES * RAND_SCALE));
}

static void plant_destroy(struct task *task)
{
}

static void plant_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void plant_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame = (FIRST_FRAME(task) + game_state.steps) % PLANT_FRAMES;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njSetTexture(plant_texlists[frame]);
    /* @hack spelling: the scale read through task->work, where work would
     * do */
    lagoon_model_draw_scaled(plant_models[frame], task->work->scl.x + 1.0F);
    njPopMatrixEx();
}
