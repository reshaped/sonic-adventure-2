/* the lamp, LAMP in City Escape's object list: a lamp post that only draws.
 * nothing collides with it */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg13/lamp.h"
#include "system/task.h"
#include "stg13/lamp/text.inc"

static void lamp_destroy(struct task *task);
static void lamp_step(struct task *task);
static void lamp_draw(struct task *task);

static NJS_TEXNAME lamp_texnames[3] = {
#include "stg13/lamp/lamp_texnames.inc"
};
static NJS_TEXLIST lamp_texlist = {
#include "stg13/lamp/lamp_texlist.inc"
};
static Sint16 lamp_polygons[399] = {
#include "stg13/lamp/lamp_polygons.inc"
};
static Sint32 lamp_vertices[327] = {
#include "stg13/lamp/lamp_vertices.inc"
};
static NJS_CNK_MODEL lamp_model = {
#include "stg13/lamp/lamp_model.inc"
};
/* @hack type: lamp_object an array of one, where a single node would do */
static NJS_CNK_OBJECT lamp_object[1] = {
#include "stg13/lamp/lamp_object.inc"
};
NJS_TEXLIST *lamp_texlists[2] = {
#include "stg13/lamp/lamp_texlists.inc"
};

void lamp_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = lamp_draw;
    task->step = lamp_step;
    task->destructor = lamp_destroy;
    work->mode = 0;
}

static void lamp_destroy(struct task *task)
{
}

static void lamp_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void lamp_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&lamp_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(lamp_object->model);
    njPopMatrix(1);
}
