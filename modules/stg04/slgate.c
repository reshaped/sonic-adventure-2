/* SLGATE in White Jungle's object list: a model the players collide with and
 * the camera passes through */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg04/slgate.h"
#include "stg04/slgate/text.inc"
#include "system/task.h"

/* the copy of the collision object collides while a player lies within
 * REACH_MARGIN of the model's sphere */
#define REACH_MARGIN 5.0F

/* The visible gate and its independent player collision mesh. */
static NJS_TEXNAME model_textures[2] = {
#include "stg04/slgate/model_textures.inc"
};

NJS_TEXLIST slgate_texlist = {
#include "stg04/slgate/slgate_texlist.inc"
};

static Sint16 gate_polygons[461] = {
#include "stg04/slgate/gate_polygons.inc"
};

static Sint32 gate_vertices[459] = {
#include "stg04/slgate/gate_vertices.inc"
};

static NJS_CNK_MODEL slgate_model = {
#include "stg04/slgate/slgate_model.inc"
};

static NJS_CNK_OBJECT gate_object = {
#include "stg04/slgate/gate_object.inc"
};

static NJS_MATERIAL gate_collision_material = {
#include "stg04/slgate/gate_collision_material.inc"
};

static Sint16 gate_collision_indices[43] = {
#include "stg04/slgate/gate_collision_indices.inc"
};

static NJS_MESHSET gate_collision_meshset = {
#include "stg04/slgate/gate_collision_meshset.inc"
};

static NJS_POINT3 gate_collision_points[16] = {
#include "stg04/slgate/gate_collision_points.inc"
};

static NJS_VECTOR gate_collision_normals[16] = {
#include "stg04/slgate/gate_collision_normals.inc"
};

static NJS_MODEL gate_collision_model = {
#include "stg04/slgate/gate_collision_model.inc"
};

static NJS_OBJECT slgate_collision = {
#include "stg04/slgate/slgate_collision.inc"
};

static void slgate_destroy(struct task *task);
static void slgate_step(struct task *task);
static void slgate_draw(struct task *task);

/* @hack type: the model and the shape as one array of void pointers, where a
 * struct would do */
static void *slgate_parts[2] = {
#include "stg04/slgate/slgate_parts.inc"
};
#define PART_MODEL 0
#define PART_COLLISION 1

void slgate_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)SLGATE_EDITOR_ANGLE_Y;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = slgate_draw;
    task->step = slgate_step;
    task->destructor = slgate_destroy;
    /* @hack redundant: a jump to the statement that follows it */
    goto allocate;
allocate:
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend, at 0x8c50088e. slgate_step reads the
     * copy untested at 0x8c50098e, though slgate_destroy tests it at
     * 0x8c50093a */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the gate, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = *(NJS_OBJECT *)slgate_parts[PART_COLLISION];
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | LAND_FAR;
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
}

static void slgate_destroy(struct task *task)
{
    if (task->motion.p) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

static void slgate_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 center;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->motion.p;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &object->model->center, &center);
    njPopMatrix(1);
    if (player_near(&center, object->model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void slgate_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&slgate_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(slgate_parts[PART_MODEL]);
    njPopMatrix(1);
}
