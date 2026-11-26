/* the model objects, SARROW and TRBACK in City Escape's object list. each
 * draws the model of its kind, from a list of shared/model_kind.h */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/model_kind.h"
#include "stg13/model_objects.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"

/* a task's list of kinds, in task->motion. task_destroy would free motion as
 * a record, so the destructor nulls it */
#define LIST(task) ((struct model_kind_list *)(task)->motion.p)
/* the low byte of the work's ang.x picks the task's kind */
#define KIND_INDEX 0xff
#define KIND(task, work) (&LIST(task)->kinds[(work)->ang.x])
/* the work's mode: MODE_NEAR where the last step found player 0 near the
 * object, and MODE_FAR otherwise */
#define MODE_NEAR 0
#define MODE_FAR 1
/* unk_3c holds a KIND_MATRIX task's matrix of its pos and ang.y, from the
 * Shinobi heap, or null. no kind here has KIND_MATRIX */
#define MATRIX(task) ((task)->unk_3c.p)

/* a SARROW's lights are the object's child and the child's next two siblings.
 * each is lit in turn for LIGHT_STEPS steps, from the last to the child. a
 * light fades in over the first FADE_STEPS steps of its turn. the one lit
 * before it fades out over the same steps. a light draws tinted from LIGHT_OFF
 * to LIGHT_ON by how far it has faded in */
#define LIGHTS 3
#define LIGHT_STEPS 30
#define FADE_STEPS 8
#define FADE_STEP (1.0F / FADE_STEPS)
/* a light at ALPHA_MIN or below doesn't draw */
#define ALPHA_MIN 0.001F
#define LIGHT_OFF 0xff000000
#define LIGHT_ON 0xffffffff

static void model_object_start(struct task *task);
static void model_object_destroy(struct task *task);
static void model_object_step(struct task *task);
static void sarrow_draw(struct task *task);
static void trback_draw(struct task *task);

NJS_TEXLIST *model_objects_texlists[] = {&trback_texlist, NULL};
static struct model_kind sarrow_kinds[] = {
    {0, NULL, &sarrow_object, NULL, 1, sarrow_draw, NULL, NULL, 0, NULL},
};
static struct model_kind trback_kinds[] = {
    {KIND_BUILD, &trback_texlist, &trback_object, NULL, 1, trback_draw, NULL,
     NULL, 0, NULL},
};
static struct model_kind_list model_kind_lists[] = {
    {sarrow_kinds, 1},
    {trback_kinds, 1},
};
static Float dat_8c691c8c = 0.008F;
static Float dat_8c691c90 = 25.0F;
static Sint32 dat_8c691c94 = 5;
static Float dat_8c691c98 = 1.2F;
static Float dat_8c691c9c = 0.38F;
static Float dat_8c691ca0 = 0.17F;
/* the half sizes of the box, in an object's frame, in which player 0 is near
 * the object */
static NJS_VECTOR model_objects_near = {60.0F, 30.0F, 130.0F};

void sarrow_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &model_kind_lists[0];
        model_object_start(task);
    }
}

void trback_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &model_kind_lists[1];
        model_object_start(task);
    }
}

Uint8 *model_objects_build(Uint8 *buf)
{
    int i;
    int j;
    struct model_kind_list *list;
    struct model_kind *kind;
    NJS_CNK_OBJECT copy;
    Uint8 *end;

    for (i = 0;
         (Uint32)i < sizeof(model_kind_lists) / sizeof(model_kind_lists[0]);
         i++) {
        list = &model_kind_lists[i];
        for (j = 0; j < list->count; j++) {
            kind = &list->kinds[j];
            if ((kind->flags & KIND_BUILD) && !(kind->flags & KIND_COMPILED)) {
                copy = *kind->object;
                if (!(kind->flags & KIND_TREE)) {
                    copy.sibling = NULL;
                    copy.child = NULL;
                    copy.evalflags |= NJD_EVAL_BREAK;
                }
                if (kind->texlist != NULL) {
                    njSetTexture(kind->texlist);
                }
                end = buf + njCnkDirectObjectCompileSize(&copy);
                if (end < DEMO_AREA) {
                    njCnkDirectObjectCompile(&copy, buf);
                    kind->built = (NJS_DIRECT_HEAD *)buf;
                    buf = end;
                    kind->flags |= KIND_COMPILED;
                }
            }
        }
    }
    return buf;
}

static void model_object_start(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;

    task->step = model_object_step;
    task->destructor = model_object_destroy;
    work->ang.x &= KIND_INDEX;
    if (work->ang.x > LIST(task)->count - 1) {
        work->ang.x = LIST(task)->count - 1;
    }
    kind = KIND(task, work);
    if (kind->shapes != NULL) {
        collision_init(task, kind->shapes, kind->shape_count, COLLISION_OBJECT);
    }
    task->draw = kind->draw;
    MATRIX(task) = NULL;
    if (kind->flags & KIND_MATRIX) {
        MATRIX(task) = syMalloc(sizeof(NJS_MATRIX));
        if (MATRIX(task) != NULL) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njGetMatrix(MATRIX(task));
            njPopMatrixEx();
        }
    }
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
}

static void model_object_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (MATRIX(task) != NULL) {
        syFree(MATRIX(task));
        MATRIX(task) = NULL;
    }
    task->motion.p = NULL;
    task->extra.p = NULL;
    task->force.p = NULL;
    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
}

/* @hack redundant: task a register parameter, where a plain parameter would
 * do */
static void model_object_step(register struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;
    NJS_POINT3 point;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODEL(0-  )";
        (void)"<- ANGLE Y";
    }
    kind = KIND(task, work);
    if (kind->step != NULL) {
        kind->step(task, work, kind);
    } else if (work->collision != NULL) {
        collision_register(task);
    }
    work->mode = MODE_FAR;
    if (player_work[0] != NULL) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, -work->ang.y);
        njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
        njCalcPoint(NULL, &player_work[0]->pos, &point);
        njPopMatrixEx();
        if (njAbs(point.x) < model_objects_near.x &&
            njAbs(point.z) < model_objects_near.z &&
            njAbs(point.y) < model_objects_near.y) {
            work->mode = MODE_NEAR;
        }
    }
}

static void sarrow_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;
    struct model_alternate *alternate;
    NJS_CNK_OBJECT *object;
    /* @hack redundant: unused and frame_address, a local and a pointer to it
     * that nothing reads, which only size the frame */
    Uint8 unused[12];
    void *frame_address = unused;
    int active;
    int lit;
    int frame;
    int fade;
    int previous;
    Float fade_in;
    int i;
    Float alpha;

    if (work->mode != MODE_NEAR) {
        return;
    }
    kind = KIND(task, work);
    alternate = kind->alternate;
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (work->submode && alternate != NULL && alternate->object != NULL) {
        njSetTexture(alternate->texlist);
        njPushMatrixEx();
        njCnkEasyDrawModel(alternate->object->model);
    } else {
        if (kind->texlist != NULL) {
            njSetTexture(kind->texlist);
        }
        object = kind->object;
        if (kind->flags & KIND_COMPILED) {
            njDirectDrawObject(kind->built);
        } else if (kind->object->model != NULL) {
            if (kind->flags & KIND_TREE) {
                njCnkEasyDrawObject(kind->object);
            } else {
                njCnkEasyDrawModel(kind->object->model);
            }
        }
        njPushMatrixEx();
        object = object->child;
        if (object != NULL) {
            frame = game_state.steps % (LIGHTS * LIGHT_STEPS);
            /* @hack spelling: active, a copy of lit for previous, where lit
             * would do */
            lit = active = (LIGHTS - 1) - (frame / LIGHT_STEPS);
            frame = frame % LIGHT_STEPS;
            /* @hack spelling: i cleared before the fades, where the loop's
             * start would do */
            i = 0;
            fade_in = frame * FADE_STEP;
            previous = (active + 1) % LIGHTS;
            /* @hack spelling: fade, a local for FADE_STEPS, where the constant
             * in place would do */
            fade = FADE_STEPS;
            for (; i < LIGHTS; i++) {
                if (i == lit) {
                    if (frame < fade) {
                        alpha = fade_in;
                    } else {
                        alpha = 1.0F;
                    }
                } else if (i == previous) {
                    if (frame < fade) {
                        alpha = (fade - frame) * FADE_STEP;
                    } else {
                        alpha = 0.0F;
                    }
                } else {
                    alpha = 0.0F;
                }
                if (alpha > ALPHA_MIN) {
                    njTranslateEx((NJS_VECTOR *)object->pos);
                    model_draw_tinted(object->model,
                                      color_blend(LIGHT_OFF, LIGHT_ON, alpha));
                    matrix_load_parent();
                }
                object = object->sibling;
            }
        }
    }
    njPopMatrix(2);
}

static void trback_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);

    njSetTexture(kind->texlist);
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njScale(NULL, work->scl.y, work->scl.z, 1.0F);
    }
    if (kind->flags & KIND_COMPILED) {
        njDirectDrawObject(kind->built);
    } else {
        njCnkEasyDrawModel(kind->object->model);
    }
    njPopMatrixEx();
}
