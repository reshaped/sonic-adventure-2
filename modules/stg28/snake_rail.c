/* the snake rail, SNEAKRAIL in Pyramid Cave's object list: a solid rail */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg28/snake_rail.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/snake_rail/text.inc"
#include "stg28/models/snake_rail.h"

#define SNAKE_RAIL_KINDS 1
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % SNAKE_RAIL_KINDS)

/* task's unk_3c counts 0 to COUNTS - 1 and round again, one a step, from a
 * random start */
#define COUNT(task) ((task)->unk_3c.l)
#define COUNTS 16

/* the rail's shape in the collision world collides only while a player lies
 * within REACH of its pos */
#define REACH 34.223F

KEPT static NJS_CNK_MODEL *snake_rail_models[SNAKE_RAIL_KINDS] = {
#include "stg28/snake_rail/snake_rail_models.inc"
};
NJS_TEXLIST *cave_snake_rail_texlists[SNAKE_RAIL_KINDS + 1] = {
#include "stg28/snake_rail/cave_snake_rail_texlists.inc"
};
static char *snake_rail_kind_names[SNAKE_RAIL_KINDS] = {
#include "stg28/snake_rail/snake_rail_kind_names.inc"
};

static void snake_rail_destroy(struct task *task);
static void snake_rail_step(struct task *task);
static void snake_rail_draw(struct task *task);

void cave_snake_rail_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = snake_rail_draw;
    task->step = snake_rail_step;
    task->destructor = snake_rail_destroy;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c5099cc).
     * snake_rail_destroy tests task->extra for null (TST at 0x8c509a66).
     * snake_rail_step hands player_near the copy's pos untested (ADD at
     * 0x8c509aa4) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the rail, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_snake_rail_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = 0;
    /* @hack spelling: the work read again through task, where work
     * would do */
    object->ang[1] = task->work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID | WORLD_SIZE_CLASS(2), task, object);
    COUNT(task) = rand() * (COUNTS * RAND_SCALE);
}

static void snake_rail_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void snake_rail_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    if (++COUNT(task) >= COUNTS) {
        COUNT(task) = 0;
    }
}

static void snake_rail_draw(struct task *task)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(cave_snake_rail_texlists[KIND(task)]);
    njCnkEasyDrawModel(snake_rail_models[KIND(task)]);
    njPopMatrixEx();
}
