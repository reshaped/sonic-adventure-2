/* the init of STEP and of STEPB, which engine_step.c includes once for each.
 * STEP_INIT names the init, and STEP_NAME that platform's draw and collision */
#include "shared/step_models.h"
#include <shinobi.h>

void STEP_INIT(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
#ifdef COLONY_STEP
    struct task *child;
    NJS_POINT3 *light;
#endif
    NJS_VECTOR position;
    Uint32 flags;
    Float radius;
    int kind;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct step_motion));
    if (!task->motion.p) {
        return;
    }
    task->draw = STEP_NAME(draw);
    task->step = step_update;
    task->destructor = step_destroy;
    work->mode = 0;
    CORE_PATH_START(TRACK(task), work, position);
    TRACK(task)->collision = STEP_NAME(collision);
    collision = &TRACK(task)->collision;
    collision->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    collision->evalflags |= NJD_EVAL_BREAK;
    collision->child = NULL;
    collision->sibling = NULL;
    collision->ang[2] = 0;
    collision->ang[0] = 0;
    collision->ang[1] = work->ang.y;
    collision->pos[0] = work->pos.x;
    collision->pos[1] = work->pos.y;
    collision->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    radius = collision->model->r + CLASS_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, collision);
    kind = PATH_KIND(work);
    if (kind == PATH_RISE || kind == PATH_SINK) {
        work->tick = START_READY;
    }
#ifdef COLONY_STEP
    colony_light_zone_create(task);
    if (task->child) {
        /* @hack spelling: light, a pointer to position, where position itself
         * would do */
        light = &position;
        *light = work->pos;
        light->y += LIGHT_HEIGHT;
        task->child->work->pos = *light;
        child = model_object_create(task->child, NULL, NULL, rails, RAILS, 0);
        if (child) {
            child->work->ang.x = 0;
            child->work->ang.z = 0;
        }
    }
#else
#ifndef STEP_B
    model_object_create(task, NULL, NULL, rails, RAILS, 0);
    if (task->child) {
        task->child->work->ang.x = 0;
        task->child->work->ang.z = 0;
    }
#endif
#endif
}
