/* the init, build and callbacks of a stage's model objects: KAZOBJ's, and
 * Crazy Gadget's KAZ3. the includer defines kazobj_kinds, a struct
 * model_kind_list, and names the init KAZOBJ_INIT and the build KAZOBJ_BUILD.
 *
 * the build compiles a kind's whole tree where its flags hold
 * KAZOBJ_BUILD_TREE, and its root alone otherwise. for Lost Colony, under
 * COLONY_KAZOBJ, it compiles a KIND_NO_FOG kind with fog off. under
 * KAZOBJ_SINGLE_SHAPES, the start ignores KIND_DOUBLE_SHAPES. the
 * placement's state carries the work's submode from one of its tasks to the
 * next */
#ifndef SHARED_KAZOBJ_TASK_H
#define SHARED_KAZOBJ_TASK_H
#include <sg_xpt.h>

#include "system/areas.h"

#define LIST(task) ((struct model_kind_list *)(task)->motion.p)
/* the work's kind, the index scl.x holds */
#define KIND(task, work) (&LIST(task)->kinds[(int)(work)->scl.x])

void KAZOBJ_INIT(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &kazobj_kinds;
        kazobj_start(task);
    }
}

Uint8 *KAZOBJ_BUILD(Uint8 *buffer)
{
    struct model_kind_list *list = &kazobj_kinds;
    int i;
    struct model_kind *kind;
    NJS_CNK_OBJECT copy;
    Uint8 *end;

    /* @hack spelling: a do loop behind a test of the count, where a for loop
     * would do */
    i = 0;
    if (list->count > 0) {
        do {
            kind = &list->kinds[i];
            if ((kind->flags & KIND_BUILD) && !(kind->flags & KIND_COMPILED)) {
#ifdef COLONY_KAZOBJ
                if (kind->flags & KIND_NO_FOG) {
                    njFogDisable();
                }
#endif
                copy = *kind->object;
                if (!(kind->flags & KAZOBJ_BUILD_TREE)) {
                    copy.sibling = NULL;
                    copy.child = NULL;
                    copy.evalflags |= NJD_EVAL_BREAK;
                }
                njSetTexture(kind->texlist);
                end = buffer + njCnkDirectObjectCompileSize(&copy);
                if (end < DEMO_AREA) {
                    njCnkDirectObjectCompile(&copy, buffer);
                    kind->built = (NJS_DIRECT_HEAD *)buffer;
                    buffer = end;
                    kind->flags |= KIND_COMPILED;
                }
#ifdef COLONY_KAZOBJ
                if (kind->flags & KIND_NO_FOG) {
                    njFogEnable();
                }
#endif
            }
        } while (++i < list->count);
    }
    return buffer;
}

static void kazobj_start(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;
#ifndef KAZOBJ_SINGLE_SHAPES
    int i;
#endif

    task->step = kazobj_step;
    task->destructor = kazobj_destroy;
    if (work->scl.x < 0.0F) {
        work->scl.x = 0.0F;
    }
    if ((int)work->scl.x > LIST(task)->count - 1) {
        work->scl.x = LIST(task)->count - 1;
    }
    kind = KIND(task, work);
    if (kind->shapes != NULL) {
#ifdef KAZOBJ_SINGLE_SHAPES
        collision_init(task, kind->shapes, kind->shape_count, COLLISION_OBJECT);
#else
        if (kind->flags & KIND_DOUBLE_SHAPES) {
            collision_init(task, kind->shapes, kind->shape_count * 2,
                           COLLISION_OBJECT);
            for (i = 0; i < kind->shape_count; i++) {
                work->collision->shapes[kind->shape_count + i].flags |=
                    SHAPE_OFF;
            }
        } else {
            collision_init(task, kind->shapes, kind->shape_count,
                           COLLISION_OBJECT);
        }
#endif
    }
    task->draw = kind->draw;
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
}

static void kazobj_destroy(struct task *task)
{
    struct task_work *work = task->work;

    task->motion.p = NULL;
    task->extra.p = NULL;
    task->force.p = NULL;
    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
}

static void kazobj_step(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;

    if (placement_try_leave(task)) {
        return;
    }
    kind = KIND(task, work);
    if (kind->step != NULL) {
        kind->step(task, work, kind);
    } else if (work->collision != NULL) {
        collision_register(task);
    }
}

#endif
