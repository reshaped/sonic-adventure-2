#include <ninja.h>

#include "shared/ocean_util.h"
#include "stg23/scenery.h"
#include "stg23/scenery_textures.h"
#include "system/port.h"
#include "system/task.h"

#define PLANT_KINDS 2
#define KIND(work) ((int)njAbs((work)->scl.z) % PLANT_KINDS)
#define SCALE(work) ((work)->scl.x + 1.0F)

KEPT static NJS_CNK_OBJECT *models[PLANT_KINDS] = {
#include "stg23/plant/models.inc"
};
/* the kinds' texture lists, then a null that ends them */
NJS_TEXLIST *base_plant_texlists[PLANT_KINDS + 1] = {
#include "stg23/plant/base_plant_texlists.inc"
};
static const char *kind_names[PLANT_KINDS] = {"SMALL", "LARGE"};

#define SCENERY_INIT base_plant_init
#include "stg23/scenery_impl.h"

static void scenery_draw(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(base_plant_texlists[KIND(task->work)]);
    ocean_model_draw_scaled(models[KIND(task->work)]->model, SCALE(task->work));
    njPopMatrixEx();
}
