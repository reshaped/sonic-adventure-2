/* MIMOCL in Aquatic Mine's object list: a solid model whose child part turns
 * about its z axis. each object's part turns at a steady speed of its own */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg07/mimocl.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg07/mimocl/text.inc"

#include "stg07/scenery/mimocl.h"

/* the copy of the collision object that the world holds, in task->motion */
#define WORLD_OBJECT(task) ((NJS_OBJECT *)(task)->motion.p)
/* the part's angle about its z axis, and what it turns a step, in the word of
 * task->extra */
#define PART_ANGLE(work) ((work)->timer)
#define PART_TURN(task) ((task)->extra.l)
/* the copy collides while a player lies within the model's radius plus
 * REACH_MARGIN of its pos. the same sum gives its size class */
#define REACH_MARGIN 15.0F

NJS_TEXLIST *mimocl_texlists[2] = {
#include "stg07/mimocl/mimocl_texlists.inc"
};
/* a part turns mimocl_turn angle units a step, plus a random share of
 * mimocl_turn_spread degrees. each object draws its share once, as it starts */
static Sint32 mimocl_turn = 0x80;
static Float mimocl_turn_spread = 1.5F;

static void mimocl_destroy(struct task *task);
static void mimocl_step(struct task *task);
static void mimocl_draw(struct task *task);

void mimocl_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float reach;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = mimocl_draw;
    task->step = mimocl_step;
    task->destructor = mimocl_destroy;
    work->submode = 0;
    PART_ANGLE(work) = (Float)rand() * (FULL_TURN * RAND_SCALE);
    PART_TURN(task) = (Sint32)((Float)rand() * mimocl_turn_spread *
                               (ANGLE_PER_DEGREE * RAND_SCALE)) +
                      mimocl_turn;
    object = task->motion.p;
    *object = mimocl_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_NO_CLIMB;
    reach = object->model->r + REACH_MARGIN;
    if (reach <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
}

/* task_destroy frees task->extra once the destructor returns. the object's
 * holds the part's turn, not a block of the heap, so the destructor clears
 * it */
static void mimocl_destroy(struct task *task)
{
    if (WORLD_OBJECT(task)->model != NULL) {
        world_remove(task, task->motion.p);
    }
    PART_TURN(task) = 0;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void mimocl_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float reach;

    if (placement_try_leave(task)) {
        return;
    }
    PART_ANGLE(work) += PART_TURN(task);
    if (WORLD_OBJECT(task)->model != NULL) {
        /* @hack spelling: the assignment to object inside the sum, where a
         * statement of its own would do */
        reach = (object = task->motion.p)->model->r + REACH_MARGIN;
        if (player_near((NJS_POINT3 *)object->pos, reach)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void mimocl_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(&mimocl_texlist);
    object = &mimocl_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateZ(NULL, PART_ANGLE(work));
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}
