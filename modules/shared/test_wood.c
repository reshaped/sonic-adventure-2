/* the test wood, TESTWOOD in a stage's object list: one model of scenery,
 * turned and scaled. nothing collides with it */
#include "shared/test_wood/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/test_wood.h"
#include "system/task.h"

/* the wood's range, squared as placement_try_leave_by takes it */
#define RANGE (2050.0F * 2050.0F)

static NJS_TEXNAME test_wood_textures[3] = {
#include "shared/test_wood/test_wood_textures.inc"
};

NJS_TEXLIST test_wood_texlist = {
#include "shared/test_wood/test_wood_texlist.inc"
};

static Sint16 test_wood_polygons[243] = {
#include "shared/test_wood/test_wood_polygons.inc"
};

static Sint32 test_wood_vertices[107] = {
#include "shared/test_wood/test_wood_vertices.inc"
};

static NJS_CNK_MODEL test_wood_model = {
#include "shared/test_wood/test_wood_model.inc"
};

static NJS_CNK_OBJECT test_wood_object = {
#include "shared/test_wood/test_wood_object.inc"
};

static void test_wood_destroy(struct task *task);
static void test_wood_step(struct task *task);
static void test_wood_draw(struct task *task);

void test_wood_init(struct task *task)
{
    if (!placement_try_leave_by(task, RANGE)) {
        task->draw = test_wood_draw;
        task->step = test_wood_step;
        task->destructor = test_wood_destroy;
    }
}

static void test_wood_destroy(struct task *task)
{
}

static void test_wood_step(struct task *task)
{
    if (placement_try_leave_by(task, RANGE)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for the strings SHC
     * places in the object, and for the register the step saves. the image
     * holds none of its code, and its local and two draws stand in to keep
     * the register saved */
    if (0) {
        NJS_CNK_MODEL *model = &test_wood_model;

        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- SCL Z";
        njCnkEasyDrawModel(model);
        njCnkEasyDrawModel(model);
    }
}

static void test_wood_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&test_wood_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, work->scl.x + 1.0F, work->scl.y + 1.0F, work->scl.z + 1.0F);
    njCnkEasyDrawModel(&test_wood_model);
    njPopMatrix(1);
}
