/* the steps of MECHA, the machinery of Death Chamber and Egg Quarters, which
 * stg25/mecha.c and stg26/machinery.c include. the includer defines models,
 * MACHINERY_INIT and MACHINERY_TEXLISTS. Egg Quarters also names its build
 * function MACHINERY_BUILD. the build makes both models once, and the draw
 * uses the builds once both are made. Death Chamber draws the chunk models */
#ifndef SHARED_MACHINERY_PROP_H
#define SHARED_MACHINERY_PROP_H
#include <ninja.h>

#include "object/set.h"
#include "shared/machinery_assets.h"
#include "system/task.h"

#ifdef MACHINERY_BUILD
#include "shared/common_object.h"
#include "shared/object_builds.h"
#endif

/* the kind scl.z picks: the whole part of |scl.z|, modulo MACHINERY_KINDS */
#define MACHINERY_KIND(work)                                                   \
    ((int)_builtin_fabsf((work)->scl.z) % MACHINERY_KINDS)

static void machinery_destroy(struct task *task);
static void machinery_step(struct task *task);
static void machinery_draw(struct task *task);

/* the set editor's names of the kinds, in their order */
static char *kind_names[MACHINERY_KINDS] = {"CURVE   ", "STRAIGHT"};

#ifdef MACHINERY_BUILD
static Bool built = FALSE;
static struct model_build builds[MACHINERY_KINDS] = {
    {MODEL_BUILD_ALONE, &machinery_curve_texlist, &machinery_curve_object,
     NULL},
    {MODEL_BUILD_ALONE, &machinery_straight_texlist, &machinery_straight_object,
     NULL},
};

Uint8 *MACHINERY_BUILD(Uint8 *buffer)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buffer, builds, MACHINERY_KINDS) ==
        MACHINERY_KINDS) {
        built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buffer;
}
#endif

void MACHINERY_INIT(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = machinery_draw;
        task->step = machinery_step;
        task->destructor = machinery_destroy;
    }
}

static void machinery_destroy(struct task *task)
{
}

static void machinery_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave and the return, where
     * the call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void machinery_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the kind read through task->work at each use, where
     * work would do */
    njSetTexture(MACHINERY_TEXLISTS[MACHINERY_KIND(task->work)]);
#ifdef MACHINERY_BUILD
    if (built) {
        njDirectDrawObject(builds[MACHINERY_KIND(task->work)].built);
    } else
#endif
    {
        njCnkEasyDrawModel(models[MACHINERY_KIND(task->work)]);
    }
    njPopMatrixEx();
}

#endif
