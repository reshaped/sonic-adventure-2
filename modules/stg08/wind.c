/* WIND and WIND2 in Security Hall's object list: updrafts that lift player 0.
 * the lift lasts while it lies in the wind, and a few steps more */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/updraft.h"
#include "stg08/security_props.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* player 0 in the wind renews the updraft, unless it rises faster than
 * RISE_LIMIT. an updraft that starts anew shakes player 0's pad */
#define RISE_LIMIT 1.0F
#define VIBRATION_POWER 2
#define VIBRATION_FREQ 38
#define VIBRATION_TIME 1

/* the acceleration player_updraft gives player 0 on each step its updraft
 * has steps left. a renewal gives it updraft_steps, and the step that renews
 * it uses one at once */
/* @hack type: three Floats, where one NJS_VECTOR would do */
static Float updraft_x = 0.0F;
static Float updraft_y = 0.4F;
static Float updraft_z = 0.0F;
static Sint16 updraft_steps = 9;
/* WIND's column, as a box shape that no code passes to collision_init.
 * wind_contains takes its radius and size_z as the column's half widths. its
 * size_y is the column's height above pos */
/* @hack type: column_shape declared as an array of one, where a single
 * collision_shape would do */
/* clang-format off */
static struct collision_shape column_shape[] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 10.0F, 0.0F}, 165.0F, 20.0F, 90.0F, 0.0F, {0, 0, 0}}};
/* clang-format on */

static void wind_destroy(struct task *task);
static void wind_draw(struct task *task);
static void wind_step(struct task *task);

void security_wind_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->step = wind_step;
        task->destructor = wind_destroy;
        task->draw = wind_draw;
        work->tick = 0;
    }
}

void security_wind2_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->step = wind_step;
        task->destructor = wind_destroy;
        task->draw = wind_draw;
        work->tick = 0;
        work->submode = AREA_BOX;
    }
}

static void wind_destroy(struct task *task)
{
}

static void wind_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 corners[3];

    /* @hack redundant: statements behind the if (1) return, which no path
     * reaches: the set editor's labels, whose strings SHC still places in
     * this source's constants, and two calls of njCalcPoint */
    if (1) {
        return;
    }
    (void)"<- ANGLE Y";
    (void)"<- SCL X";
    (void)"<- SCL Y";
    (void)"<- SCL Z";
    njCalcPoint(NULL, &work->pos, &corners[0]);
    njCalcPoint(NULL, &work->scl, &corners[1]);
}

static Bool wind_contains(struct task_work *work, NJS_POINT3 *pos)
{
    NJS_POINT3 local;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, pos, &local);
    njPopMatrix(1);
    if (work->submode == AREA_COLUMN) {
        if (_builtin_fabsf(local.x) < column_shape->radius &&
            _builtin_fabsf(local.z) < column_shape->size_z) {
            /* @hack spelling: above, a local for the height over the column's
             * top, where the difference tested in place would do */
            Float above = local.y - column_shape->size_y;
            if (above < 0.0F) {
                return TRUE;
            }
        }
    } else if (_builtin_fabsf(local.x) < work->scl.x &&
               _builtin_fabsf(local.y) < work->scl.y &&
               _builtin_fabsf(local.z) < work->scl.z) {
        return TRUE;
    }
    return FALSE;
}

static void wind_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        if (wind_contains(work, &player_work[0]->pos) &&
            !(player_motion[0]->spd.y > RISE_LIMIT)) {
            if (work->timer == 0) {
                vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_TIME);
            }
            work->timer = updraft_steps;
        }
        if (work->timer) {
            player_updraft(0, updraft_x, updraft_y, updraft_z);
            work->timer--;
        }
        /* @hack redundant: a second test and updraft of player 0, in a block
         * no path reaches */
        if (0) {
            struct task_work *player = player_work[0];
            if (wind_contains(work, &player->pos)) {
                player_updraft(0, player->pos.x, player->pos.y, player->pos.z);
            }
        }
    }
}
