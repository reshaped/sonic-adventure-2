/* White Jungle's solid scenery, JO, JO2, JO_FAR and JO2_FAR in the stage's
 * object list. it is a model of one of three shapes, which stands in the
 * collision world */
#include "system/port.h"
#include "stg04/solid_scenery/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/hover_lift.h"
#include "stg04/jungle_prop.h"
#include "stg04/solid_scenery.h"
#include "system/task.h"
#include "system/util.h"

/* the scenery's shapes. a shape past the last takes the last */
#define SHAPES 3
#define LAST_SHAPE (SHAPES - 1)

/* the far scenery's range, JO_FAR's and JO2_FAR's, squared as
 * placement_try_leave_by takes it */
#define FAR_RANGE (1450.0F * 1450.0F)

/* JO's and JO2's scenery takes its size class from its model's radius plus
 * CLASS_MARGIN. the far scenery takes it from the radius alone */
#define CLASS_MARGIN 2.0F
/* the scenery's object collides while a player lies within REACH_MARGIN of
 * the model's sphere. the far scenery's takes FAR_REACH_MARGIN instead */
#define REACH_MARGIN 15.0F
#define FAR_REACH_MARGIN 5.0F

#define SHAPE(work) ((work)->tick)
/* the model's center in the world, which the first step writes over scl */
#define CENTER(work) ((work)->scl)
/* the copy of the shape's collision object, which world_pool_alloc lends */
#define COPY(task) ((NJS_OBJECT *)(task)->motion.p)

static NJS_MATERIAL white_jungle_scenery_collision_0_materials[1] = {
#include "stg04/solid_scenery/white_jungle_scenery_collision_0_materials.inc"
};

static Sint16 white_jungle_scenery_collision_0_indices_0[21] = {
#include "stg04/solid_scenery/white_jungle_scenery_collision_0_indices_0.inc"
};

static NJS_MESHSET white_jungle_scenery_collision_0_meshsets[1] = {
#include "stg04/solid_scenery/white_jungle_scenery_collision_0_meshsets.inc"
};

static NJS_POINT3 white_jungle_scenery_collision_0_points[9] = {
#include "stg04/solid_scenery/white_jungle_scenery_collision_0_points.inc"
};

static NJS_VECTOR white_jungle_scenery_collision_0_normals[9] = {
#include "stg04/solid_scenery/white_jungle_scenery_collision_0_normals.inc"
};

static NJS_MODEL white_jungle_scenery_collision_0_model = {
#include "stg04/solid_scenery/white_jungle_scenery_collision_0_model.inc"
};

static NJS_OBJECT white_jungle_scenery_collision_0 = {
#include "stg04/solid_scenery/white_jungle_scenery_collision_0.inc"
};

static NJS_TEXNAME white_jungle_scenery_textures_1[2] = {
#include "stg04/solid_scenery/white_jungle_scenery_textures_1.inc"
};

static NJS_TEXLIST white_jungle_scenery_texlist_1 = {
#include "stg04/solid_scenery/white_jungle_scenery_texlist_1.inc"
};

static Sint16 white_jungle_scenery_polygons_1[187] = {
#include "stg04/solid_scenery/white_jungle_scenery_polygons_1.inc"
};

static Sint32 white_jungle_scenery_vertices_1[99] = {
#include "stg04/solid_scenery/white_jungle_scenery_vertices_1.inc"
};

static NJS_CNK_MODEL white_jungle_scenery_model_1 = {
#include "stg04/solid_scenery/white_jungle_scenery_model_1.inc"
};

static NJS_CNK_OBJECT white_jungle_scenery_object_1 = {
#include "stg04/solid_scenery/white_jungle_scenery_object_1.inc"
};

static NJS_TEXNAME white_jungle_scenery_textures_2[2] = {
#include "stg04/solid_scenery/white_jungle_scenery_textures_2.inc"
};

static NJS_TEXLIST white_jungle_scenery_texlist_2 = {
#include "stg04/solid_scenery/white_jungle_scenery_texlist_2.inc"
};

static Sint16 white_jungle_scenery_polygons_2[187] = {
#include "stg04/solid_scenery/white_jungle_scenery_polygons_2.inc"
};

static Sint32 white_jungle_scenery_vertices_2[99] = {
#include "stg04/solid_scenery/white_jungle_scenery_vertices_2.inc"
};

static NJS_CNK_MODEL white_jungle_scenery_model_2 = {
#include "stg04/solid_scenery/white_jungle_scenery_model_2.inc"
};

static NJS_CNK_OBJECT white_jungle_scenery_object_2 = {
#include "stg04/solid_scenery/white_jungle_scenery_object_2.inc"
};

NJS_TEXLIST *white_jungle_scenery_texlists[4] = {
#include "stg04/solid_scenery/white_jungle_scenery_texlists.inc"
};
/* each shape's model and collision object */
KEPT static NJS_CNK_MODEL *models[SHAPES] = {
#include "stg04/solid_scenery/models.inc"
};
KEPT static NJS_OBJECT *collisions[SHAPES] = {
#include "stg04/solid_scenery/collisions.inc"
};
/* whether each shape draws under DRAW_CONTROL_SHADOW */
KEPT static Bool shadowed[SHAPES] = {
#include "stg04/solid_scenery/shadowed.inc"
};

static void solid_scenery_destroy(struct task *task);
static void solid_scenery_step(struct task *task);
static void solid_scenery_draw_shadowed(struct task *task);
static void solid_scenery_draw(struct task *task);
static void solid_scenery_far_step(struct task *task);

void white_jungle_scenery_0_init(struct task *task)
{
    task->work->scl.x = 0.0F;
    white_jungle_scenery_init(task);
}

void white_jungle_scenery_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODEL SHAPE";
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    if (placement_try_leave(task)) {
        return;
    }
    SHAPE(work) = (int)work->scl.x;
    /* @hack redundant: the test of the shape below 0, which never holds for
     * the unsigned tick */
    if (SHAPE(work) >= SHAPES || SHAPE(work) < 0) {
        SHAPE(work) = LAST_SHAPE;
    }
    if (shadowed[SHAPE(work)]) {
        task->draw = solid_scenery_draw_shadowed;
    } else {
        task->draw = solid_scenery_draw;
    }
    task->step = solid_scenery_step;
    task->destructor = solid_scenery_destroy;
    /* @hack redundant: a jump to the statement after it */
    goto alloc;
alloc:
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies the shape's collision object through null where
     * world_pool_alloc has none to lend, at 0x8c503174. solid_scenery_step
     * reads the copy untested at 0x8c50333a, though solid_scenery_destroy
     * tests it at 0x8c50326e */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the scenery, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = *collisions[SHAPE(work)];
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[0] = work->ang.x;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    radius = object->model->r + CLASS_MARGIN;
    flags = WORLD_SOLID;
    if (radius <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &CENTER(work));
    njPopMatrix(1);
}

static void solid_scenery_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

static void solid_scenery_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: the else after the return */
    if (placement_try_leave(task)) {
        return;
    } else if (player_near(&CENTER(work),
                           COPY(task)->model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void solid_scenery_draw_shadowed(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(white_jungle_scenery_texlists[SHAPE(work)]);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(models[SHAPE(work)]);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void solid_scenery_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(white_jungle_scenery_texlists[SHAPE(work)]);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(models[SHAPE(work)]);
    njPopMatrix(1);
}

void white_jungle_scenery_0_far_init(struct task *task)
{
    task->work->scl.x = 0.0F;
    white_jungle_scenery_far_init(task);
}

void white_jungle_scenery_far_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;

    if (placement_try_leave_by(task, FAR_RANGE)) {
        return;
    }
    SHAPE(work) = (int)work->scl.x;
    /* @hack redundant: the test of the shape below 0, which never holds for
     * the unsigned tick */
    if (SHAPE(work) >= SHAPES || SHAPE(work) < 0) {
        SHAPE(work) = LAST_SHAPE;
    }
    if (shadowed[SHAPE(work)]) {
        task->draw = solid_scenery_draw_shadowed;
    } else {
        task->draw = solid_scenery_draw;
    }
    task->step = solid_scenery_far_step;
    task->destructor = solid_scenery_destroy;
    /* @hack redundant: a jump to the statement after it */
    goto alloc;
alloc:
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies through null as white_jungle_scenery_init does, at
     * 0x8c503522. solid_scenery_far_step reads the copy untested at
     * 0x8c503638, though solid_scenery_destroy tests it at 0x8c50326e */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the scenery, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = *collisions[SHAPE(work)];
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[0] = work->ang.x;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    if (object->model->r <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (object->model->r > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (object->model->r > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &CENTER(work));
    njPopMatrix(1);
}

static void solid_scenery_far_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: the else after the return */
    if (placement_try_leave_by(task, FAR_RANGE)) {
        return;
    } else if (player_near(&CENTER(work),
                           COPY(task)->model->r + FAR_REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}
