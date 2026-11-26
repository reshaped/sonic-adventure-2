/* PROPELLER's build, init and callbacks, which stg26/propeller.c compiles,
 * and propeller2.c again for PROPELLER2, under SPHERE_PROPELLER. the includer
 * defines PROPELLER(name), and each color's model, object and texture list */
#ifndef STG26_PROPELLER_IMPL_H
#define STG26_PROPELLER_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg26/prop_builds.h"
#include "stg26/propellers.h"
#include "stg26/quarters_util.h"
#include "system/task.h"

#define COLOR(work) ((int)_builtin_fabsf((work)->scl.x) % PROPELLER_COLORS)
#define SIZE(work) ((work)->scl.z + 1.0F)

static void propeller_destroy(struct task *task);
static void propeller_step(struct task *task);
static void propeller_draw(struct task *task);

static char *color_names[PROPELLER_COLORS] = {"RED  ", "GREEN", "BLUE "};
static struct collision_shape shape = {
#ifdef SPHERE_PROPELLER
    0,
    SHAPE_SPHERE,
    PUSH_FIRM(PUSH_ALL_BUT_PICKUPS) | PUSH_ALL_BUT_PICKUPS,
    SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000,
    {0.0F, 0.0F, 0.0F},
    2.5F,
    0.0F,
#else
    0,
    SHAPE_CAPSULE,
    PUSH_FIRM(PUSH_ALL_BUT_PICKUPS) | PUSH_ALL_BUT_PICKUPS,
    SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000,
    {0.0F, 25.0F, 0.0F},
    0.75F,
    25.0F,
#endif
    0.0F,
    0.0F,
    {0, 0, 0}};
static Bool built = FALSE;
static struct model_build builds[PROPELLER_COLORS] = {
    {MODEL_BUILD_ALONE, &red_texlist, &red_object, NULL},
    {MODEL_BUILD_ALONE, &green_texlist, &green_object, NULL},
    {MODEL_BUILD_ALONE, &blue_texlist, &blue_object, NULL}};
#ifdef SPHERE_PROPELLER
/* the radius of the sphere's collision record before SIZE scales it. it
 * replaces the radius collision_fit_radius gives the record */
static Float collision_radius = 3.0F;
#endif

Uint8 *PROPELLER(build)(Uint8 *buffer)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buffer, builds, PROPELLER_COLORS) ==
        PROPELLER_COLORS) {
        built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buffer;
}

void PROPELLER(init)(struct task *task)
{
#ifdef SPHERE_PROPELLER
    struct task_work *work = task->work;
#endif

    if (!placement_try_leave(task)) {
        task->draw = propeller_draw;
        task->step = propeller_step;
        task->destructor = propeller_destroy;
        if (task->work->scl.y == 0.0F) {
            collision_init(task, &shape, 1, COLLISION_OBJECT);
            task->work->collision->shapes->center.y *= SIZE(task->work);
            task->work->collision->shapes->radius *= SIZE(task->work);
            task->work->collision->shapes->size_y *= SIZE(task->work);
            collision_fit_radius(task);
#ifdef SPHERE_PROPELLER
            work->collision->radius = collision_radius * SIZE(task->work);
#endif
        }
    }
}

static void propeller_destroy(struct task *task)
{
}

static void propeller_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: an unreachable collision update before the placement
     * check. the propeller never changes its collision radius in this step */
    if (0) {
        struct collision_work *collision = work->collision;
        struct collision_shape *active_shape = collision->shapes;
        Float radius = collision->radius;

        collision_fit_radius(collision->owner);
        collision->radius = radius;
        active_shape->radius = active_shape->size_y = radius;
        work->collision = collision;
    }
    if (!placement_try_leave(task)) {
        if (task->work->scl.y == 0.0F) {
            collision_register(task);
        }
    }
}

static void propeller_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(PROPELLER(texlists)[COLOR(task->work)]);
    if (built) {
        quarters_direct_draw_scaled(builds[COLOR(task->work)].built,
                                    SIZE(task->work));
    } else {
        quarters_model_draw_scaled(models[COLOR(task->work)], SIZE(task->work));
    }
    njPopMatrixEx();
}

#endif
