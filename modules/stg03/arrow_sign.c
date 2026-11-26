/* the arrow signs, YAJI and YAJI2 in the stage's object list. each is an
 * arrow's model, turned and scaled. neither puts anything in the collision
 * world */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg03/arrow_sign.h"
#include "stg03/arrow_sign/text.inc"
#include "system/task.h"
#include "system/util.h"

/* Arrow signs, including their original unused transform nodes. */
static NJS_TEXNAME arrow_sign_textures[2] = {
#include "stg03/arrow_sign/arrow_sign_textures.inc"
};

static NJS_TEXLIST arrow_sign_texlist = {
#include "stg03/arrow_sign/arrow_sign_texlist.inc"
};

static Sint16 arrow_sign_polygons[75] = {
#include "stg03/arrow_sign/arrow_sign_polygons.inc"
};

static Sint32 arrow_sign_vertices[51] = {
#include "stg03/arrow_sign/arrow_sign_vertices.inc"
};

static NJS_CNK_MODEL arrow_sign_model = {
#include "stg03/arrow_sign/arrow_sign_model.inc"
};

static NJS_CNK_OBJECT arrow_sign_object = {
#include "stg03/arrow_sign/arrow_sign_object.inc"
};

static NJS_TEXNAME arrow_sign_2_textures[2] = {
#include "stg03/arrow_sign/arrow_sign_2_textures.inc"
};

static NJS_TEXLIST arrow_sign_2_texlist = {
#include "stg03/arrow_sign/arrow_sign_2_texlist.inc"
};

static Sint16 arrow_sign_2_polygons[75] = {
#include "stg03/arrow_sign/arrow_sign_2_polygons.inc"
};

static Sint32 arrow_sign_2_vertices[51] = {
#include "stg03/arrow_sign/arrow_sign_2_vertices.inc"
};

static NJS_CNK_MODEL arrow_sign_2_model = {
#include "stg03/arrow_sign/arrow_sign_2_model.inc"
};

static NJS_CNK_OBJECT arrow_sign_2_object = {
#include "stg03/arrow_sign/arrow_sign_2_object.inc"
};

NJS_TEXLIST *arrow_sign_texlists[3] = {
#include "stg03/arrow_sign/arrow_sign_texlists.inc"
};

static void arrow_sign_destroy(struct task *task);
static void arrow_sign_step(struct task *task);
static void arrow_sign_draw(struct task *task);
static void arrow_sign_2_draw(struct task *task);

void arrow_sign_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = arrow_sign_draw;
        task->step = arrow_sign_step;
        task->destructor = arrow_sign_destroy;
    }
}

void arrow_sign_2_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = arrow_sign_2_draw;
        task->step = arrow_sign_step;
        task->destructor = arrow_sign_destroy;
    }
}

static void arrow_sign_destroy(struct task *task)
{
}

static void arrow_sign_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings and for
     * the register the step saves. SHC still places the strings in this
     * source's constants, and they include the SET editor labels. the
     * image holds none of the block's code: its local and its two draws only
     * keep the register saved */
    if (0) {
        NJS_CNK_MODEL *model = &arrow_sign_model;

        (void)&arrow_sign_object;
        (void)&arrow_sign_2_object;
        (void)ARROW_SIGN_EDITOR_ANGLE_X;
        (void)ARROW_SIGN_EDITOR_ANGLE_Y;
        (void)ARROW_SIGN_EDITOR_ANGLE_Z;
        (void)ARROW_SIGN_EDITOR_SCLX;
        (void)ARROW_SIGN_EDITOR_SCLY;
        (void)ARROW_SIGN_EDITOR_SCLZ;
        njCnkEasyDrawModel(model);
        njCnkEasyDrawModel(model);
    }
}

static void arrow_sign_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    Float radius;

    njSetTexture(&arrow_sign_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njScale(NULL, work->scl.x + 1.0F, work->scl.y + 1.0F, work->scl.z + 1.0F);
    scale = work->scl.x;
    if (work->scl.y > scale) {
        scale = work->scl.y;
    }
    if (work->scl.z > scale) {
        scale = work->scl.z;
    }
    scale += 1.0F;
    radius = arrow_sign_model.r;
    arrow_sign_model.r = scale * radius;
    njCnkEasyDrawModel(&arrow_sign_model);
    arrow_sign_model.r = radius;
    njPopMatrixEx();
}

static void arrow_sign_2_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    Float radius;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&arrow_sign_2_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njScale(NULL, work->scl.x + 1.0F, work->scl.y + 1.0F, work->scl.z + 1.0F);
    scale = work->scl.x;
    if (work->scl.y > scale) {
        scale = work->scl.y;
    }
    if (work->scl.z > scale) {
        scale = work->scl.z;
    }
    scale += 1.0F;
    /* @bug logic: scales the radius of YAJI's model, though the draw is of
     * YAJI2's, so YAJI2's sign is clipped by its unscaled radius. MOV.L at
     * 0x8c508a14 takes YAJI's model, and MOV.L at 0x8c508a1e YAJI2's */
    radius = arrow_sign_model.r;
    arrow_sign_model.r = scale * radius;
    njCnkEasyDrawModel(&arrow_sign_2_model);
    arrow_sign_model.r = radius;
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
