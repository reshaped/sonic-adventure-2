/* the tree, TREEST in City Escape's object list: a tree that faces the
 * camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "stg13/tree.h"
#include "system/task.h"
#include "stg13/tree/text.inc"

/* the trunk: a capsule of radius TRUNK_RADIUS and half length
 * TRUNK_HALF_LENGTH, centered TRUNK_RISE above pos */
#define TRUNK_RISE 80.0F
#define TRUNK_RADIUS 5.0F
#define TRUNK_HALF_LENGTH 83.0F
#define TREE_SHAPES 1

static void tree_destroy(struct task *task);
static void tree_step(struct task *task);
static void tree_draw(struct task *task);

static NJS_TEXNAME tree_texnames[3] = {
#include "stg13/tree/tree_texnames.inc"
};
static NJS_TEXLIST tree_texlist = {
#include "stg13/tree/tree_texlist.inc"
};
static Sint16 tree_polygons[199] = {
#include "stg13/tree/tree_polygons.inc"
};
static Sint32 tree_vertices[167] = {
#include "stg13/tree/tree_vertices.inc"
};
static NJS_CNK_MODEL tree_model = {
#include "stg13/tree/tree_model.inc"
};
static NJS_CNK_OBJECT tree_object = {
#include "stg13/tree/tree_object.inc"
};
static Sint16 tree_ground_polygons[39] = {
#include "stg13/tree/tree_ground_polygons.inc"
};
static Sint32 tree_ground_vertices[35] = {
#include "stg13/tree/tree_ground_vertices.inc"
};
static NJS_CNK_MODEL tree_ground_model = {
#include "stg13/tree/tree_ground_model.inc"
};
static NJS_CNK_OBJECT tree_root = {
#include "stg13/tree/tree_root.inc"
};
NJS_TEXLIST *tree_texlists[2] = {
#include "stg13/tree/tree_texlists.inc"
};
/* clang-format off */
static struct collision_shape tree_shapes[TREE_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, TRUNK_RISE, 0.0F}, TRUNK_RADIUS, TRUNK_HALF_LENGTH, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */

void tree_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = tree_draw;
    task->step = tree_step;
    task->destructor = tree_destroy;
    work->mode = 0;
    collision_init(task, tree_shapes, TREE_SHAPES, COLLISION_OBJECT);
}

static void tree_destroy(struct task *task)
{
}

static void tree_step(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    if (!placement_try_leave(owner)) {
        collision_register(owner);
    }
}

static void tree_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &tree_root;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&tree_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y - work->ang.y);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}
