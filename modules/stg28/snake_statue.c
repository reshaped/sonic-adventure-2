/* the snake statue, SNAKESTATUE in Pyramid Cave's object list: a solid statue
 * that faces right or left */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg28/snake_statue.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

enum snake_statue_kind { STATUE_RIGHT, STATUE_LEFT, SNAKE_STATUE_KINDS };
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % SNAKE_STATUE_KINDS)

/* task's unk_3c counts 0 to COUNTS - 1 and round again, one a step, from a
 * random start */
#define COUNT(task) ((task)->unk_3c.l)
#define COUNTS 16

/* the statue's shape in the collision world collides only while a player lies
 * within REACH of its pos */
#define REACH 113.56F

#include "stg28/snake_statue/text.inc"
#include "stg28/models/snake_statue.h"

static void snake_statue_destroy(struct task *task);
static void snake_statue_step(struct task *task);
static void snake_statue_draw(struct task *task);

KEPT static NJS_CNK_MODEL *snake_statue_models[SNAKE_STATUE_KINDS] = {
#include "stg28/snake_statue/snake_statue_models.inc"
};
NJS_TEXLIST *cave_snake_statue_texlists[SNAKE_STATUE_KINDS + 1] = {
#include "stg28/snake_statue/cave_snake_statue_texlists.inc"
};
KEPT static NJS_OBJECT *snake_statue_collisions[SNAKE_STATUE_KINDS] = {
#include "stg28/snake_statue/snake_statue_collisions.inc"
};
static char *snake_statue_kind_names[SNAKE_STATUE_KINDS] = {
#include "stg28/snake_statue/snake_statue_kind_names.inc"
};

void cave_snake_statue_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = snake_statue_draw;
    task->step = snake_statue_step;
    task->destructor = snake_statue_destroy;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c50b9d0).
     * snake_statue_destroy tests task->extra for null (TST at 0x8c50ba6c).
     * snake_statue_step hands player_near the copy's pos untested (ADD at
     * 0x8c50baaa) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the statue, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = *snake_statue_collisions[KIND(task)];
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID | WORLD_SIZE_CLASS(1), task, object);
    work->ang.z = 0;
    work->ang.x = 0;
    COUNT(task) = rand() * (COUNTS * RAND_SCALE);
}

static void snake_statue_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void snake_statue_step(struct task *task)
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

static void snake_statue_draw(struct task *task)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(cave_snake_statue_texlists[KIND(task)]);
    njCnkEasyDrawModel(snake_statue_models[KIND(task)]);
    njPopMatrixEx();
}
