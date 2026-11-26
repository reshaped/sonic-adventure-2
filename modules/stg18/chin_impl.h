/* the build, init and callbacks of a piece of Dry Lagoon's ruins. stg18/chin.c
 * compiles them for CHIN, and chin2.c, chin3.c and shelf.c for CHIN2, CHIN3
 * and SHELF. the includer defines chin_texlist, chin_model, chin_object and
 * chin_collision, under those names or its own */
#ifndef STG18_CHIN_IMPL_H
#define STG18_CHIN_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg18/lagoon_util.h"
#include "system/task.h"
#include "system/util.h"

#define BUILDS 1
/* the init puts a copy of chin_collision in the collision world, and keeps it
 * in task->extra. the copy is WORLD_SOLID, and its WORLD_SIZE_CLASS is 1
 * whatever the scale */
#define CHIN_WORLD_FLAGS (WORLD_SOLID | WORLD_SIZE_CLASS(1))
/* the eval flags the copy clears, so that the world reads its pos, ang and
 * scl. chin2.c's copy keeps NJD_EVAL_UNIT_SCL */
#define EVAL_UNIT_POS_ANG (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG)
#ifndef CHIN_EVAL_CLEARED
#define CHIN_EVAL_CLEARED (EVAL_UNIT_POS_ANG | NJD_EVAL_UNIT_SCL)
#endif
/* the copy collides only while a player lies within CHIN_REACH times the
 * scale plus REACH_MARGIN of pos */
#ifndef CHIN_REACH
#define CHIN_REACH 87.0F
#endif
#define REACH_MARGIN 10.0F
/* turn the copy as CHIN_TURN turns the draw. shelf.c turns both about y
 * alone */
#ifndef CHIN_TURN_COPY
#define CHIN_TURN_COPY(object, work)                                           \
    ((object)->ang[0] = (work)->ang.x, (object)->ang[1] = (work)->ang.y,       \
     (object)->ang[2] = (work)->ang.z)
#endif
#ifndef CHIN_TURN
#define CHIN_TURN(work) njRotateEx(&(work)->ang.x, 0)
#endif
NJS_TEXLIST *chin_texlists[] = {&chin_texlist, NULL};
static Bool chin_built = FALSE;
static struct model_build chin_builds[BUILDS] = {
    {MODEL_BUILD_ALONE, &chin_texlist, &chin_object, NULL},
};

static void chin_destroy(struct task *task);
static void chin_step(struct task *task);
static void chin_draw(struct task *task);

Uint8 *chin_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chin_builds, BUILDS) == BUILDS) {
        chin_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void chin_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = chin_draw;
    task->step = chin_step;
    task->destructor = chin_destroy;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: writes chin_collision through null where world_pool_alloc
     * has none to lend. chin_step then reads pos through null, though
     * chin_destroy tests task->extra for it. the write is at 0x6c of the
     * object's code, the test at 0xfe and the read at 0x14a, and shelf.c's
     * test at 0xfc and read at 0x148 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the piece, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = chin_collision;
    object->evalflags &= ~CHIN_EVAL_CLEARED;
    /* @hack spelling: task->work read again for scl.x, where work would do */
    object->scl[0] = object->scl[1] = object->scl[2] = LAGOON_SCALE(task->work);
    CHIN_TURN_COPY(object, work);
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(CHIN_WORLD_FLAGS, task, object);
}

static void chin_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void chin_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    /* @hack spelling: task->work read again for scl.x, where work would do */
    if (player_near((NJS_POINT3 *)object->pos,
                    (LAGOON_SCALE(task->work) * CHIN_REACH) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void chin_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    CHIN_TURN(work);
    njSetTexture(&chin_texlist);
    /* @hack spelling: task->work read again for scl.x in both branches, where
     * work would do */
    if (chin_built) {
        lagoon_direct_draw_scaled(chin_builds[0].built,
                                  LAGOON_SCALE(task->work));
    } else {
        lagoon_model_draw_scaled(&chin_model, LAGOON_SCALE(task->work));
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#endif
