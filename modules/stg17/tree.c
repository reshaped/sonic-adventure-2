/* the tree, TREEST in Mission Street's object list: a tree that faces the
 * camera */
#include <ninja.h>

#include "stg17/tree/text.inc"

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "stg17/tree.h"
#include "system/task.h"

/* The tree owns one capsule centered above the placement. */
#define TREE_SHAPES 1

#include "stg17/tree_art.h"

static void street_tree_destroy(struct task *task);
static void street_tree_step(struct task *task);
static void street_tree_draw(struct task *task);

NJS_TEXLIST *street_tree_texlists[2] = {
#include "stg17/tree/street_tree_texlists.inc"
};
static struct collision_shape street_tree_shapes[TREE_SHAPES] = {
#include "stg17/tree/street_tree_shapes.inc"
};

void street_tree_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = street_tree_draw;
    task->step = street_tree_step;
    task->destructor = street_tree_destroy;
    work->mode = 0;
    collision_init(task, street_tree_shapes, TREE_SHAPES, COLLISION_OBJECT);
}

static void street_tree_destroy(struct task *task)
{
}

static void street_tree_step(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    if (!placement_try_leave(owner)) {
        collision_register(owner);
    }
}

static void street_tree_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &street_tree_root;

    /* @hack redundant: a block no path reaches, kept for its string, which SHC
     * still places in this source's constants */
    if (0) {
        (void)TREE_EDITOR_ANGLE_Y;
    }
    njSetTexture(&street_tree_texlist);
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
