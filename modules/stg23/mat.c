#include <ninja.h>

#include "shared/ocean_util.h"
#include "stg23/scenery.h"
#include "stg23/scenery_textures.h"
#include "system/task.h"
#include "system/util.h"

#define MAT_KINDS 1
#define KIND(work) ((int)njAbs((work)->scl.z) % MAT_KINDS)
#define SCALE(work) ((work)->scl.x + 1.0F)
/* the mat draws this far above its pos, clear of the floor under it */
#define ABOVE_FLOOR 0.001F

static NJS_CNK_MODEL *models[MAT_KINDS] = {&base_mat_model};
NJS_TEXLIST *base_mat_texlists[] = {&base_mat_texlist, NULL};
static const char *kind_names[MAT_KINDS] = {"GREEN"};

#define SCENERY_INIT base_mat_init
#include "stg23/scenery_impl.h"

static void scenery_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.z = SCALE(task->work);
    scale.y = 1.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + ABOVE_FLOOR, work->pos.z);
    njRotateY(NULL, work->ang.y);
    /* @hack redundant: KIND's picks from tables of one, where
     * &base_mat_texlist and &base_mat_model would do */
    njSetTexture(base_mat_texlists[KIND(task->work)]);
    ocean_model_draw_scaled_v(models[KIND(task->work)], &scale);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
