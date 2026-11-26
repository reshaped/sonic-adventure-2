/* the updraft, WIND and WIND2 in Radical Highway's object list: an area of
 * rising air. it holds up the players in it. it is shared/updraft.c's updraft
 * without a draw, and with a test of the time stop. its column's top is pos,
 * not 20 above it. its defect in the vibration is player 0's, where
 * shared/updraft.c's is player 1's */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/updraft.h"
#include "stg14/updraft.h"
#include "system/game_state.h"
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

static void updraft_destroy(struct task *task);
static void updraft_step(struct task *task);

void highway_updraft_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->step = updraft_step;
    task->destructor = updraft_destroy;
}

void highway_updraft_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = updraft_step;
    task->destructor = updraft_destroy;
    work->submode = AREA_BOX;
}

static void updraft_destroy(struct task *task)
{
}

#include "shared/updraft_area.h"

static void updraft_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 corner[3];
    Float limit;
    struct task_work **work_1;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: limit, a local for RISE_LIMIT, where the constant in
     * each test would do */
    limit = RISE_LIMIT;
    if (!(game_state.time_stopped & TIME_STOPPED_BIT(0))) {
        if (updraft_contains(work, &player_work[0]->pos) &&
            player_motion[0]->spd.y <= limit) {
            /* @bug logic: tests player 1's steps left where player 0's
             * belong. so player 0's pad vibrates on each step that renews its
             * updraft while player 1 has none left, and not as its updraft
             * starts while player 1's runs. MOV.B at 0x8c500f98 reads the
             * work's tick, player 1's, where player 0's are the timer that
             * MOV.W at 0x8c500fae stores */
            /* @hack spelling: the test written with !, where == 0 as player
             * 1's has it would do */
            if (!STEPS_LEFT_1(work)) {
                vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_TIME);
            }
            STEPS_LEFT_0(work) = updraft_steps;
        }
        if (STEPS_LEFT_0(work)) {
            player_updraft(0, updraft_x, updraft_y, updraft_z);
            STEPS_LEFT_0(work)--;
        }
    }
    /* @hack spelling: work_1, a pointer to player 1's slot, where
     * player_work[1] read in place would do */
    work_1 = &player_work[1];
    if (*work_1 != NULL && !(game_state.time_stopped & TIME_STOPPED_BIT(1))) {
        if (updraft_contains(work, &(*work_1)->pos) &&
            player_motion[1]->spd.y <= limit) {
            if (STEPS_LEFT_1(work) == 0) {
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
    /* @hack redundant: a block no path reaches, whose code the image doesn't
     * hold. its strings, the set editor's labels, stay among the source's
     * constants. its call is there only to name corner, which keeps corner in
     * the step's frame */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- SCL Z";
        njCalcPoint(NULL, &corner[0], &corner[2]);
    }
}
