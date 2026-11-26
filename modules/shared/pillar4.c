/* PILLAR4 in a stage's object list: a pillar whose object stands in the
 * collision world */
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "shared/pillar4.h"
#include "system/task.h"
#include "system/util.h"

#define SCALE(work) ((work)->scl.x + 1.0F)
/* the step clears WORLD_ADDED while player_near finds no player within
 * NEAR_RADIUS times the scale, plus NEAR_MARGIN. world_collect_near_players
 * then passes over the pillar's object */
#define NEAR_RADIUS 153.0F
#define NEAR_MARGIN 15.0F

NJS_TEXLIST *pillar4_texlists[] = {&pillar4_texlist, NULL};

static void pillar4_destroy(struct task *task);
static void pillar4_step(struct task *task);
static void pillar4_draw(struct task *task);

void pillar4_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = pillar4_draw;
    task->step = pillar4_step;
    task->destructor = pillar4_destroy;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies pillar4_object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x2c of the object's code).
     * pillar4_destroy tests task->extra for that null (TST at 0xbc).
     * pillar4_step hands player_near the copy's pos untested (MOV at 0xf2) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the pillar, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = pillar4_object;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = object->scl[1] = object->scl[2] = SCALE(task->work);
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID, task, object);
}

static void pillar4_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void pillar4_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (!placement_try_leave(task)) {
        object = task->extra.p;
        /* @hack spelling: task->work read again, where work would do */
        if (player_near((NJS_POINT3 *)object->pos,
                        (SCALE(task->work) * NEAR_RADIUS) + NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void pillar4_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&pillar4_texlist);
    /* @hack spelling: task->work read again, where work would do */
    ocean_model_draw_scaled(&pillar4_model, SCALE(task->work));
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
