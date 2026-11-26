/* the scorpion, SCORPION in Death Chamber's object list: a figure of a
 * scorpion, of six kinds */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg25/chamber_util.h"
#include "stg25/scorpion.h"
#include "system/angle.h"
#include "system/task.h"
#include "stg25/scorpion/text.inc"

enum chamber_scorpion_kind {
    KIND_RED,
    KIND_GREEN,
    KIND_BLUE,
    KIND_RED_DARK,
    KIND_GREEN_DARK,
    KIND_BLUE_DARK,
    KINDS
};
#define KIND(work) ((int)_builtin_fabsf((work)->scl.z) % KINDS)
#define SCALE(work) ((work)->scl.x + 1.0F)
#define TURNED(work) ((int)(work)->scl.y & 1)

static NJS_TEXNAME chamber_scorpion_red_texture_names[2] = {
#include "stg25/scorpion/chamber_scorpion_red_texture_names.inc"
};

static NJS_TEXLIST chamber_scorpion_red_texlist = {
#include "stg25/scorpion/chamber_scorpion_red_texlist.inc"
};

static Sint16 chamber_scorpion_red_polygons[107] = {
#include "stg25/scorpion/chamber_scorpion_red_polygons.inc"
};

static Sint32 chamber_scorpion_red_vertices[67] = {
#include "stg25/scorpion/chamber_scorpion_red_vertices.inc"
};

static NJS_CNK_MODEL chamber_scorpion_red_model = {
#include "stg25/scorpion/chamber_scorpion_red_model.inc"
};

static NJS_CNK_OBJECT chamber_scorpion_red_object = {
#include "stg25/scorpion/chamber_scorpion_red_object.inc"
};

static NJS_TEXNAME chamber_scorpion_green_texture_names[2] = {
#include "stg25/scorpion/chamber_scorpion_green_texture_names.inc"
};

static NJS_TEXLIST chamber_scorpion_green_texlist = {
#include "stg25/scorpion/chamber_scorpion_green_texlist.inc"
};

static Sint16 chamber_scorpion_green_polygons[107] = {
#include "stg25/scorpion/chamber_scorpion_green_polygons.inc"
};

static Sint32 chamber_scorpion_green_vertices[67] = {
#include "stg25/scorpion/chamber_scorpion_green_vertices.inc"
};

static NJS_CNK_MODEL chamber_scorpion_green_model = {
#include "stg25/scorpion/chamber_scorpion_green_model.inc"
};

static NJS_CNK_OBJECT chamber_scorpion_green_object = {
#include "stg25/scorpion/chamber_scorpion_green_object.inc"
};

static NJS_TEXNAME chamber_scorpion_blue_texture_names[2] = {
#include "stg25/scorpion/chamber_scorpion_blue_texture_names.inc"
};

static NJS_TEXLIST chamber_scorpion_blue_texlist = {
#include "stg25/scorpion/chamber_scorpion_blue_texlist.inc"
};

static Sint16 chamber_scorpion_blue_polygons[107] = {
#include "stg25/scorpion/chamber_scorpion_blue_polygons.inc"
};

static Sint32 chamber_scorpion_blue_vertices[67] = {
#include "stg25/scorpion/chamber_scorpion_blue_vertices.inc"
};

static NJS_CNK_MODEL chamber_scorpion_blue_model = {
#include "stg25/scorpion/chamber_scorpion_blue_model.inc"
};

static NJS_CNK_OBJECT chamber_scorpion_blue_object = {
#include "stg25/scorpion/chamber_scorpion_blue_object.inc"
};

static NJS_TEXNAME chamber_scorpion_red_dark_texture_names[2] = {
#include "stg25/scorpion/chamber_scorpion_red_dark_texture_names.inc"
};

static NJS_TEXLIST chamber_scorpion_red_dark_texlist = {
#include "stg25/scorpion/chamber_scorpion_red_dark_texlist.inc"
};

static Sint16 chamber_scorpion_red_dark_polygons[107] = {
#include "stg25/scorpion/chamber_scorpion_red_dark_polygons.inc"
};

static Sint32 chamber_scorpion_red_dark_vertices[67] = {
#include "stg25/scorpion/chamber_scorpion_red_dark_vertices.inc"
};

static NJS_CNK_MODEL chamber_scorpion_red_dark_model = {
#include "stg25/scorpion/chamber_scorpion_red_dark_model.inc"
};

static NJS_CNK_OBJECT chamber_scorpion_red_dark_object = {
#include "stg25/scorpion/chamber_scorpion_red_dark_object.inc"
};

static NJS_TEXNAME chamber_scorpion_green_dark_texture_names[2] = {
#include "stg25/scorpion/chamber_scorpion_green_dark_texture_names.inc"
};

static NJS_TEXLIST chamber_scorpion_green_dark_texlist = {
#include "stg25/scorpion/chamber_scorpion_green_dark_texlist.inc"
};

static Sint16 chamber_scorpion_green_dark_polygons[107] = {
#include "stg25/scorpion/chamber_scorpion_green_dark_polygons.inc"
};

static Sint32 chamber_scorpion_green_dark_vertices[67] = {
#include "stg25/scorpion/chamber_scorpion_green_dark_vertices.inc"
};

static NJS_CNK_MODEL chamber_scorpion_green_dark_model = {
#include "stg25/scorpion/chamber_scorpion_green_dark_model.inc"
};

static NJS_CNK_OBJECT chamber_scorpion_green_dark_object = {
#include "stg25/scorpion/chamber_scorpion_green_dark_object.inc"
};

static NJS_TEXNAME chamber_scorpion_blue_dark_texture_names[2] = {
#include "stg25/scorpion/chamber_scorpion_blue_dark_texture_names.inc"
};

static NJS_TEXLIST chamber_scorpion_blue_dark_texlist = {
#include "stg25/scorpion/chamber_scorpion_blue_dark_texlist.inc"
};

static Sint16 chamber_scorpion_blue_dark_polygons[107] = {
#include "stg25/scorpion/chamber_scorpion_blue_dark_polygons.inc"
};

static Sint32 chamber_scorpion_blue_dark_vertices[67] = {
#include "stg25/scorpion/chamber_scorpion_blue_dark_vertices.inc"
};

static NJS_CNK_MODEL chamber_scorpion_blue_dark_model = {
#include "stg25/scorpion/chamber_scorpion_blue_dark_model.inc"
};

static NJS_CNK_OBJECT chamber_scorpion_blue_dark_object = {
#include "stg25/scorpion/chamber_scorpion_blue_dark_object.inc"
};

static void chamber_scorpion_destroy(struct task *task);
static void chamber_scorpion_step(struct task *task);
static void chamber_scorpion_draw(struct task *task);

static NJS_CNK_MODEL *chamber_scorpion_models[KINDS] = {
#include "stg25/scorpion/chamber_scorpion_models.inc"
};
NJS_TEXLIST *chamber_scorpion_texlists[KINDS + 1] = {
#include "stg25/scorpion/chamber_scorpion_texlists.inc"
};
static char *chamber_scorpion_kind_names[KINDS] = {
#include "stg25/scorpion/chamber_scorpion_kind_names.inc"
};
/* TRUE once one call of chamber_scorpion_build has compiled all six kinds'
 * models */
static Bool chamber_scorpion_built = FALSE;
static struct model_build chamber_scorpion_builds[KINDS] = {
#include "stg25/scorpion/chamber_scorpion_builds.inc"
};

Uint8 *chamber_scorpion_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_scorpion_builds, KINDS) == KINDS) {
        chamber_scorpion_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void chamber_scorpion_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = chamber_scorpion_draw;
        task->step = chamber_scorpion_step;
        task->destructor = chamber_scorpion_destroy;
    }
}

static void chamber_scorpion_destroy(struct task *task)
{
}

static void chamber_scorpion_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, and its
     * return, where the call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void chamber_scorpion_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    /* @hack spelling: task->work read again for scl, where work would do */
    scale.x = scale.y = SCALE(task->work);
    scale.z = 1.0F;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (TURNED(task->work)) {
        njRotateX(NULL, -QUARTER_TURN);
    }
    njSetTexture(chamber_scorpion_texlists[KIND(task->work)]);
    if (chamber_scorpion_built) {
        chamber_direct_draw_scaled_v(
            chamber_scorpion_builds[KIND(task->work)].built, &scale);
    } else {
        chamber_model_draw_scaled_v(chamber_scorpion_models[KIND(task->work)],
                                    &scale);
    }
    njPopMatrixEx();
}
