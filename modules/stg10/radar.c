/* the radar, RADER in Metal Harbor's object list: a dish that turns on its
 * base */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg10/radar.h"
#include "stg10/radar/text.inc"
#include "system/task.h"

/* the dish's angle about y, kept in the work's timer */
#define DISH_ANG(work) ((work)->timer)

/* Harbor radar base and the dish that rotates above it. */

static NJS_TEXNAME radar_textures[9] = {
#include "stg10/radar/radar_textures.inc"
};

static NJS_TEXLIST radar_texlist = {
#include "stg10/radar/radar_texlist.inc"
};

static Sint16 radar_dish_polygons[579] = {
#include "stg10/radar/radar_dish_polygons.inc"
};

static Sint32 radar_dish_vertices[531] = {
#include "stg10/radar/radar_dish_vertices.inc"
};

static NJS_CNK_MODEL radar_dish_mesh = {
#include "stg10/radar/radar_dish_mesh.inc"
};

static NJS_CNK_OBJECT radar_dish = {
#include "stg10/radar/radar_dish.inc"
};

static Sint16 radar_polygons[219] = {
#include "stg10/radar/radar_polygons.inc"
};

static Sint32 radar_vertices[183] = {
#include "stg10/radar/radar_vertices.inc"
};

static NJS_CNK_MODEL radar_mesh = {
#include "stg10/radar/radar_mesh.inc"
};

static NJS_CNK_OBJECT radar_object = {
#include "stg10/radar/radar_object.inc"
};

NJS_TEXLIST *radar_texlists[2] = {
#include "stg10/radar/radar_texlists.inc"
};

static void radar_destroy(struct task *task);
static void radar_step(struct task *task);
static void radar_draw(struct task *task);

void radar_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = radar_draw;
    task->step = radar_step;
    task->destructor = radar_destroy;
}

static void radar_destroy(struct task *task)
{
}

static void radar_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its SET editor
     * caption, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ROTSPD";
    }
    DISH_ANG(work) += work->ang.y;
}

static void radar_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njSetTexture(&radar_texlist);
    object = &radar_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateY(NULL, DISH_ANG(work));
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}
