/* the updraft, WIND and WIND2 in a stage's object list: an area of rising air.
 * it holds up the players in it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/updraft.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a player rising faster than this doesn't renew its updraft */
#define RISE_LIMIT 1.0F

/* the steps left of each player's updraft. the work's timer holds player 0's,
 * and its tick player 1's */
#define STEPS_LEFT_0(work) ((work)->timer)
#define STEPS_LEFT_1(work) ((work)->tick)

#define VIBRATION_POWER 2
#define VIBRATION_FREQ 38
#define VIBRATION_TIME 1

/* the acceleration player_updraft gives a player on each step its updraft
 * has steps left */
/* @hack type: three Floats, where one NJS_VECTOR would do */
static Float updraft_x = 0.0F;
static Float updraft_y = 0.4F;
static Float updraft_z = 0.0F;
/* the steps left a renewal gives a player's updraft. the step that renews it
 * uses one at once */
static Sint32 updraft_steps = 9;
/* the updraft's box, which no code passes to collision_init. updraft_contains
 * reads only its size_y, the height of WIND's column above pos */
/* clang-format off */
static struct collision_shape updraft_shapes[] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 10.0F, 0.0F}, 165.0F, 20.0F, 90.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void updraft_destroy(struct task *task);
static void updraft_draw(struct task *task);
static void updraft_step(struct task *task);

void updraft_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->step = updraft_step;
    task->destructor = updraft_destroy;
    task->draw = updraft_draw;
}

void updraft_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = updraft_step;
    task->destructor = updraft_destroy;
    task->draw = updraft_draw;
    work->submode = AREA_BOX;
}

static void updraft_destroy(struct task *task)
{
}

static void updraft_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 corner[3];

    /* @hack redundant: statements behind a return, kept for the strings SHC
     * places in the object, and for the frame their locals give the draw.
     * the image holds none of their code, and the statements after the
     * strings stand in for the original's */
    if (1) {
        return;
    }
    (void)"<- ANGLE Y";
    (void)"<- SCL X";
    (void)"<- SCL Z";
    (void)"<- SCL Y";
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njCalcPoint(NULL, &corner[0], &corner[2]);
    njPopMatrixEx();
}

/* whether pos lies in work's area */
static Bool updraft_contains(struct task_work *work, NJS_POINT3 *pos)
{
    NJS_POINT3 point;
    Float above;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, pos, &point);
    njPopMatrix(1);
    if (work->submode == AREA_COLUMN) {
        if (_builtin_fabsf(point.x) < work->scl.x &&
            _builtin_fabsf(point.z) < work->scl.z) {
            /* @hack spelling: above, a local for the height over the
             * column's top, where the difference in the test would do */
            above = point.y - updraft_shapes[0].size_y;
            if (above < 0.0F) {
                return TRUE;
            }
        }
    } else if (_builtin_fabsf(point.x) < work->scl.x &&
               _builtin_fabsf(point.y) < work->scl.y &&
               _builtin_fabsf(point.z) < work->scl.z) {
        return TRUE;
    }
    return FALSE;
}

static void updraft_step(struct task *task)
{
    struct task_work *work = task->work;
    Float limit;
    struct task_work **work_1;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: limit, a local for RISE_LIMIT, where the constant in
     * each test would do */
    limit = RISE_LIMIT;
    if (updraft_contains(work, &player_work[0]->pos) &&
        player_motion[0]->spd.y <= limit) {
        if (STEPS_LEFT_0(work) == 0) {
            vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ, VIBRATION_TIME);
        }
        STEPS_LEFT_0(work) = updraft_steps;
    }
    if (STEPS_LEFT_0(work)) {
        player_updraft(0, updraft_x, updraft_y, updraft_z);
        STEPS_LEFT_0(work)--;
    }
    /* @hack spelling: work_1, a pointer to player 1's slot, where
     * player_work[1] read in place would do */
    work_1 = &player_work[1];
    if (*work_1 != NULL) {
        if (updraft_contains(work, &(*work_1)->pos) &&
            player_motion[1]->spd.y <= limit) {
            /* @bug logic: tests player 0's steps left where player 1's
             * belong. so player 1's pad vibrates on each step that renews
             * its updraft while player 0 has none left, and not as its
             * updraft starts while player 0's runs. MOV.W at 0x204 of the
             * object's code reads the work's timer, player 0's, where player
             * 1's are the tick that MOV.B at 0x21a of the object's code
             * stores */
            if (STEPS_LEFT_0(work) == 0) {
                vibration_start(1, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_TIME);
            }
            STEPS_LEFT_1(work) = updraft_steps;
        }
        if (STEPS_LEFT_1(work)) {
            player_updraft(1, updraft_x, updraft_y, updraft_z);
            STEPS_LEFT_1(work)--;
        }
    }
}
