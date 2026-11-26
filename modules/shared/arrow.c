/* the arrow, ARROW in Final Chase's and Cosmic Wall's object lists. its two
 * parts blink in turn between a bright color and a dim one */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/arrow.h"
#include "shared/common_object.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"

/* the blink, in steps: blink_level's period, rest and fade */
#define BLINK_PERIOD 50
#define BLINK_REST 25
#define BLINK_FADE 8

/* the set editor's label for scl.x, which picks the second model at 1 or
 * more. a copy of the arrow may name it otherwise */
#ifndef ARROW_MODEL_LABEL
#define ARROW_MODEL_LABEL "<- MODEL"
#endif

NJS_TEXLIST *arrow_texlists[] = {&arrow_texlist, &arrow_2_texlist, NULL};
/* the blink's two colors. while the first part is bright, the second is
 * dim */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff303030;

static void arrow_destroy(struct task *task);
static void arrow_step(struct task *task);
static void arrow_draw(struct task *task);

void arrow_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = arrow_draw;
        task->step = arrow_step;
        task->destructor = arrow_destroy;
    }
}

static void arrow_destroy(struct task *task)
{
}

static void arrow_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings and its
     * local. SHC still places the strings, the set editor's labels, in this
     * source's constants. the local, which the two draws keep live, makes the
     * step save a register, as the image's does. none of the block's code is
     * in the image */
    if (0) {
        NJS_CNK_MODEL *model = arrow_object.model;

        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)ARROW_MODEL_LABEL;
        njCnkEasyDrawModel(model);
        njCnkEasyDrawModel(model);
    }
}

static void arrow_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *part;
    Float level;

    if (work->scl.x >= 1.0F) {
        njSetTexture(&arrow_2_texlist);
        object = &arrow_2_object;
    } else {
        njSetTexture(&arrow_texlist);
        object = &arrow_object;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, XYZ_TURN);
    njPushMatrixEx();
    njCnkEasyDrawModel(object->model);
    part = object->child;
    level = blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE);
    njTranslateEx((NJS_VECTOR *)part->pos);
    model_draw_tinted(part->model, color_blend(bright_color, dim_color, level));
    matrix_load_parent();
    part = part->sibling;
    if (part != NULL) {
        njTranslateEx((NJS_VECTOR *)part->pos);
        model_draw_tinted(part->model,
                          color_blend(dim_color, bright_color, level));
    }
    njPopMatrix(2);
}
