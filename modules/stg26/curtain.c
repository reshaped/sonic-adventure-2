/* NOREN in Egg Quarters' object list: a hanging curtain, which only draws.
 * nothing collides with it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg26/curtain.h"
#include "stg26/curtain_art.h"
#include "stg26/prop_builds.h"
#include "system/task.h"

/* the frame the curtain draws while its build hasn't compiled. the build is
 * frame 0's */
#define FRAME(task) ((task)->unk_3c.l)

/* the set editor's label for ang.y, which no code reads */
static const char editor_angle_y[20] = "<- ANGLE Y (   )";

static const char curtain_panel_filename[12] = {
#include "stg26/curtain/curtain_panel_filename.inc"
};

static const char curtain_border_filename[12] = {
#include "stg26/curtain/curtain_border_filename.inc"
};

static struct curtain_frame curtain_frames[CURTAIN_FRAMES] = {
#include "stg26/curtain/curtain_frames.inc"
};

static NJS_CNK_MODEL *quarters_curtain_models[CURTAIN_FRAMES] = {
#include "stg26/curtain/quarters_curtain_models.inc"
};

NJS_TEXLIST *quarters_curtain_texlists[CURTAIN_FRAMES + 1] = {
#include "stg26/curtain/quarters_curtain_texlists.inc"
};

static void curtain_destroy(struct task *task);
static void curtain_step(struct task *task);
static void curtain_draw(struct task *task);

/* a collision box, which no code reads */
/* clang-format off */
static struct collision_shape curtain_shape = {
    0, SHAPE_BOX, 0, SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
    {0.0F, -35.0F, 0.0F}, 15.0F, 35.0F, 5.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Bool built = FALSE;
/* @hack type: curtain_build an array of one, where a single record would
 * do */
static struct model_build curtain_build[1] = {
#include "stg26/curtain/curtain_build.inc"
};

Uint8 *quarters_curtain_build(Uint8 *buffer)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buffer, curtain_build, 1) == 1) {
        built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buffer;
}

void quarters_curtain_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = curtain_draw;
        task->step = curtain_step;
        task->destructor = curtain_destroy;
        FRAME(task) = 0;
    }
}

static void curtain_destroy(struct task *task)
{
}

static void curtain_step(struct task *task)
{
    /* @hack redundant: work, a volatile local that nothing reads */
    struct task_work *volatile work = task->work;

    /* @hack redundant: the test of placement_try_leave's result, and its
     * return, where the call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void curtain_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (built) {
        njSetTexture(quarters_curtain_texlists[0]);
        njDirectDrawObject(curtain_build[0].built);
    } else {
        njSetTexture(quarters_curtain_texlists[FRAME(task)]);
        njCnkEasyDrawModel(quarters_curtain_models[FRAME(task)]);
    }
    njPopMatrixEx();
}
