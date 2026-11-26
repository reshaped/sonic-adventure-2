/* the searchlight, SEARCHLIGHT in Prison Lane's object list: a lamp whose head
 * tilts up and down */
#include <ninja.h>

#include "stg09/searchlight/text.inc"

#include "object/set.h"
#include "stg09/searchlight.h"
#include "system/task.h"

/* the head tilts about its hinge, HINGE_Z along the base's z, by up to twice
 * TILT_SCALE */
#define HINGE_Z 15.75F
#define TILT_SCALE NJM_DEG_ANG(22.5)

/* the base's node, whose child is the head and the head's sibling the
 * beam */
static NJS_TEXLIST searchlight_texlist;
static NJS_CNK_OBJECT searchlight_object;

static void searchlight_destroy(struct task *task);
static void searchlight_step(struct task *task);
static void searchlight_draw(struct task *task);

void searchlight_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = searchlight_draw;
    task->step = searchlight_step;
    task->destructor = searchlight_destroy;
}

static void searchlight_destroy(struct task *task)
{
}

static void searchlight_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    work->timer += work->ang.x;
}

static void searchlight_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *head;
    NJS_CNK_OBJECT *beam;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)SEARCHLIGHT_EDIT_ROT_SPD;
        (void)SEARCHLIGHT_EDIT_ROT_Y;
    }
    njSetTexture(&searchlight_texlist);
    object = &searchlight_object;
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    njTranslate(NULL, 0.0F, 0.0F, HINGE_Z);
    njRotateX(NULL, (njSin(work->timer) + 1.0F) * TILT_SCALE);
    head = object->child;
    njFogDisable();
    njCnkSimpleDrawModel(head->model);
    njFogEnable();
    beam = head->sibling;
    njCnkEasyDrawModel(beam->model);
    njPopMatrix(1);
}

#include "stg09/fixtures/searchlight.h"

NJS_TEXLIST *searchlight_texlists[2] = {
#include "stg09/searchlight/searchlight_texlists.inc"
};
