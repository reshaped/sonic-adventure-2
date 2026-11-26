/* the updraft, WIND and WIND2 in Eternal Engine's object list: an area of
 * rising air. it holds up the players in it. it is stg14/updraft.c's updraft
 * without the time stop's test and the vibration */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/updraft.h"
#include "stg24/machinery.h"
#include "system/task.h"

/* a player rising faster than this doesn't renew its updraft */
#define RISE_LIMIT 1.0F

/* the steps left of each player's updraft. the work's timer holds player 0's,
 * and its tick player 1's */
#define STEPS_LEFT_0(work) ((work)->timer)
#define STEPS_LEFT_1(work) ((work)->tick)

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

void engine_updraft_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->step = updraft_step;
    task->destructor = updraft_destroy;
}

void engine_updraft_box_init(struct task *task)
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

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the speed's test nested in the area's, where the two
     * joined by && would do */
    if (updraft_contains(work, &player_work[0]->pos)) {
        if (player_motion[0]->spd.y <= RISE_LIMIT) {
            STEPS_LEFT_0(work) = updraft_steps;
        }
    }
    if (STEPS_LEFT_0(work)) {
        player_updraft(0, updraft_x, updraft_y, updraft_z);
        STEPS_LEFT_0(work)--;
    }
    if (player_work[1]) {
        /* @hack spelling: player 1's speed test nested in its area test too,
         * where && would join them */
        if (updraft_contains(work, &player_work[1]->pos)) {
            if (player_motion[1]->spd.y <= RISE_LIMIT) {
                STEPS_LEFT_1(work) = updraft_steps;
            }
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
