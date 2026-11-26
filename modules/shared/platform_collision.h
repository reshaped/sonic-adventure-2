/* the init, step and destructor of frstage.h's platforms and of Mad Space's
 * ASHIBA, which each one's source includes. the source defines frstage_draw,
 * NEAR_RANGE and FENCES. the work keeps WORLD_ADDED only while a player is
 * within NEAR_RANGE of its pos. FENCES is the count of fences the platform may
 * carry:
 * - with 4, the source defines fence_collisions, FENCE_SHIFT and FENCE_BITS.
 *   fence n's field of ang.x is FENCE_BITS, shifted n * FENCE_SHIFT bits up.
 *   a field that isn't 0 adds its fence;
 * - with 1, the source defines frstage_fence_collision_0, and FENCE_ENABLED,
 *   the bit of ang.x that adds the fence;
 * - with 0, all three of the work's angles turn the platform, where otherwise
 *   ang.y alone does.
 * the source may define PLATFORM_INIT and PLATFORM_COLLISION together. they
 * name the init and the model it copies, in place of frstage_init and
 * frstage_collision. the source may define PLATFORM_FLAGS in place of
 * WORLD_SOLID */
#ifndef SHARED_PLATFORM_COLLISION_H
#define SHARED_PLATFORM_COLLISION_H
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/frstage.h"
#include "system/task.h"

#ifndef PLATFORM_FLAGS
#define PLATFORM_FLAGS WORLD_SOLID
#endif

#ifndef PLATFORM_INIT
#define PLATFORM_INIT frstage_init
#define PLATFORM_COLLISION frstage_collision
#endif

extern NJS_OBJECT PLATFORM_COLLISION;

/* the platform's node in the collision world, a copy of PLATFORM_COLLISION */
#define NODE(task) ((NJS_OBJECT *)(task)->motion.p)
/* the node takes the size class that its model's radius plus CLASS_MARGIN
 * fits */
#define CLASS_MARGIN 15.0F

static void frstage_destroy(struct task *task);
static void frstage_step(struct task *task);
static void frstage_draw(struct task *task);

void PLATFORM_INIT(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;
#if FENCES == 4
    struct task *child;
    Sint32 i;
#elif FENCES == 1
    struct task *child;
#endif

    if (!placement_try_leave(task)) {
        task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
        if (task->motion.p != NULL) {
            task->draw = frstage_draw;
            task->step = frstage_step;
            task->destructor = frstage_destroy;
            work->mode = 0;
            /* @hack spelling: the copy stored through NODE before object
             * takes the record, where a store through object would do */
            *NODE(task) = PLATFORM_COLLISION;
            object = task->motion.p;
            object->child = NULL;
            object->sibling = NULL;
            object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
#if FENCES
            object->evalflags |= NJD_EVAL_BREAK;
            object->ang[2] = 0;
            object->ang[0] = 0;
#else
            object->ang[2] = work->ang.z;
            object->ang[0] = work->ang.x;
#endif
            object->ang[1] = work->ang.y;
            object->pos[0] = work->pos.x;
            object->pos[1] = work->pos.y;
            object->pos[2] = work->pos.z;
            flags = PLATFORM_FLAGS;
            radius = object->model->r + CLASS_MARGIN;
            if (radius <= WORLD_SIZE_CLASS_1_MAX) {
                if (radius > WORLD_SIZE_CLASS_2_MAX) {
                    flags = PLATFORM_FLAGS | WORLD_SIZE_CLASS(1);
                } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
                    flags = PLATFORM_FLAGS | WORLD_SIZE_CLASS(2);
                } else {
                    flags = PLATFORM_FLAGS | WORLD_SIZE_CLASS(3);
                }
            }
            world_add(flags, task, object);
#if FENCES == 1
            if (work->ang.x & FENCE_ENABLED) {
                child = solid_object_create(task, NULL, NULL, NULL, 0,
                                            frstage_fence_collision_0, 0);
                if (child != NULL) {
                    child->work->pos = work->pos;
                    child->work->ang.y =
                        work->ang.y + frstage_fence_collision_0->ang[1];
                }
            }
#elif FENCES == 4
            for (i = 0; i < FENCES; i++) {
                if ((work->ang.x >> (i * FENCE_SHIFT)) & FENCE_BITS) {
                    child = solid_object_create(task, NULL, NULL, NULL, 0,
                                                fence_collisions[i], 0);
                    if (child != NULL) {
                        child->work->pos = work->pos;
                        child->work->ang.y =
                            work->ang.y + fence_collisions[i]->ang[1];
                    }
                }
            }
#endif
        }
    }
}

static void frstage_destroy(struct task *task)
{
    if (NODE(task)->model != NULL) {
        world_remove(task, NODE(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void frstage_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        if (player_near(&work->pos, NEAR_RANGE)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

#endif
