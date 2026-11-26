/* the airship, AIRSHIP in Radical Highway's object list: a model whose child
 * swings about z */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg14/airship/text.inc"
#include "stg14/airship.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"

/* the set file's choice of model, 0 or 1. a choice of MODEL_1 or more draws
 * model 1 */
#define MODEL(work) ((work)->scl.x)
#define MODEL_1 1.0F
/* the child swings about z by SWING_LIMIT degrees times the sine of the work's
 * timer. each step adds SWING_SPEED to the timer */
#define SWING_SPEED(work) ((work)->ang.x)
#define SWING_LIMIT 30.0F

/* Radical Highway's two airships, each with a swinging child */

static NJS_TEXNAME airship_0_textures[3] = {
#include "stg14/airship/airship_0_textures.inc"
};

static NJS_TEXLIST airship_0_texlist = {
#include "stg14/airship/airship_0_texlist.inc"
};

static Sint16 airship_0_swing_polygons[139] = {
#include "stg14/airship/airship_0_swing_polygons.inc"
};

static Sint32 airship_0_swing_vertices[103] = {
#include "stg14/airship/airship_0_swing_vertices.inc"
};

static NJS_CNK_MODEL airship_0_swing_mesh = {
#include "stg14/airship/airship_0_swing_mesh.inc"
};

static NJS_CNK_OBJECT airship_0_swing = {
#include "stg14/airship/airship_0_swing.inc"
};

static Sint16 airship_0_polygons[779] = {
#include "stg14/airship/airship_0_polygons.inc"
};

static Sint32 airship_0_vertices[411] = {
#include "stg14/airship/airship_0_vertices.inc"
};

static NJS_CNK_MODEL airship_0_mesh = {
#include "stg14/airship/airship_0_mesh.inc"
};

static NJS_CNK_OBJECT airship_0_object = {
#include "stg14/airship/airship_0_object.inc"
};

static NJS_TEXNAME airship_1_textures[3] = {
#include "stg14/airship/airship_1_textures.inc"
};

static NJS_TEXLIST airship_1_texlist = {
#include "stg14/airship/airship_1_texlist.inc"
};

static Sint16 airship_1_swing_polygons[139] = {
#include "stg14/airship/airship_1_swing_polygons.inc"
};

static Sint32 airship_1_swing_vertices[103] = {
#include "stg14/airship/airship_1_swing_vertices.inc"
};

static NJS_CNK_MODEL airship_1_swing_mesh = {
#include "stg14/airship/airship_1_swing_mesh.inc"
};

static NJS_CNK_OBJECT airship_1_swing = {
#include "stg14/airship/airship_1_swing.inc"
};

static Sint16 airship_1_polygons[779] = {
#include "stg14/airship/airship_1_polygons.inc"
};

static Sint32 airship_1_vertices[411] = {
#include "stg14/airship/airship_1_vertices.inc"
};

static NJS_CNK_MODEL airship_1_mesh = {
#include "stg14/airship/airship_1_mesh.inc"
};

static NJS_CNK_OBJECT airship_1_object = {
#include "stg14/airship/airship_1_object.inc"
};

NJS_TEXLIST *airship_texlists[3] = {
#include "stg14/airship/airship_texlists.inc"
};

static void airship_destroy(struct task *task);
static void airship_draw(struct task *task);
static void airship_step(struct task *task);

void airship_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- SPD";
        (void)"<- ANGLE Y";
        (void)"<- MODEL(0-1)";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->step = airship_step;
    task->draw = airship_draw;
    task->destructor = airship_destroy;
}

static void airship_destroy(struct task *task)
{
}

static void airship_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    if (MODEL(work) >= MODEL_1) {
        object = &airship_1_object;
        njSetTexture(&airship_1_texlist);
    } else {
        object = &airship_0_object;
        njSetTexture(&airship_0_texlist);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateZ(NULL, object->ang[2] + (Angle)(njSin(work->timer) *
                                             (SWING_LIMIT * ANGLE_PER_DEGREE)));
    njFogDisable();
    njCnkEasyDrawModel(object->model);
    njFogEnable();
    njPopMatrixEx();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void airship_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for the register SHC
     * saves for it. the image holds none of the block's code, so its calls
     * are stand-ins */
    if (0) {
        matrix_load_parent();
        matrix_load_parent();
        matrix_load_parent();
    }
    if (!game_state.time_stopped) {
        work->timer += SWING_SPEED(work);
    }
}
