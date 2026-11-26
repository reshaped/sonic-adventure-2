/* the harbor's tank, TANK in Metal Harbor's object list: a solid tank of two
 * parts. the work's scl.y raises the upper one */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg10/harbor_tank.h"
#include "stg10/harbor_tank/text.inc"
#include "system/task.h"

/* a player is near the world's copy within its model's radius plus
 * REACH_MARGIN of the model's center. the world collects the copy only while
 * one is near. the same sum gives the copy's size class */
#define REACH_MARGIN 15.0F

/* a tank's record, in task->extra */
struct tank {
    /* the copy of harbor_tank_collision that the collision world holds,
     * placed where the work is */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the center of object's model, in world space */
};

/* how high the upper part stands above the base */
#define RAISE(work) ((work)->scl.y)

#define TANK(task) ((struct tank *)(task)->extra.p)

/* The harbor tank and the solid hull registered with the collision world. */

static NJS_MATERIAL harbor_tank_collision_materials[1] = {
#include "stg10/harbor_tank/harbor_tank_collision_materials.inc"
};

static Sint16 harbor_tank_collision_indices_0[42] = {
#include "stg10/harbor_tank/harbor_tank_collision_indices_0.inc"
};

static NJS_MESHSET harbor_tank_collision_meshsets[1] = {
#include "stg10/harbor_tank/harbor_tank_collision_meshsets.inc"
};

static NJS_POINT3 harbor_tank_collision_points[17] = {
#include "stg10/harbor_tank/harbor_tank_collision_points.inc"
};

static NJS_VECTOR harbor_tank_collision_normals[17] = {
#include "stg10/harbor_tank/harbor_tank_collision_normals.inc"
};

static NJS_MODEL harbor_tank_collision_model = {
#include "stg10/harbor_tank/harbor_tank_collision_model.inc"
};

static NJS_OBJECT harbor_tank_collision = {
#include "stg10/harbor_tank/harbor_tank_collision.inc"
};

static NJS_TEXNAME harbor_tank_textures[7] = {
#include "stg10/harbor_tank/harbor_tank_textures.inc"
};

static NJS_TEXLIST harbor_tank_texlist = {
#include "stg10/harbor_tank/harbor_tank_texlist.inc"
};

static Sint16 harbor_tank_object_node_1_polygons[93] = {
#include "stg10/harbor_tank/harbor_tank_object_node_1_polygons.inc"
};

static Sint32 harbor_tank_object_node_1_vertices[39] = {
#include "stg10/harbor_tank/harbor_tank_object_node_1_vertices.inc"
};

static NJS_CNK_MODEL harbor_tank_object_node_1_mesh = {
#include "stg10/harbor_tank/harbor_tank_object_node_1_mesh.inc"
};

static NJS_CNK_OBJECT harbor_tank_object_node_1 = {
#include "stg10/harbor_tank/harbor_tank_object_node_1.inc"
};

static Sint16 harbor_tank_polygons[505] = {
#include "stg10/harbor_tank/harbor_tank_polygons.inc"
};

static Sint32 harbor_tank_vertices[491] = {
#include "stg10/harbor_tank/harbor_tank_vertices.inc"
};

static NJS_CNK_MODEL harbor_tank_mesh = {
#include "stg10/harbor_tank/harbor_tank_mesh.inc"
};

static NJS_CNK_OBJECT harbor_tank_object[1] = {
#include "stg10/harbor_tank/harbor_tank_object.inc"
};

NJS_TEXLIST *harbor_tank_texlists[2] = {
#include "stg10/harbor_tank/harbor_tank_texlists.inc"
};

static void harbor_tank_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- OFS Y";
    }
    njSetTexture(&harbor_tank_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(harbor_tank_object->model);
    njTranslate(NULL, 0.0F, RAISE(work), 0.0F);
    njCnkEasyDrawModel(harbor_tank_object->child->model);
    njPopMatrix(1);
}

static void harbor_tank_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        if (TANK(task)->object.model != NULL) {
            world_remove(task, &TANK(task)->object);
        }
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void harbor_tank_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the else after the return, where the test after it
     * would do */
    if (placement_try_leave(task)) {
        return;
    } else if (task->extra.p != NULL && TANK(task)->object.model != NULL) {
        if (player_near(&TANK(task)->center,
                        TANK(task)->object.model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

void harbor_tank_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    task->extra.p = syCalloc(1, sizeof(struct tank));
    if (task->extra.p == NULL) {
        return;
    }
    object = &TANK(task)->object;
    *object = harbor_tank_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    /* @hack spelling: the angles set y, z, x, where x, y, z would do */
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    /* @hack spelling: the record reached through task->extra again, where
     * object would do */
    njTranslateV(NULL, (NJS_VECTOR *)TANK(task)->object.pos);
    njRotateY(NULL, TANK(task)->object.ang[1]);
    njCalcPoint(NULL, &TANK(task)->object.model->center, &TANK(task)->center);
    njPopMatrix(1);
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = harbor_tank_draw;
    task->step = harbor_tank_step;
    task->destructor = harbor_tank_destroy;
}
