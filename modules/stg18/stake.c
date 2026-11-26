/* the stake, STAKE in Dry Lagoon's object list: a post of two kinds */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg18/lagoon_util.h"
#include "stg18/stake.h"
#include "system/port.h"
#include "stg18/stake/text.inc"
#include "system/task.h"

enum stake_kind { KIND_BRIGHT, KIND_DARK, KINDS };
#define KIND(work) ((int)_builtin_fabsf((work)->scl.z) % KINDS)
#define SCALE(work) ((work)->scl.x + 1.0F)

#define STAKE_SHAPES 1

/* Stake. */

static NJS_TEXNAME stake_bright_texlist_names[1] = {
#include "stg18/stake/stake_bright_texlist_names.inc"
};

static NJS_TEXLIST stake_bright_texlist = {
#include "stg18/stake/stake_bright_texlist.inc"
};

static Sint16 stake_bright_polygons[123] = {
#include "stg18/stake/stake_bright_polygons.inc"
};

static Sint32 stake_bright_vertices[55] = {
#include "stg18/stake/stake_bright_vertices.inc"
};

static NJS_CNK_MODEL stake_bright_model = {
#include "stg18/stake/stake_bright_model.inc"
};

static NJS_CNK_OBJECT stake_bright_object = {
#include "stg18/stake/stake_bright_object.inc"
};

static NJS_TEXNAME stake_dark_texlist_names[1] = {
#include "stg18/stake/stake_dark_texlist_names.inc"
};

static NJS_TEXLIST stake_dark_texlist = {
#include "stg18/stake/stake_dark_texlist.inc"
};

static Sint16 stake_dark_polygons[123] = {
#include "stg18/stake/stake_dark_polygons.inc"
};

static Sint32 stake_dark_vertices[55] = {
#include "stg18/stake/stake_dark_vertices.inc"
};

static NJS_CNK_MODEL stake_dark_model = {
#include "stg18/stake/stake_dark_model.inc"
};

static NJS_CNK_OBJECT stake_dark_object = {
#include "stg18/stake/stake_dark_object.inc"
};

KEPT static NJS_CNK_MODEL *stake_models[KINDS] = {
#include "stg18/stake/stake_models.inc"
};
NJS_TEXLIST *stake_texlists[3] = {
#include "stg18/stake/stake_texlists.inc"
};
static char *stake_kind_names[KINDS] = {"BRIGHT", "DARK  "};
/* clang-format off */
static struct collision_shape stake_shapes[STAKE_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 7.25F, 0.0F}, 2.5F, 7.25F,
     0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Bool stake_built = FALSE;
static struct model_build stake_builds[KINDS] = {
    {MODEL_BUILD_ALONE, &stake_bright_texlist, &stake_bright_object, NULL},
    {MODEL_BUILD_ALONE, &stake_dark_texlist, &stake_dark_object, NULL},
};

static void stake_destroy(struct task *task);
static void stake_step(struct task *task);
static void stake_draw(struct task *task);

Uint8 *stake_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, stake_builds, KINDS) == KINDS) {
        stake_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void stake_init(struct task *task)
{
    /* @hack type: the copy as an array of one shape, where a shape would
     * do */
    struct collision_shape shapes[STAKE_SHAPES];

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's order
     * would set the step first */
    task->draw = stake_draw;
    task->step = stake_step;
    task->destructor = stake_destroy;
    shapes[0] = stake_shapes[0];
    shapes[0].radius *= SCALE(task->work);
    shapes[0].size_y *= SCALE(task->work);
    shapes[0].center.y *= SCALE(task->work);
    collision_init(task, shapes, STAKE_SHAPES, COLLISION_OBJECT);
}

static void stake_destroy(struct task *task)
{
}

/* @hack redundant: task a register parameter, where a plain parameter would
 * do */
static void stake_step(register struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
}

static void stake_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for scl, where work would do */
    njSetTexture(stake_texlists[KIND(task->work)]);
    if (stake_built) {
        lagoon_direct_draw_scaled(stake_builds[KIND(task->work)].built,
                                  SCALE(task->work));
    } else {
        lagoon_model_draw_scaled(stake_models[KIND(task->work)],
                                 SCALE(task->work));
    }
    njPopMatrixEx();
}
