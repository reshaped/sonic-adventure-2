/* SOLIDBOX's init and callbacks, which shared/solid_box.c and
 * shared/solid_box_e.c compile. the includer defines solid_box_texlist and
 * solid_box_model by shared/solid_box_art.h */
#ifndef SHARED_SOLID_BOX_TASK_H
#define SHARED_SOLID_BOX_TASK_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/solid_box.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the box is 2 * HALF_SIZE on a side, and its pos is the center of its base */
#define HALF_SIZE 10.0F

/* the height of the floor under the box, kept in task->extra in place of a
 * pointer. task_destroy frees a non-null extra, so the destructor clears extra
 * first */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FLOOR(task) (*(Float *)&(task)->extra)
/* FLOOR(task) is NO_FLOOR of collision/surface.h until the box finds a floor.
 * the box looks for one every FLOOR_STEPS steps, a power of 2, from
 * FLOOR_LOOK_HEIGHT above its pos */
#define FLOOR_STEPS 32
#define FLOOR_LOOK_HEIGHT 5.0F

/* the box's work holds its fall in scl, not a scale. FALL_LEFT is the height
 * left to fall, which stack_drop raises. FALL_SPEED is the box's speed in y, 0
 * while it rests. each step the speed keeps FALL_DRAG of itself and loses
 * FALL_GRAVITY */
#define FALL_LEFT(work) ((work)->scl.y)
#define FALL_SPEED(work) ((work)->scl.z)
#define FALL_DRAG 0.99F
#define FALL_GRAVITY 0.08F
/* a box that lands within DUST_HEIGHT of its floor raises puffs along its
 * base's edges */
#define DUST_HEIGHT 5.0F
#define DUST_SCALE 8.0F
#define DUST_DENSITY 1.2F
#define DUST_SPEED 0.5F

#define SOLID_BOX_SHAPES 1

static void solid_box_destroy(struct task *task);
static void solid_box_step(struct task *task);
static void solid_box_draw(struct task *task);

/* the box's shape, which its record and its landing child copy */
static struct collision_shape solid_box_shapes[SOLID_BOX_SHAPES] = {
#include "shared/solid_box_task/solid_box_shapes.inc"
};

void solid_box_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * caption, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = solid_box_draw;
    task->step = solid_box_step;
    task->destructor = solid_box_destroy;
    work->submode = 0;
    FLOOR(task) = NO_FLOOR;
    FALL_LEFT(work) = 0.0F;
    FALL_SPEED(work) = 0.0F;
    collision_init(task, solid_box_shapes, SOLID_BOX_SHAPES, COLLISION_OBJECT);
    stack_add(task);
    work->ang.x = 0;
    work->ang.z = 0;
}

static void solid_box_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    stack_remove(owner);
    owner->extra.p = NULL;
}

static void solid_box_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (FALL_LEFT(work) > 0.0F) {
        /* @hack spelling: height, a local for FALL_LEFT(work), where
         * FALL_LEFT(work) read in place would do */
        Float height = FALL_LEFT(work);
        Float step = FALL_SPEED(work);

        if (height < -step) {
            step = -FALL_LEFT(work);
        }
        work->pos.y += step;
        FALL_LEFT(work) += step;
        FALL_SPEED(work) = (FALL_SPEED(work) * FALL_DRAG) - FALL_GRAVITY;
        /* a falling box's child stands where the box will land. the child has a
         * copy of the box's shape, and no model */
        if (task->child == NULL) {
            model_object_create(task, NULL, NULL, solid_box_shapes,
                                SOLID_BOX_SHAPES, 0);
        }
        if (task->child != NULL) {
            struct task_work *child = task->child->work;

            child->pos.x = work->pos.x;
            child->pos.z = work->pos.z;
            child->pos.y = work->pos.y - FALL_LEFT(work);
        }
    } else if (task->child != NULL) {
        task_destroy_children(task);
    }
    if (FALL_LEFT(work) <= 0.0F && FALL_SPEED(work) != 0.0F) {
        FALL_LEFT(work) = 0.0F;
        FALL_SPEED(work) = 0.0F;
        if (njAbs(work->pos.y - FLOOR(task)) < DUST_HEIGHT) {
            dust_short_rect(&work->pos, HALF_SIZE, HALF_SIZE, &work->ang,
                            DUST_SCALE, DUST_DENSITY, DUST_SPEED);
        }
    }
    if (FLOOR(task) == NO_FLOOR && !(work->timer & (FLOOR_STEPS - 1))) {
        struct angles ang;

        ang.x = 0;
        ang.y = work->ang.y;
        ang.z = 0;
        FLOOR(task) = surface_probe_floor(
            work->pos.x, work->pos.y + FLOOR_LOOK_HEIGHT, work->pos.z, &ang);
    }
    collision_register(task);
    work->timer++;
}

static void solid_box_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&solid_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&solid_box_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}

#endif
