/* a plain tree: TREESTNB in City Escape's object list, and TREEST in the Big
 * Foot stage's. City Escape's own TREEST also draws a quad on the ground under
 * its tree. a plain tree's model has a child that turns about y to face the
 * camera */
#include <ninja.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "shared/plain_tree.h"
#include "system/task.h"

NJS_TEXLIST *plain_tree_texlists[] = {&plain_tree_texlist, NULL};

/* a capsule for the trunk */
/* clang-format off */
static struct collision_shape trunk_capsule = {
    0, SHAPE_CAPSULE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
    0, {0.0F, 80.0F, 0.0F}, 5.0F, 83.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */

static void plain_tree_destroy(struct task *task);
static void plain_tree_step(struct task *task);
static void plain_tree_draw(struct task *task);

void plain_tree_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = plain_tree_draw;
        task->step = plain_tree_step;
        task->destructor = plain_tree_destroy;
        work->mode = 0;
    }
}

static void plain_tree_destroy(struct task *task)
{
}

static void plain_tree_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which leaves its string in
     * the object */
    if (0) {
        (void)"<- ANGLE Y";
    }
}

static void plain_tree_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &plain_tree_object;

    njSetTexture(&plain_tree_texlist);
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
