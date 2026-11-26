#include "stg22/fixture_models.h"
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/model_kind.h"
#include "stg22/rail_marker.h"
#include "stg22/prop_textures.h"
#include "system/task.h"

static void kazobj_start(struct task *task);
static void kazobj_destroy(struct task *task);
static void kazobj_step(struct task *task);
static void marker_draw(struct task *task);

/* draws that no kind names: one turned by ang.y alone, and one also scaled
 * by scl.y and scl.z. each is external, since SHC leaves out a static
 * function its source never names */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void gadget_marker_draw_yaw(struct task *task);
void gadget_marker_draw_scaled(struct task *task);
/* NOLINTEND(misc-use-internal-linkage) */

NJS_TEXLIST *gadget_rail_marker_texlists[] = {&gadget_marker_texlist, NULL};
static struct model_kind kinds[] = {
    {KIND_BUILD | KIND_TREE, &gadget_marker_texlist, &gadget_marker_object,
     NULL, 0, marker_draw, NULL, NULL, 0, NULL},
};
static struct model_kind_list kazobj_kinds = {kinds, 1};

void gadget_rail_marker_art(NJS_TEXLIST **textures, NJS_CNK_OBJECT **object,
                            NJS_DIRECT_HEAD **build)
{
    *textures = kinds[0].texlist;
    *object = kinds[0].object;
    *build = kinds[0].built;
}

#define KAZOBJ_INIT gadget_rail_marker_init
#define KAZOBJ_BUILD gadget_rail_marker_build
/* this stage's build takes a kind's whole tree only where the kind has
 * KIND_MATRIX. shared/kazobj.c's stages test KIND_TREE instead. the marker's
 * kind lacks KIND_MATRIX, but the test changes nothing for it: its object has
 * no child or sibling, and already sets NJD_EVAL_BREAK */
#define KAZOBJ_BUILD_TREE KIND_MATRIX
#define KAZOBJ_SINGLE_SHAPES
#include "shared/kazobj_task.h"

#define DRAW_MARKER(kind)                                                      \
    do {                                                                       \
        if ((kind)->flags & KIND_COMPILED) {                                   \
            njDirectDrawObject((kind)->built);                                 \
        } else {                                                               \
            njCnkEasyDrawModel((kind)->object->model);                         \
        }                                                                      \
    } while (0)

void gadget_marker_draw_yaw(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);

    njSetTexture(kind->texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    DRAW_MARKER(kind);
    njPopMatrixEx();
}

static void marker_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);

    njSetTexture(kind->texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    DRAW_MARKER(kind);
    njPopMatrixEx();
}

void gadget_marker_draw_scaled(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);

    /* @hack redundant: the set editor's labels, as statements that do
     * nothing */
    (void)"<- MODEL(0-  )";
    (void)"<- ANGLE Y";
    (void)"<- ANGLE X";
    (void)"<- ANGLE Z";
    njSetTexture(kind->texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, work->scl.y, work->scl.z, 1.0F);
    DRAW_MARKER(kind);
    njPopMatrixEx();
}
