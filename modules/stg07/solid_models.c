/* Aquatic Mine's solid models: MIFRAME, MILIFT, MIBAR, MIWOOD and MIWOODY in
 * the stage's object list, and miwood and miwoody. each is a model that
 * stands in the collision world */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg07/solid_models.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg07/solid_models/text.inc"
#include "stg07/scenery/solid_models.h"

/* a model's record, in task->motion. object comes first: world_add and
 * world_remove take task->motion as the copy */
struct solid_model {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_POINT3 center; /* the collision model's center, in the world */
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model; /* the model the draw draws, or null for none */
};

#define SOLID(task) ((struct solid_model *)(task)->motion.p)

/* the copy collides while a player lies within the model's radius plus
 * REACH_MARGIN of its center. the same sum gives its size class */
#define REACH_MARGIN 15.0F

NJS_TEXLIST *solid_models_texlists[6] = {
#include "stg07/solid_models/solid_models_texlists.inc"
};
/* a cylinder's shape, its axis turned a quarter turn about z, which none of
 * the models takes */
static struct collision_shape dat_8c5639f4 = {
#include "stg07/solid_models/dat_8c5639f4.inc"
};

static void solid_model_start(struct task *task, NJS_TEXLIST *texlist,
                              NJS_CNK_MODEL *model, NJS_OBJECT *collision,
                              Bool shadowed, Uint32 flags,
                              struct collision_shape *shapes, int count);
static void solid_model_destroy(struct task *task);
static void solid_model_step(struct task *task);
static void solid_model_draw(struct task *task);
static void solid_model_draw_shadowed(struct task *task);

void miframe_init(struct task *task)
{
    solid_model_start(task, miframe_texlist, miframe_model, &miframe_collision,
                      TRUE, 0, NULL, 0);
}

void milift_init(struct task *task)
{
    solid_model_start(task, milift_texlist, milift_model, &milift_collision,
                      TRUE, 0, NULL, 0);
}

void mibar_init(struct task *task)
{
    solid_model_start(task, mibar_texlist, mibar_model, &mibar_collision, TRUE,
                      0, NULL, 0);
}

void miwood_init(struct task *task)
{
    solid_model_start(task, miwood_texlist, miwood_model, &miwood_collision,
                      FALSE, 0, NULL, 0);
}

void miwoody_init(struct task *task)
{
    solid_model_start(task, miwoody_texlist, miwoody_model, &miwoody_collision,
                      FALSE, 0, NULL, 0);
}

/* set task up to draw model with texlist, under DRAW_CONTROL_SHADOW where
 * shadowed is TRUE. a null model draws nothing. a copy of collision stands in
 * the collision world under flags and WORLD_SOLID. a non-null shapes gives
 * task a collision record of count shapes too */
static void solid_model_start(struct task *task, NJS_TEXLIST *texlist,
                              NJS_CNK_MODEL *model, NJS_OBJECT *collision,
                              Bool shadowed, Uint32 flags,
                              struct collision_shape *shapes, int count)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 world_flags;

    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct solid_model));
    if (task->motion.p == NULL) {
        return;
    }
    SOLID(task)->model = model;
    SOLID(task)->texlist = texlist;
    if (SOLID(task)->model != NULL) {
        if (shadowed) {
            task->draw = solid_model_draw_shadowed;
        } else {
            task->draw = solid_model_draw;
        }
    }
    task->step = solid_model_step;
    task->destructor = solid_model_destroy;
    object = task->motion.p;
    *object = *collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, (NJS_POINT3 *)object->pos, (NJS_POINT3 *)object->pos);
    njPopMatrix(1);
    world_flags = flags | WORLD_SOLID;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            world_flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            world_flags |= WORLD_SIZE_CLASS(2);
        } else {
            world_flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(world_flags, task, object);
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &SOLID(task)->center);
    njPopMatrix(1);
}

static void solid_model_destroy(struct task *task)
{
    if (SOLID(task)->object.model != NULL) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void solid_model_step(struct task *task)
{
    struct task_work *work = task->work;
    struct solid_model *solid;
    NJS_OBJECT *object;

    /* @hack redundant: the else after the return */
    if (!work->submode && placement_try_leave(task)) {
        return;
    } else if (SOLID(task)->object.model != NULL) {
        /* @hack spelling: solid set inside the expression that sets object,
         * where a statement of its own would do */
        object = &(solid = task->motion.p)->object;
        if (player_near(&solid->center, object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
}

static void solid_model_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)SOLID_MODEL_ANGLE_X_LABEL;
        (void)SOLID_MODEL_ANGLE_Y_LABEL;
        (void)SOLID_MODEL_ANGLE_Z_LABEL;
    }
    njSetTexture(SOLID(task)->texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(SOLID(task)->model);
    njPopMatrix(1);
}

static void solid_model_draw_shadowed(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(SOLID(task)->texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(SOLID(task)->model);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
