/* the round shadow, SHADOW in Death Chamber's object list: a model textured as
 * a round shadow. it comes in two sizes */
#include <sg_xpt.h>
#include <ninja.h>

#include "object/set.h"
#include "stg25/round_shadow.h"
#include "system/task.h"
#include "stg25/round_shadow/text.inc"

enum chamber_round_shadow_size { SIZE_LARGE, SIZE_SMALL, SIZES };
#define SIZE(work) ((int)_builtin_fabsf((work)->scl.z) % SIZES)
/* the shadow's scale along x, and along z */
#define SCALE_X(work) ((work)->scl.x + 1.0F)
#define SCALE_Z(work) ((work)->scl.y + 1.0F)

static NJS_TEXNAME chamber_round_shadow_large_texture_names[1] = {
#include "stg25/round_shadow/chamber_round_shadow_large_texture_names.inc"
};

static NJS_TEXLIST chamber_round_shadow_large_texlist = {
#include "stg25/round_shadow/chamber_round_shadow_large_texlist.inc"
};

static Sint16 chamber_round_shadow_large_polygons[25] = {
#include "stg25/round_shadow/chamber_round_shadow_large_polygons.inc"
};

static Sint32 chamber_round_shadow_large_vertices[19] = {
#include "stg25/round_shadow/chamber_round_shadow_large_vertices.inc"
};

static NJS_CNK_MODEL chamber_round_shadow_large_model = {
#include "stg25/round_shadow/chamber_round_shadow_large_model.inc"
};

static NJS_CNK_OBJECT chamber_round_shadow_large_object = {
#include "stg25/round_shadow/chamber_round_shadow_large_object.inc"
};

static NJS_TEXNAME chamber_round_shadow_small_texture_names[1] = {
#include "stg25/round_shadow/chamber_round_shadow_small_texture_names.inc"
};

NJS_TEXLIST chamber_round_shadow_small_texlist = {
#include "stg25/round_shadow/chamber_round_shadow_small_texlist.inc"
};

static Sint16 chamber_round_shadow_small_polygons[25] = {
#include "stg25/round_shadow/chamber_round_shadow_small_polygons.inc"
};

static Sint32 chamber_round_shadow_small_vertices[19] = {
#include "stg25/round_shadow/chamber_round_shadow_small_vertices.inc"
};

NJS_CNK_MODEL chamber_round_shadow_small_model = {
#include "stg25/round_shadow/chamber_round_shadow_small_model.inc"
};

static NJS_CNK_OBJECT chamber_round_shadow_small_object = {
#include "stg25/round_shadow/chamber_round_shadow_small_object.inc"
};

static void chamber_round_shadow_destroy(struct task *task);
static void chamber_round_shadow_step(struct task *task);
static void chamber_round_shadow_draw(struct task *task);

static NJS_CNK_MODEL *chamber_round_shadow_models[SIZES] = {
#include "stg25/round_shadow/chamber_round_shadow_models.inc"
};
NJS_TEXLIST *chamber_round_shadow_texlists[3] = {
#include "stg25/round_shadow/chamber_round_shadow_texlists.inc"
};
static char *chamber_round_shadow_size_names[SIZES] = {
#include "stg25/round_shadow/chamber_round_shadow_size_names.inc"
};

void chamber_round_shadow_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = chamber_round_shadow_draw;
        task->step = chamber_round_shadow_step;
        task->destructor = chamber_round_shadow_destroy;
    }
}

static void chamber_round_shadow_destroy(struct task *task)
{
}

static void chamber_round_shadow_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, and its
     * return, where the call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void chamber_round_shadow_draw(struct task *task)
{
    /* @hack spelling: task->work read again for scl and the size, where
     * work would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, SCALE_X(task->work), 1.0F, SCALE_Z(task->work));
    njSetTexture(chamber_round_shadow_texlists[SIZE(task->work)]);
    njCnkEasyDrawModel(chamber_round_shadow_models[SIZE(task->work)]);
    njPopMatrixEx();
}
