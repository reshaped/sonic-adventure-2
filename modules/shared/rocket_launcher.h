/* the launcher's init and destructor. shared/rocket.c includes them for
 * ROCKET, and stg30/stage_rocket.c for STROCKET, ahead of their own
 * rocket_step. ROCKET_INIT names the init: rocket_init, or stage_rocket_init
 * under FINAL_RUSH_ROCKET. shared/rocket_body.h defines the launcher's draw
 * and the rocket's callbacks */
#ifndef SHARED_ROCKET_LAUNCHER_H
#define SHARED_ROCKET_LAUNCHER_H
#include <mathf.h>
#include <shinobi.h>
#include "system/math.h"
#include "system/util.h"

static void rocket_destroy(struct task *task);
static void rocket_step(struct task *task);
static void rocket_draw_base(struct task *task);
static void rocket_body_init(struct task *task);
static void rocket_body_draw(struct task *task);
static void rocket_body_step(struct task *task);
static void rocket_body_destroy(struct task *task);

void ROCKET_INIT(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR to_place;
    Angle pitch;
    NJS_OBJECT *base;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct launcher));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = rocket_draw_base;
    task->step = rocket_step;
    task->destructor = rocket_destroy;
    work->mode = ROCKET_WAIT;
    /* @hack redundant: body set to null, though syCalloc cleared the
     * record */
    LAUNCHER(task)->body = NULL;
    /* @hack spelling: the way to the place as -pos + scl, where scl - pos
     * would do */
    to_place.x = -work->pos.x + work->scl.x;
    to_place.y = -work->pos.y + work->scl.y;
    to_place.z = -work->pos.z + work->scl.z;
    pitch = (Angle)(atan2f(to_place.y, njSqrt((to_place.z * to_place.z) +
                                              (to_place.x * to_place.x))) *
                    ANGLE_PER_RADIAN);
    work->ang.y = -(Angle)(atan2f(to_place.z, to_place.x) * ANGLE_PER_RADIAN);
    if ((Uint16)pitch > EIGHTH_TURN) {
        work->submode = ROCKET_UPRIGHT;
    }
    base = &LAUNCHER(task)->base;
    *base = rocket_base_collision;
    base->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    base->ang[1] = work->ang.y;
    base->ang[2] = 0;
    base->ang[0] = 0;
    base->pos[0] = work->pos.x;
    base->pos[1] = work->pos.y;
    base->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_UPRIGHT;
    radius = base->model->r + BASE_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, base);
    work->ang.x &= ROCKET_ON_SWITCH;
    work->ang.z &= ROCKET_SWITCH_ID;
    /* @hack redundant: the frames and the riders cleared, though syCalloc
     * cleared the record */
    LAUNCHER(task)->body_frame = 0.0F;
    LAUNCHER(task)->base_frame = 0.0F;
    LAUNCHER(task)->riders = 0;
}

static void rocket_destroy(struct task *task)
{
    if (LAUNCHER(task)->base.model != NULL) {
        world_remove(task, &LAUNCHER(task)->base);
    }
    task_destroy_children(task);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

#endif
