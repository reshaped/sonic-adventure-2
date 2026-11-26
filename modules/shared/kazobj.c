/* KAZOBJ's kinds and callbacks, as Eternal Engine builds them. under
 * COLONY_KAZOBJ, which stg27/kazobj.c defines, they are Lost Colony's */
#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/kazobj.h"
#include "shared/model_kind.h"
#include "system/task.h"

static void kazobj_start(struct task *task);
static void kazobj_destroy(struct task *task);
static void kazobj_step(struct task *task);
static void kazobj_draw(struct task *task);

#ifdef COLONY_KAZOBJ
static void kazobj_draw_unfogged(struct task *task);
#else
#include "stg24/scenery_art.h"
#endif

/* clang-format off */
static struct collision_shape box = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {-15.0F, 75.0F, 0.0F}, 15.0F, 15.0F, 40.0F, 0, {0, 0, 0},
};
/* clang-format on */

/* the kind of kazobj_model_##index, drawn with its texture list. shapes is null
 * or one shape */
/* clang-format off */
#define MODEL(index, flags, shapes, draw)                                      \
    {(flags), &kazobj_texlist_##index, &kazobj_model_##index, (shapes), 1,     \
     (draw), NULL, NULL, 0, NULL}
/* clang-format on */

static struct model_kind kinds[] = {
    MODEL(0, KIND_BUILD, NULL, kazobj_draw),
#ifdef COLONY_KAZOBJ
    MODEL(1, KIND_BUILD, &box, kazobj_draw),
    MODEL(2, KIND_BUILD, NULL, kazobj_draw),
    MODEL(3, KIND_BUILD | KIND_NO_FOG, NULL, kazobj_draw_unfogged),
    MODEL(4, KIND_BUILD | KIND_NO_FOG, NULL, kazobj_draw_unfogged),
    MODEL(5, KIND_BUILD | KIND_NO_FOG, NULL, kazobj_draw_unfogged),
#else
    MODEL(1, KIND_BUILD, NULL, kazobj_draw),
    MODEL(2, KIND_BUILD, NULL, kazobj_draw),
    MODEL(3, KIND_BUILD, NULL, kazobj_draw),
    /* Eternal Engine's kind 4, which has no model or texture list */
    {0, NULL, NULL, NULL, 1, NULL, NULL, NULL, 0, NULL},
    MODEL(5, KIND_BUILD, NULL, kazobj_draw),
    MODEL(6, KIND_BUILD, &box, kazobj_draw),
    MODEL(7, KIND_BUILD, NULL, kazobj_draw),
#endif
};

static struct model_kind_list kazobj_kinds = {
    kinds,
    sizeof(kinds) / sizeof(kinds[0]),
};

#define KAZOBJ_INIT kazobj_init
#define KAZOBJ_BUILD kazobj_build
#define KAZOBJ_BUILD_TREE KIND_TREE
#include "shared/kazobj_task.h"

/* @hack spelling: a macro of a bare block, where an inline function taking
 * work and kind would do */
#define DRAW_MODEL(work, kind)                                                 \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njTranslateEx(&(work)->pos);                                           \
        njRotateY(NULL, (work)->ang.y);                                        \
        if ((kind)->flags & KIND_COMPILED) {                                   \
            njDirectDrawObject((kind)->built);                                 \
        } else {                                                               \
            njCnkEasyDrawModel((kind)->object->model);                         \
        }                                                                      \
        njPopMatrixEx();                                                       \
    }

static void kazobj_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);

    njSetTexture(kind->texlist);
    DRAW_MODEL(work, kind);
}

#ifdef COLONY_KAZOBJ
static void kazobj_draw_unfogged(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);

    njSetTexture(kind->texlist);
    njFogDisable();
    DRAW_MODEL(work, kind);
    njFogEnable();
}
#endif
