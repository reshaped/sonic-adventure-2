/* the init and destructor of GATE and VULCAN, which each one's source
 * includes. SOLID_INIT and SOLID_DESTROY name the init and the destructor,
 * which this header defines. SOLID_DRAW and SOLID_STEP name the task's draw
 * and step, which the source defines. SOLID_COLLISION is the model whose copy
 * the task puts into the world.
 *
 * the init does nothing where placement_try_leave finds task out of range.
 * where the heap can't give task's motion record, it does nothing, and the
 * next step tries again. otherwise it gives task the source's step, draw and
 * destructor. the record opens with the copy, which has no child or sibling.
 * the copy goes into the world as a solid, at the work's pos, turned by its
 * ang.y. the destructor takes the copy out of the world and frees the record.
 *
 * VULCAN's source defines SOLID_MOTION, the record's type, and SOLID_SHAPE, a
 * collision shape the init gives task. without SOLID_MOTION, the record is
 * the copy alone, and the init sets the work's mode to 0 */
#ifndef STG43_SOLID_IMPL_H
#define STG43_SOLID_IMPL_H
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "system/task.h"

/* the task's node in the collision world, the copy of SOLID_COLLISION */
#define NODE(task) ((NJS_OBJECT *)(task)->motion.p)
/* the node takes the size class that its model's radius plus CLASS_MARGIN
 * fits */
#define CLASS_MARGIN 15.0F

void SOLID_INIT(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    if (!placement_try_leave(task)) {
#ifdef SOLID_MOTION
        task->motion.p = syCalloc(1, sizeof(SOLID_MOTION));
#else
        task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
#endif
        if (task->motion.p != NULL) {
            task->draw = SOLID_DRAW;
            task->step = SOLID_STEP;
            task->destructor = SOLID_DESTROY;
#ifndef SOLID_MOTION
            work->mode = 0;
#endif
            /* @hack spelling: the copy stored through NODE before object
             * takes the record, where a store through object would do */
            *NODE(task) = SOLID_COLLISION;
            object = task->motion.p;
            object->child = NULL;
            object->sibling = NULL;
            object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
            object->ang[2] = 0;
            object->ang[0] = 0;
            object->ang[1] = work->ang.y;
            object->pos[0] = work->pos.x;
            object->pos[1] = work->pos.y;
            object->pos[2] = work->pos.z;
            flags = WORLD_SOLID;
            radius = object->model->r + CLASS_MARGIN;
            if (radius <= WORLD_SIZE_CLASS_1_MAX) {
                if (radius > WORLD_SIZE_CLASS_2_MAX) {
                    flags = WORLD_SOLID | WORLD_SIZE_CLASS(1);
                } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
                    flags = WORLD_SOLID | WORLD_SIZE_CLASS(2);
                } else {
                    flags = WORLD_SOLID | WORLD_SIZE_CLASS(3);
                }
            }
            world_add(flags, task, object);
#ifdef SOLID_MOTION
            collision_init(task, &SOLID_SHAPE, 1, COLLISION_OBJECT);
#endif
        }
    }
}

static void SOLID_DESTROY(struct task *task)
{
    if (NODE(task)->model != NULL) {
        world_remove(task, NODE(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

#endif
