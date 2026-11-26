/* a stage's sandglasses, kept for other objects to find by id, read, hold
 * and throw */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "shared/sandglass.h"
#include "shared/sandglass_registry.h"
#include "system/angle.h"
#include "system/task.h"

/* a throw sets the sandglass's speed to THROW_SPEED along its holder's
 * heading, so it flies backward. the throw also sets spd.y to THROW_LIFT and
 * ang_spd.x to THROW_TUMBLE */
#define THROW_SPEED (-0.5F)
#define THROW_LIFT 1.5F
#define THROW_TUMBLE (-0x800)

/* free the table and null it. stg28/sandglass_registry.c defines its own,
 * without the test for null */
#ifndef SANDGLASS_TABLE_FREE
#define SANDGLASS_TABLE_FREE                                                   \
    if (sandglasses != NULL) {                                                 \
        syFree(sandglasses);                                                   \
        sandglasses = NULL;                                                    \
    }
#endif

struct task **sandglasses = NULL;
Sint32 sandglass_count = 0;

void sandglass_take_mode(struct task *task)
{
    if (SANDGLASS(task)->next_mode != SANDGLASS_NO_MODE) {
        task->work->mode = SANDGLASS(task)->next_mode;
        SANDGLASS(task)->next_mode = SANDGLASS_NO_MODE;
    }
}

void sandglass_set_mode(struct task *task, int mode)
{
    SANDGLASS(task)->next_mode = mode;
}

struct task **sandglass_register(struct task *task)
{
    Uint32 i;

    if (sandglasses == NULL) {
        sandglasses = syCalloc(SANDGLASS_SLOTS, sizeof(struct task *));
        /* @bug memory: clears the table through null where syCalloc fails,
         * at 0x48 of the object's code. the test at 0x5a shows that the code
         * expects the null */
#ifdef NONMATCHING
        /* this side repairs the tag above: it clears nothing, as syCalloc has
         * cleared the table where it gave one */
#else
        for (i = 0; i < SANDGLASS_SLOTS; ++i) {
            sandglasses[i] = NULL;
        }
#endif
    }
    if (sandglasses != NULL && task != NULL) {
        for (i = 0; i < SANDGLASS_SLOTS; ++i) {
            if (sandglasses[i] == NULL) {
                ++sandglass_count;
                sandglasses[i] = task;
                return &sandglasses[i];
            }
        }
    }
    if (sandglass_count == 0) {
        SANDGLASS_TABLE_FREE
    }
    return NULL;
}

void sandglass_unregister(struct task *task)
{
    Uint32 i;

    /* @hack spelling: the bare test of sandglasses, where != NULL would do */
    if (sandglasses) {
        for (i = 0; i < SANDGLASS_SLOTS; ++i) {
            if (sandglasses[i] == task) {
                --sandglass_count;
                sandglasses[i] = NULL;
                break;
            }
        }
    }
    /* @hack spelling: any test of sandglasses SANDGLASS_TABLE_FREE makes,
     * nested in the test of sandglass_count, where && would do */
    if (sandglass_count == 0) {
        SANDGLASS_TABLE_FREE
    }
}

struct task *sandglass_find(int id)
{
    Uint32 i;
    struct task *task;
    struct task_work *work;

    if (sandglasses != NULL) {
        for (i = 0; i < SANDGLASS_SLOTS; ++i) {
            task = sandglasses[i];
            /* @hack spelling: the nested tests of the slot and its work, where
             * && would do */
            if (task != NULL) {
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((work = task->work) != NULL) {
                    if ((Uint8)work->ang.z == id) {
                        return task;
                    }
                }
            }
        }
    }
    return NULL;
}

Bool sandglass_running(int id)
{
    struct task *task = sandglass_find(id);

    if (task == NULL) {
        return FALSE;
    }
    return task->work->mode == SANDGLASS_MODE_RUNNING;
}

Float sandglass_time_left(int id)
{
    struct task *task = sandglass_find(id);

    if (task != NULL && task->work->mode == SANDGLASS_MODE_RUNNING) {
        return SANDGLASS_RUN_STEPS(task) - SANDGLASS(task)->time;
    }
    return 0.0F;
}

Bool sandglass_pick_up(struct task *task, NJS_POINT3 *pos, Angle ang)
{
    if (task->work->mode != SANDGLASS_MODE_STANDING) {
        return FALSE;
    }
    sandglass_set_mode(task, SANDGLASS_MODE_HELD);
    task->work->pos = *pos;
    /* @hack type: the cast of the turn to Sint16, which sandglass_angle_turn's
     * result already fits */
    SANDGLASS(task)->held_turn =
        (Sint16)sandglass_angle_turn(ang, task->work->ang.y);
    return TRUE;
}

void sandglass_carry(struct task *task, NJS_POINT3 *pos, Angle ang)
{
    if (task->work->mode == SANDGLASS_MODE_HELD) {
        task->work->pos = *pos;
        task->work->ang.y = ang + SANDGLASS(task)->held_turn;
    }
}

void sandglass_throw(struct task *task, Angle ang)
{
    struct sandglass_motion *motion = task->motion.p;

    if (task->work->mode == SANDGLASS_MODE_HELD) {
        sandglass_set_mode(task, SANDGLASS_MODE_THROWN);
        SANDGLASS(task)->tumble_turn =
            sandglass_angle_turn(task->work->ang.y, ang ^ HALF_TURN);
        sandglass_polar_xz(ang, THROW_SPEED, &motion->spd.x, &motion->spd.z);
        motion->spd.y = THROW_LIFT;
        motion->ang_spd.x = THROW_TUMBLE;
        motion->ang_spd.y = 0;
        motion->ang_spd.z = 0;
    }
}
