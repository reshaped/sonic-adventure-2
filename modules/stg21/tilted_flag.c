#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/ocean_util.h"
#include "stg21/flag.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define SUPPORTS 2
#define CLOTH_PITCH NJM_DEG_ANG(70)
#define SUPPORT(work) ((int)njAbs((work)->scl.z) % SUPPORTS)

KEPT static NJS_CNK_MODEL *support_models[SUPPORTS] = {
#include "stg21/tilted_flag/support_models.inc"
};
NJS_TEXLIST *tilted_flag_texlists[SUPPORTS + 1] = {
#include "stg21/tilted_flag/tilted_flag_texlists.inc"
};
static char *support_names[SUPPORTS] = {"SHADOW", "BRIGHT"};

static void tilted_flag_destroy(struct task *task);
static void tilted_flag_step(struct task *task);
static void tilted_flag_draw(struct task *task);

void tilted_flag_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = tilted_flag_draw;
        task->step = tilted_flag_step;
        task->destructor = tilted_flag_destroy;
        FLAG_PHASE(task) = rand() * (FLAG_FRAMES * RAND_SCALE);
    }
}

static void tilted_flag_destroy(struct task *task)
{
}

static void tilted_flag_step(struct task *task)
{
    /* @hack redundant: a test of the result that returns either way, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void tilted_flag_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame = (FLAG_PHASE(task) + game_state.steps) % FLAG_FRAMES;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    /* @hack spelling: the support and the scale read through task at each use,
     * where work would do */
    njSetTexture(tilted_flag_texlists[SUPPORT(task->work)]);
    ocean_model_draw_scaled(support_models[SUPPORT(task->work)],
                            task->work->scl.x + 1.0F);
    njPopMatrixEx();
    njRotateX(NULL, CLOTH_PITCH);
    njSetTexture(FLAG_CLOTH_TEXLIST(frame));
    ocean_model_draw_scaled(ocean_flag_models[frame], task->work->scl.x + 1.0F);
    njPopMatrixEx();
}
