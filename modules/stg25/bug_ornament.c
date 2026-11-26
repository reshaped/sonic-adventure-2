/* the bug, BUG in Death Chamber's object list: a figure of a bug, of six
 * kinds. the players bump into it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg25/bug_ornament.h"
#include "stg25/chamber_util.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"

enum chamber_bug_ornament_kind {
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
#define INTANGIBLE(work) ((int)(work)->scl.y & 1)

#define BUG_SHAPES 1

#include "stg25/bug_ornament/text.inc"

static NJS_TEXNAME chamber_bug_ornament_red_texture_names[1] = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_texture_names.inc"
};

static NJS_TEXLIST chamber_bug_ornament_red_texlist = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_texlist.inc"
};

static Sint16 chamber_bug_ornament_red_polygons[439] = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_polygons.inc"
};

static Sint32 chamber_bug_ornament_red_vertices[343] = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_vertices.inc"
};

static NJS_CNK_MODEL chamber_bug_ornament_red_model = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_model.inc"
};

static NJS_CNK_OBJECT chamber_bug_ornament_red_object = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_object.inc"
};

static NJS_TEXNAME chamber_bug_ornament_green_texture_names[1] = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_texture_names.inc"
};

static NJS_TEXLIST chamber_bug_ornament_green_texlist = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_texlist.inc"
};

static Sint16 chamber_bug_ornament_green_polygons[439] = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_polygons.inc"
};

static Sint32 chamber_bug_ornament_green_vertices[343] = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_vertices.inc"
};

static NJS_CNK_MODEL chamber_bug_ornament_green_model = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_model.inc"
};

static NJS_CNK_OBJECT chamber_bug_ornament_green_object = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_object.inc"
};

static NJS_TEXNAME chamber_bug_ornament_blue_texture_names[1] = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_texture_names.inc"
};

static NJS_TEXLIST chamber_bug_ornament_blue_texlist = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_texlist.inc"
};

static Sint16 chamber_bug_ornament_blue_polygons[439] = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_polygons.inc"
};

static Sint32 chamber_bug_ornament_blue_vertices[343] = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_vertices.inc"
};

static NJS_CNK_MODEL chamber_bug_ornament_blue_model = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_model.inc"
};

static NJS_CNK_OBJECT chamber_bug_ornament_blue_object = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_object.inc"
};

static NJS_TEXNAME chamber_bug_ornament_red_dark_texture_names[1] = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_dark_texture_names.inc"
};

static NJS_TEXLIST chamber_bug_ornament_red_dark_texlist = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_dark_texlist.inc"
};

static Sint16 chamber_bug_ornament_red_dark_polygons[439] = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_dark_polygons.inc"
};

static Sint32 chamber_bug_ornament_red_dark_vertices[343] = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_dark_vertices.inc"
};

static NJS_CNK_MODEL chamber_bug_ornament_red_dark_model = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_dark_model.inc"
};

static NJS_CNK_OBJECT chamber_bug_ornament_red_dark_object = {
#include "stg25/bug_ornament/chamber_bug_ornament_red_dark_object.inc"
};

static NJS_TEXNAME chamber_bug_ornament_green_dark_texture_names[1] = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_dark_texture_names.inc"
};

static NJS_TEXLIST chamber_bug_ornament_green_dark_texlist = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_dark_texlist.inc"
};

static Sint16 chamber_bug_ornament_green_dark_polygons[439] = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_dark_polygons.inc"
};

static Sint32 chamber_bug_ornament_green_dark_vertices[343] = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_dark_vertices.inc"
};

static NJS_CNK_MODEL chamber_bug_ornament_green_dark_model = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_dark_model.inc"
};

static NJS_CNK_OBJECT chamber_bug_ornament_green_dark_object = {
#include "stg25/bug_ornament/chamber_bug_ornament_green_dark_object.inc"
};

static NJS_TEXNAME chamber_bug_ornament_blue_dark_texture_names[1] = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_dark_texture_names.inc"
};

static NJS_TEXLIST chamber_bug_ornament_blue_dark_texlist = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_dark_texlist.inc"
};

static Sint16 chamber_bug_ornament_blue_dark_polygons[439] = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_dark_polygons.inc"
};

static Sint32 chamber_bug_ornament_blue_dark_vertices[343] = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_dark_vertices.inc"
};

static NJS_CNK_MODEL chamber_bug_ornament_blue_dark_model = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_dark_model.inc"
};

static NJS_CNK_OBJECT chamber_bug_ornament_blue_dark_object = {
#include "stg25/bug_ornament/chamber_bug_ornament_blue_dark_object.inc"
};

static void chamber_bug_ornament_destroy(struct task *task);
static void chamber_bug_ornament_step(struct task *task);
static void chamber_bug_ornament_draw(struct task *task);

KEPT static NJS_CNK_MODEL *chamber_bug_ornament_models[KINDS] = {
#include "stg25/bug_ornament/chamber_bug_ornament_models.inc"
};
NJS_TEXLIST *chamber_bug_ornament_texlists[KINDS + 1] = {
#include "stg25/bug_ornament/chamber_bug_ornament_texlists.inc"
};
static char *chamber_bug_ornament_kind_names[KINDS] = {
#include "stg25/bug_ornament/chamber_bug_ornament_kind_names.inc"
};
/* the bug's shape at a scale of 1: a cylinder lying along z */
/* clang-format off */
static struct collision_shape chamber_bug_ornament_shapes[BUG_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000 | SHAPE_IGNORE_UNK_9,
     {0.0F, 0.0F, 3.0F}, 10.0F, 3.0F, 0.0F, 0, {QUARTER_TURN, 0, 0}},
};
/* clang-format on */
/* TRUE once one call of chamber_bug_ornament_build has compiled all six
 * kinds' models */
static Bool chamber_bug_ornament_built = FALSE;
static struct model_build chamber_bug_ornament_builds[KINDS] = {
#include "stg25/bug_ornament/chamber_bug_ornament_builds.inc"
};
static Float chamber_bug_ornament_radius = 12.5F;

Uint8 *chamber_bug_ornament_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_bug_ornament_builds, KINDS) ==
        KINDS) {
        chamber_bug_ornament_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void chamber_bug_ornament_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = chamber_bug_ornament_draw;
        task->step = chamber_bug_ornament_step;
        task->destructor = chamber_bug_ornament_destroy;
        collision_init(task, chamber_bug_ornament_shapes, BUG_SHAPES,
                       COLLISION_OBJECT);
        /* @hack spelling: task->work read again for scl, where work would
         * do */
        work->collision->shapes->radius *= SCALE(task->work);
        work->collision->shapes->size_y *= SCALE(task->work);
        collision_fit_radius(task);
        /* @bug logic: the record's radius is 12.5 whatever scl.x is, below
         * the shape's reach of about 13.44 at a scale of 1. the call at
         * 0x8c50a5da fits the radius to the scaled shape, and the store at
         * 0x8c50a5e6 replaces it. the checks test two records only while
         * their tasks lie less than their radii's sum apart, so a touch
         * past that goes unseen */
        work->collision->radius = chamber_bug_ornament_radius;
    }
}

static void chamber_bug_ornament_destroy(struct task *task)
{
}

static void chamber_bug_ornament_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        if (!INTANGIBLE(task->work)) {
            collision_register(task);
        }
    }
}

static void chamber_bug_ornament_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for scl, where work would do */
    njSetTexture(chamber_bug_ornament_texlists[KIND(task->work)]);
    if (chamber_bug_ornament_built) {
        chamber_direct_draw_scaled(
            chamber_bug_ornament_builds[KIND(task->work)].built,
            SCALE(task->work));
    } else {
        chamber_model_draw_scaled(chamber_bug_ornament_models[KIND(task->work)],
                                  SCALE(task->work));
    }
    njPopMatrixEx();
}
