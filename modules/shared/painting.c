/* the painting, PAINTING in a stage's object list: one of three pictures */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/painting_art.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "shared/painting.h"
#include "shared/painting_textures.h"
#include "system/task.h"
#include "system/util.h"

#define PICTURES 3
#define PICTURE(work) ((int)_builtin_fabsf((work)->scl.z) % PICTURES)

static void painting_destroy(struct task *task);
static void painting_step(struct task *task);
static void painting_draw(struct task *task);

#include "shared/painting/text.inc"

static NJS_CNK_MODEL *painting_models[PICTURES] = {
#include "shared/painting/painting_models.inc"
};
NJS_TEXLIST *painting_texlists[PICTURES + 1] = {
#include "shared/painting/painting_texlists.inc"
};
static char *painting_picture_names[PICTURES] = {
#include "shared/painting/painting_picture_names.inc"
};
/* TRUE once one call of painting_build has built all three pictures */
static Bool painting_built = FALSE;
static struct model_build painting_builds[PICTURES] = {
#include "shared/painting/painting_builds.inc"
};

Uint8 *painting_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, painting_builds, PICTURES) == PICTURES) {
        painting_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void painting_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = painting_draw;
    task->step = painting_step;
    task->destructor = painting_destroy;
}

static void painting_destroy(struct task *task)
{
}

static void painting_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void painting_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    /* @hack spelling: task->work read again for scl and the picture, where
     * work would do */
    scale = task->work->scl.x + 1.0F;
    njScale(NULL, scale, scale, 1.0F);
    njSetTexture(painting_texlists[PICTURE(task->work)]);
    if (painting_built) {
        njDirectDrawObject(painting_builds[PICTURE(task->work)].built);
    } else {
        njCnkEasyDrawModel(painting_models[PICTURE(task->work)]);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
