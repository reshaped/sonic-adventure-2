/* the fallen tree, TOBOKU and TOBOKU_FAR in a stage's object list: a log
 * that the players collide with */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/fallen_tree.h"
#include "system/task.h"
#include "system/util.h"

/* TOBOKU_FAR's range, squared as placement_try_leave_by takes it */
#define FAR_RANGE (810.0F * 810.0F)
/* the work's submode: the range the tree leaves at */
enum fallen_tree_range { RANGE_PLACEMENT, RANGE_FAR };
/* the tree collides only while a player lies within the model's radius plus
 * REACH_MARGIN of its center */
#define REACH_MARGIN 10.0F

#include "shared/fallen_tree/text.inc"

/* the halves of the log's polygon list. stg04/fallen_tree.c compiles this
 * source again over White Jungle's log, and defines its list's first */
#ifndef FALLEN_TREE_POLYGON_HALVES
#define FALLEN_TREE_POLYGON_HALVES 203
#endif

/* the log's textures and model */
#define TEXTURES 3
static NJS_TEXNAME fallen_tree_textures[TEXTURES] = {
#include "shared/fallen_tree/fallen_tree_textures.inc"
};
NJS_TEXLIST fallen_tree_texlist = {fallen_tree_textures, TEXTURES};

static Sint16 fallen_tree_polygons[FALLEN_TREE_POLYGON_HALVES] = {
#include "shared/fallen_tree/fallen_tree_polygons.inc"
};

static Sint32 fallen_tree_vertices[115] = {
#include "shared/fallen_tree/fallen_tree_vertices.inc"
};

static NJS_CNK_MODEL fallen_tree_model = {
#include "shared/fallen_tree/fallen_tree_model.inc"
};

static NJS_CNK_OBJECT fallen_tree_object = {
#include "shared/fallen_tree/fallen_tree_object.inc"
};

/* the log's collision shape, a basic model */
static NJS_MATERIAL fallen_tree_collision_materials[1] = {
#include "shared/fallen_tree/fallen_tree_collision_materials.inc"
};

static Sint16 fallen_tree_collision_indices_0[12] = {
#include "shared/fallen_tree/fallen_tree_collision_indices_0.inc"
};

static NJS_MESHSET fallen_tree_collision_meshsets[1] = {
#include "shared/fallen_tree/fallen_tree_collision_meshsets.inc"
};

static NJS_POINT3 fallen_tree_collision_points[6] = {
#include "shared/fallen_tree/fallen_tree_collision_points.inc"
};

static NJS_VECTOR fallen_tree_collision_normals[6] = {
#include "shared/fallen_tree/fallen_tree_collision_normals.inc"
};

static NJS_MODEL fallen_tree_collision_model = {
#include "shared/fallen_tree/fallen_tree_collision_model.inc"
};

static NJS_OBJECT fallen_tree_collision = {
#include "shared/fallen_tree/fallen_tree_collision.inc"
};

static void fallen_tree_start(struct task *task);
static void fallen_tree_destroy(struct task *task);
static void fallen_tree_step(struct task *task);
static void fallen_tree_draw(struct task *task);

/* the tree's model, and the collision object the start copies */
/* @hack type: the model and the collision object as one array of void
 * pointers, where a struct would do */
static void *fallen_tree_parts[2] = {&fallen_tree_model,
                                     &fallen_tree_collision};
#define PART_MODEL 0
#define PART_COLLISION 1

void fallen_tree_init(struct task *task)
{
    /* @hack spelling: work, a local for task->work read before the test,
     * where task->work read after it would do */
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants ahead of the texture names */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    if (placement_try_leave(task)) {
        return;
    }
    work->submode = RANGE_PLACEMENT;
    fallen_tree_start(task);
}

void fallen_tree_far_init(struct task *task)
{
    if (placement_try_leave_by(task, FAR_RANGE)) {
        return;
    }
    task->work->submode = RANGE_FAR;
    fallen_tree_start(task);
}

static void fallen_tree_start(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;

    task->draw = fallen_tree_draw;
    task->destructor = fallen_tree_destroy;
    task->step = fallen_tree_step;
    /* @hack redundant: a jump to the statement that follows it */
    goto allocate;
allocate:
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies the collision object through null where the pool
     * has none to lend. fallen_tree_destroy tests task->motion for that null
     * (at 0x126 of the object's code). the copy at 0x7a and fallen_tree_step's
     * read of the object's model at 0x21e use it untested */
#ifdef NONMATCHING
    /* this side repairs the tag above: without an object, the tree ends on
     * its next step, and its placement makes it again while in range */
    if (object == NULL) {
        task_kill(task);
        return;
    }
#endif
    *object = *(NJS_OBJECT *)fallen_tree_parts[PART_COLLISION];
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
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
}

static void fallen_tree_destroy(struct task *task)
{
    if (task->motion.p) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

static void fallen_tree_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 center;

    if (work->submode != RANGE_PLACEMENT
            ? placement_try_leave_by(task, FAR_RANGE)
            : placement_try_leave(task)) {
        return;
    }
    object = task->motion.p;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &center);
    njPopMatrix(1);
    if (player_near(&center, object->model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void fallen_tree_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&fallen_tree_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(fallen_tree_parts[PART_MODEL]);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
