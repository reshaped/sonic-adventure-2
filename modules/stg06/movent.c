/* MOVENT and MOVENTF in Sky Rail's object list: a solid model whose child turns
 * about z. each object's child turns at a speed of its own. the code is
 * stg07/mimocl.c's but for a time stop, which holds the child still here */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg06/movent.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for the model's placement */
static const char movent_edit_angle_x[12] = "<- ANGLE X";
static const char movent_edit_angle_y[12] = "<- ANGLE Y";
static const char movent_edit_angle_z[12] = "<- ANGLE Z";

#include "stg06/movent/text.inc"
#include "stg06/scenery_art/movent.h"

/* the copy of movent_collision that the object puts in the world */
#define COLLISION(task) ((NJS_OBJECT *)(task)->motion.p)
/* PART_ANGLE is the part's angle about z, in the work's timer. PART_TURN is its
 * turn a step, held in task->extra */
#define PART_ANGLE(work) ((work)->timer)
#define PART_TURN(task) ((task)->extra.l)
/* a player is near within the collision model's radius plus REACH_MARGIN of the
 * work's pos. the same sum picks the model's size class */
#define REACH_MARGIN 15.0F

NJS_TEXLIST *movent_texlists[] = {&movent_texlist, NULL};
/* a part turns movent_turn a step, plus a share of movent_turn_spread degrees
 * that its init picks at random */
static Sint32 movent_turn = 0x80;
static Float movent_turn_spread = 1.5F;

static void movent_destroy(struct task *task);
static void movent_step(struct task *task);
static void movent_draw(struct task *task);

void movent_init(struct task *task)
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
    task->draw = movent_draw;
    task->step = movent_step;
    task->destructor = movent_destroy;
    work->submode = 0;
    PART_ANGLE(work) = RANDOM_TURN();
    task->extra.l = ((Sint32)((Float)rand() * movent_turn_spread *
                              (ANGLE_PER_DEGREE * RAND_SCALE)) +
                     movent_turn);
    object = task->motion.p;
    *object = movent_collision;
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

/* task_destroy frees task->extra once the destructor returns. extra holds the
 * part's turn, not a block of the heap, so the destructor clears it */
static void movent_destroy(struct task *task)
{
    if (COLLISION(task)->model != NULL) {
        world_remove(task, task->motion.p);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void movent_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float reach;

    /* @hack redundant: the else after the return */
    if (placement_try_leave(task)) {
        return;
    } else if (!game_state.time_stopped) {
        PART_ANGLE(work) += PART_TURN(task);
    }
    if (COLLISION(task)->model != NULL) {
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

static void movent_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njSetTexture(&movent_texlist);
    object = &movent_object;
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
