/* the solid scenery, JO and JO_FAR in a stage's object list. it is a model
 * of one of three shapes, which stands in the collision world */
#include "system/port.h"
#include "shared/solid_scenery/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/jungle_prop.h"
#include "shared/solid_scenery.h"
#include "system/task.h"
#include "system/util.h"

/* the scenery's shapes. a shape past the last takes the last */
#define SHAPES 3
#define LAST_SHAPE (SHAPES - 1)

/* JO_FAR's range, squared as placement_try_leave_by takes it */
#define FAR_RANGE (1450.0F * 1450.0F)

/* JO's scenery takes its size class from its model's radius plus
 * CLASS_MARGIN. JO_FAR's takes it from the radius alone */
#define CLASS_MARGIN 2.0F
/* the scenery's object collides while a player lies within REACH_MARGIN of
 * the model's sphere */
#define REACH_MARGIN 5.0F

#define SHAPE(work) ((work)->tick)
/* the model's center in the world, which the init writes over scl */
#define CENTER(work) ((work)->scl)

static NJS_TEXNAME solid_scenery_textures_0[1] = {
#include "shared/solid_scenery/solid_scenery_textures_0.inc"
};

static NJS_TEXLIST solid_scenery_texlist_0 = {
#include "shared/solid_scenery/solid_scenery_texlist_0.inc"
};

static Sint16 solid_scenery_polygons_0[121] = {
#include "shared/solid_scenery/solid_scenery_polygons_0.inc"
};

static Sint32 solid_scenery_vertices_0[67] = {
#include "shared/solid_scenery/solid_scenery_vertices_0.inc"
};

static NJS_CNK_MODEL solid_scenery_model_0 = {
#include "shared/solid_scenery/solid_scenery_model_0.inc"
};

static NJS_CNK_OBJECT solid_scenery_object_0 = {
#include "shared/solid_scenery/solid_scenery_object_0.inc"
};

static NJS_TEXNAME solid_scenery_textures_1[1] = {
#include "shared/solid_scenery/solid_scenery_textures_1.inc"
};

static NJS_TEXLIST solid_scenery_texlist_1 = {
#include "shared/solid_scenery/solid_scenery_texlist_1.inc"
};

static Sint16 solid_scenery_polygons_1[219] = {
#include "shared/solid_scenery/solid_scenery_polygons_1.inc"
};

static Sint32 solid_scenery_vertices_1[111] = {
#include "shared/solid_scenery/solid_scenery_vertices_1.inc"
};

static NJS_CNK_MODEL solid_scenery_model_1 = {
#include "shared/solid_scenery/solid_scenery_model_1.inc"
};

static NJS_CNK_OBJECT solid_scenery_object_1 = {
#include "shared/solid_scenery/solid_scenery_object_1.inc"
};

static NJS_MATERIAL solid_scenery_collision_0_materials[1] = {
#include "shared/solid_scenery/solid_scenery_collision_0_materials.inc"
};

static Sint16 solid_scenery_collision_0_indices_0[45] = {
#include "shared/solid_scenery/solid_scenery_collision_0_indices_0.inc"
};

static NJS_MESHSET solid_scenery_collision_0_meshsets[1] = {
#include "shared/solid_scenery/solid_scenery_collision_0_meshsets.inc"
};

static NJS_POINT3 solid_scenery_collision_0_points[13] = {
#include "shared/solid_scenery/solid_scenery_collision_0_points.inc"
};

static NJS_VECTOR solid_scenery_collision_0_normals[13] = {
#include "shared/solid_scenery/solid_scenery_collision_0_normals.inc"
};

static NJS_MODEL solid_scenery_collision_0_model = {
#include "shared/solid_scenery/solid_scenery_collision_0_model.inc"
};

static NJS_OBJECT solid_scenery_collision_0 = {
#include "shared/solid_scenery/solid_scenery_collision_0.inc"
};

static NJS_MATERIAL solid_scenery_collision_1_materials[1] = {
#include "shared/solid_scenery/solid_scenery_collision_1_materials.inc"
};

static Sint16 solid_scenery_collision_1_indices_0[36] = {
#include "shared/solid_scenery/solid_scenery_collision_1_indices_0.inc"
};

static NJS_MESHSET solid_scenery_collision_1_meshsets[1] = {
#include "shared/solid_scenery/solid_scenery_collision_1_meshsets.inc"
};

static NJS_POINT3 solid_scenery_collision_1_points[17] = {
#include "shared/solid_scenery/solid_scenery_collision_1_points.inc"
};

static NJS_VECTOR solid_scenery_collision_1_normals[17] = {
#include "shared/solid_scenery/solid_scenery_collision_1_normals.inc"
};

static NJS_MODEL solid_scenery_collision_1_model = {
#include "shared/solid_scenery/solid_scenery_collision_1_model.inc"
};

static NJS_OBJECT solid_scenery_collision_1 = {
#include "shared/solid_scenery/solid_scenery_collision_1.inc"
};

static NJS_MATERIAL solid_scenery_collision_2_materials[1] = {
#include "shared/solid_scenery/solid_scenery_collision_2_materials.inc"
};

static Sint16 solid_scenery_collision_2_indices_0[21] = {
#include "shared/solid_scenery/solid_scenery_collision_2_indices_0.inc"
};

static NJS_MESHSET solid_scenery_collision_2_meshsets[1] = {
#include "shared/solid_scenery/solid_scenery_collision_2_meshsets.inc"
};

static NJS_POINT3 solid_scenery_collision_2_points[9] = {
#include "shared/solid_scenery/solid_scenery_collision_2_points.inc"
};

static NJS_VECTOR solid_scenery_collision_2_normals[9] = {
#include "shared/solid_scenery/solid_scenery_collision_2_normals.inc"
};

static NJS_MODEL solid_scenery_collision_2_model = {
#include "shared/solid_scenery/solid_scenery_collision_2_model.inc"
};

static NJS_OBJECT solid_scenery_collision_2 = {
#include "shared/solid_scenery/solid_scenery_collision_2.inc"
};

NJS_TEXLIST *solid_scenery_texlists[4] = {
#include "shared/solid_scenery/solid_scenery_texlists.inc"
};
/* each shape's model and collision object */
KEPT static NJS_CNK_MODEL *models[SHAPES] = {
#include "shared/solid_scenery/models.inc"
};
KEPT static NJS_OBJECT *collisions[SHAPES] = {
#include "shared/solid_scenery/collisions.inc"
};
/* whether each shape draws under DRAW_CONTROL_SHADOW */
KEPT static Sint32 shadowed[SHAPES] = {
#include "shared/solid_scenery/shadowed.inc"
};

static void solid_scenery_destroy(struct task *task);
static void solid_scenery_step(struct task *task);
static void solid_scenery_draw_shadowed(struct task *task);
static void solid_scenery_draw(struct task *task);
static void solid_scenery_far_step(struct task *task);

void solid_scenery_init(struct task *task)
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
    /* @hack spelling: a jump to the statement after it, where falling through
     * would do */
    goto alloc;
alloc:
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies the shape's collision object through null where
     * world_pool_alloc has none to lend, though solid_scenery_destroy tests
     * task->motion for null. solid_scenery_step reads through it untested:
     * 0x6c, 0x166 and 0x23a of the object's code */
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
                           ((NJS_OBJECT *)task->motion.p)->model->r +
                               REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void solid_scenery_draw_shadowed(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(solid_scenery_texlists[SHAPE(work)]);
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

    njSetTexture(solid_scenery_texlists[SHAPE(work)]);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(models[SHAPE(work)]);
    njPopMatrix(1);
}

void solid_scenery_far_init(struct task *task)
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
    /* @hack spelling: a jump to the statement after it, where falling through
     * would do */
    goto alloc;
alloc:
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies the shape's collision object through null where
     * world_pool_alloc has none to lend, though solid_scenery_destroy tests
     * task->motion for null. solid_scenery_far_step reads through it
     * untested: 0x422, 0x166 and 0x538 of the object's code */
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
                           ((NJS_OBJECT *)task->motion.p)->model->r +
                               REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}
