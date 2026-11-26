/* the pumpkin's init and callbacks, which shared/pumpkin_pickup.c compiles
 * for Pumpkin Hill's PICPUM1 and stg07/pickup.c for Aquatic Mine's PIC. the
 * includer defines pumpkin_pickup_texlist, pumpkin_pickup_model and
 * PUMPKIN_PICKUP_REST(task). that macro sets the body's BODY_RESTS, or does
 * nothing */
#ifndef SHARED_PUMPKIN_PICKUP_TASK_H
#define SHARED_PUMPKIN_PICKUP_TASK_H

#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define FLOOR_BOUNCE 0.39F

/* ANIMAL(work) keeps the low byte of ang.z, which its label calls
 * "MINIMAL AP(  ) LEV( )": the pumpkin's animal byte of object/animal.h. the
 * first carry may release the animal. a nonzero count of n in
 * ANIMAL_CHANCE_BITS gives a chance of ANIMAL_CHANCE_BASE plus n times
 * ANIMAL_CHANCE_STEP. the animal starts ANIMAL_RISE above the pumpkin */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CHANCE_STEP 0.1F
#define ANIMAL_CHANCE_BASE 0.05F
#define ANIMAL_RISE 3.0F
/* the placement's state once a player has carried the pumpkin. a carry may
 * release the animal only while the placement's state is still 0 */
#define STATE_HELD 1

#define PUMPKIN_SHAPES 3

static struct collision_shape pumpkin_pickup_shapes[PUMPKIN_SHAPES] = {
#include "shared/pumpkin_pickup_task/pumpkin_pickup_shapes.inc"
};

static void pumpkin_pickup_start(struct task *task);
static void pumpkin_pickup_destroy(struct task *task);
static void pumpkin_pickup_step(struct task *task);
static void pumpkin_pickup_draw(struct task *task);

void pumpkin_pickup_init(struct task *task)
{
    /* @hack redundant: work, a local that nothing reads */
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        pumpkin_pickup_start(task);
    }
}

static void pumpkin_pickup_start(struct task *task)
{
    struct task_work *work = task->work;

    task->draw = pumpkin_pickup_draw;
    task->step = pumpkin_pickup_step;
    task->destructor = pumpkin_pickup_destroy;
    ANIMAL(work) = work->ang.z;
    collision_init(task, pumpkin_pickup_shapes, PUMPKIN_SHAPES,
                   COLLISION_OBJECT);
    body_init(task);
    if (task->force.p != NULL) {
        BODY(task)->floor_bounce = FLOOR_BOUNCE;
        PUMPKIN_PICKUP_REST(task);
    }
#ifdef NONMATCHING
    /* this side repairs the tag in pumpkin_pickup_step: a pumpkin without a
     * body kills itself, so it ends at its next step, before body_step */
    if (task->force.p == NULL) {
        task_kill(task);
    }
#endif
    work->submode = 0;
}

static void pumpkin_pickup_destroy(struct task *task)
{
    body_destroy(task);
}

static void pumpkin_pickup_step(struct task *task)
{
    struct task_work *work = task->work;
    int slot;
    Float chance;
    /* @hack redundant: room, four bytes of the frame that nothing reads, and
     * unused, a local that holds room's address */
    Uint32 room[1];
    Uint32 *unused = room;

    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's saved registers */
    if (0) {
        task_destroy(NULL);
        task_destroy(NULL);
        task_destroy(NULL);
    }
    if (!placement_try_leave(task)) {
        if (work->flags & WORK_HELD_BY_PLAYER) {
            slot = player_holding(task);
            if (slot >= 0) {
                work->ang.y = QUARTER_TURN - player_work[slot]->ang.y;
                if (task->placement != NULL && !placement_get_state(task)) {
                    placement_set_state(task, STATE_HELD);
                    if (ANIMAL(work) & ANIMAL_CHANCE_BITS) {
                        /* @hack type: the masked byte cast to Uint8 before
                         * the shift, where ANIMAL(work) >> ANIMAL_CHANCE_SHIFT
                         * would do */
                        chance = (Float)((Uint8)(ANIMAL(work) &
                                                 ANIMAL_CHANCE_BITS) >>
                                         ANIMAL_CHANCE_SHIFT);
                        /* @hack spelling: the product in a statement of its
                         * own, where one expression would do */
                        chance *= ANIMAL_CHANCE_STEP;
                        if (rand() * RAND_SCALE < chance + ANIMAL_CHANCE_BASE) {
                            animal_release(work->pos.x,
                                           work->pos.y + ANIMAL_RISE,
                                           work->pos.z,
                                           (ANIMAL(work) & ANIMAL_KIND_BITS) %
                                               ANIMAL_RELEASE_KINDS);
                        }
                    }
                }
            }
        }
        /* @bug memory: body_step reads and writes the body through null
         * where body_init had no room for it, though pumpkin_pickup_start
         * tests the body for that null. the call is at 0x130 of the object's
         * code, and the TST at 0x4a */
        body_step(task);
    }
}

static void pumpkin_pickup_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&pumpkin_pickup_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&pumpkin_pickup_model);
    njPopMatrixEx();
}

#endif
