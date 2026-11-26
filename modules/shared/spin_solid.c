/* SOLID3 of shared/spin_crates.h, the spinning crate that doesn't break */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/spin_crate_shape.h"
#include "shared/spin_crates.h"
#include "system/task.h"

/* builds holds the crate's one model */
#define SOLID_BUILDS 1

NJS_TEXLIST *spin_solid_texlists[] = {&spin_solid_texlist, NULL};
static Bool built = FALSE;
static struct model_build builds[SOLID_BUILDS] = {
    {MODEL_BUILD_ALONE_AS_LAND, &spin_solid_texlist, &spin_solid_object, NULL},
};
static struct collision_shape shape = CRATE_SHAPE;
static void box_destroy(struct task *task);
static void box_step(struct task *task);
static void box_draw(struct task *task);

Uint8 *spin_solid_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, builds, SOLID_BUILDS) == SOLID_BUILDS) {
        built = TRUE;
    }
    return buffer;
}

void spin_solid_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = box_draw;
        task->step = box_step;
        task->destructor = box_destroy;
        work->submode = 0;
        collision_init(task, &shape, 1, COLLISION_OBJECT);
    }
}

static void box_destroy(struct task *task)
{
}

static void box_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        collision_register(task);
        work->ang.x += (Angle)(work->scl.x * CRATE_TURN_X);
        work->ang.y += (Angle)(work->scl.x * CRATE_TURN_Y);
        work->ang.z += (Angle)(work->scl.x * CRATE_TURN_Z);
    }
}

static void box_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: the set editor's label, in a block no path reaches */
    if (0) {
        (void)"ROTSPD";
    }
    njSetTexture(&spin_solid_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    if (built) {
        njDirectDrawObject(builds[0].built);
    } else {
        njCnkEasyDrawModel(&spin_solid_model);
    }
    njPopMatrixEx();
}
