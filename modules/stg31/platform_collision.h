/* what Green Hill's two platforms share. a platform keeps a copy of its
 * collision object in its task's motion. it puts the copy in the world. a step
 * moves the copy with the platform and tells player 0 how far it moved */
#ifndef STG31_PLATFORM_COLLISION_H
#define STG31_PLATFORM_COLLISION_H
#include <ninja.h>
#include <shinobi.h>

#include "collision/world.h"
#include "system/task.h"

#define PLATFORM_OBJECT(task) ((NJS_OBJECT *)(task)->motion.p)
/* the first of the task's TASK_FORCE records, by which player 0 rides */
#define RIDER_FORCE(task) ((struct task_force *)(task)->force.p)
/* a platform's size class in the world comes from its model's radius plus
 * SIZE_MARGIN. a step keeps WORLD_ADDED only while a player is near. a player
 * is near within the model's radius plus REACH_MARGIN of the work's pos */
#define SIZE_MARGIN 15.0F
#define REACH_MARGIN 30.0F
/* add to flags the size class of radius, the model's plus SIZE_MARGIN */
#define ADD_SIZE_CLASS(flags, radius)                                          \
    do {                                                                       \
        if ((radius) <= WORLD_SIZE_CLASS_1_MAX) {                              \
            if ((radius) > WORLD_SIZE_CLASS_2_MAX) {                           \
                (flags) |= WORLD_SIZE_CLASS(1);                                \
            } else if ((radius) > WORLD_SIZE_CLASS_3_MAX) {                    \
                (flags) |= WORLD_SIZE_CLASS(2);                                \
            } else {                                                           \
                (flags) |= WORLD_SIZE_CLASS(3);                                \
            }                                                                  \
        }                                                                      \
    } while (0)

/* put object at the work's pos, turned by its ang.y alone */
/* @hack spelling: a macro, where a #pragma inline function would do */
#define PLACE_PLATFORM(object, work)                                           \
    do {                                                                       \
        (object)->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);       \
        (object)->ang[2] = 0;                                                  \
        (object)->ang[0] = 0;                                                  \
        (object)->ang[1] = (work)->ang.y;                                      \
        (object)->pos[0] = (work)->pos.x;                                      \
        (object)->pos[1] = (work)->pos.y;                                      \
        (object)->pos[2] = (work)->pos.z;                                      \
    } while (0)

#pragma inline(platform_release)
static void platform_release(struct task *task)
{
    if (PLATFORM_OBJECT(task)->model) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* once a step has moved the work from previous, move the world copy to the
 * work's pos. RIDER_FORCE's speed takes the move, which carries player 0 with
 * the platform */
/* @hack spelling: a macro, where a #pragma inline function would do */
#define FOLLOW_WORK(task, work, previous)                                      \
    do {                                                                       \
        RIDER_FORCE(task)->speed.x = (work)->pos.x - (previous).x;             \
        RIDER_FORCE(task)->speed.y = (work)->pos.y - (previous).y;             \
        RIDER_FORCE(task)->speed.z = (work)->pos.z - (previous).z;             \
        PLATFORM_OBJECT(task)->pos[0] = (work)->pos.x;                         \
        PLATFORM_OBJECT(task)->pos[1] = (work)->pos.y;                         \
        PLATFORM_OBJECT(task)->pos[2] = (work)->pos.z;                         \
    } while (0)

#endif
