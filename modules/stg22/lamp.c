#include "stg22/fixture_models.h"
#include <sg_xpt.h>
#include <stdlib.h>
#include <ninja.h>

#include "object/set.h"
#include "stg22/prop_textures.h"
#include "stg22/scenery.h"
#include "system/task.h"
#include "system/util.h"

NJS_TEXLIST *gadget_lamp_texlists[] = {
    &gadget_lamp_frames[0].list,
    &gadget_lamp_frames[1].list,
    &gadget_lamp_frames[2].list,
    NULL,
};
/* the lamp shows one of its texture lists for frame_interval + 2 steps at a
 * time. it picks the list by a whole number drawn below FRAME_DRAW, mod
 * GADGET_LIGHT_FRAMES */
#define FRAME_DRAW 100.0F
static int frame_interval = 2;

static void lamp_destroy(struct task *task);
static void lamp_step(struct task *task);
static void lamp_draw(struct task *task);

void gadget_lamp_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = lamp_draw;
        task->step = lamp_step;
        task->destructor = lamp_destroy;
    }
}

static void lamp_destroy(struct task *task)
{
}

static void lamp_step(struct task *task)
{
    struct task_work *work = task->work;
    int frame;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->timer > frame_interval) {
        work->timer = 0;
        frame = (int)(rand() * (FRAME_DRAW * RAND_SCALE)) % GADGET_LIGHT_FRAMES;
        work->tick = frame;
    } else {
        work->timer++;
    }
    /* @hack redundant: the set editor's labels, a call of rand and stores of
     * the work's mode to tick and id, in a block no path reaches */
    if (0) {
        int frame_setting = work->mode;

        (void)"<- ROT X";
        (void)"<- ROT Y";
        (void)"<- ROT Z";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        rand();
        work->tick = frame_setting;
        work->id = frame_setting;
    }
}

static void lamp_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &gadget_lamp_object;
    NJS_CNK_MODEL *model = object->model;
    /* the cull takes the model's r. the draw scales it by 1 plus the larger
     * of scl.x and scl.y. every lamp draws the one model, so the draw then
     * sets r back to radius */
    Float radius = model->r;
    Float scale;

    /* @hack spelling: the radius worked out in scale, and the model reached
     * through object below, where one expression and model would do */
    scale = work->scl.x;
    scale = scale > work->scl.y ? scale + 1.0F : work->scl.y + 1.0F;
    scale *= radius;
    model->r = scale;
    njSetTexture(gadget_lamp_texlists[work->tick]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njScale(NULL, work->scl.x + 1.0F, work->scl.y + 1.0F, 1.0F);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    object->model->r = radius;
}
