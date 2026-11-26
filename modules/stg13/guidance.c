/* the guidance, GUIDANCE in City Escape's object list: a box that steers
 * players on boards toward a point. it also speeds them up */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "stg13/guidance.h"
#include "system/angle.h"
#include "system/task.h"

/* the box's half size along an axis: twice the low byte of the work's angle
 * about that axis */
#define HALF_SIZE(ang) ((Uint8)(ang) * 2.0F)
/* the guidance acts on the players in the first GUIDED_PLAYERS slots */
#define GUIDED_PLAYERS 2
/* the most a step turns a player toward the point */
#define TURN_STEP 0xc00
/* a player is slow while its record's spd.x lies within SLOW_SPEED of 0 */
#define SLOW_SPEED 2.0F
/* a player heading within ON_COURSE of the point gains SPEED_STEP of spd.x a
 * step, up to SPEED_MAX */
#define ON_COURSE 0x1800
#define SPEED_STEP 0.1F
#define SPEED_MAX 1.2F

static void guidance_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[36];

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and for the read of work
     * above it */
    if (0) {
        (void)"->      /X SIZE";
        (void)"-> Y ROT/Y SIZE";
        (void)"->      /Z SIZE";
        (void)"-> DIR X OFFSET";
        (void)"-> DIR Y OFFSET";
        (void)"-> DIR Z OFFSET";
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        njTranslate(NULL, work->scl.x, work->scl.y, work->scl.z);
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    }
}

static void guidance_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;
    Float half_x;
    Float half_y;
    Float half_z;
    Angle heading;
    /* @hack type: mode a Sint16, where an int would do */
    Sint16 mode;
    Bool steered;
    int i;
    struct task_work **unused;

    if (placement_try_leave(task)) {
        task_kill(task);
        return;
    }
    half_x = HALF_SIZE(work->ang.x);
    half_y = HALF_SIZE(work->ang.y);
    half_z = HALF_SIZE(work->ang.z);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    /* @hack redundant: unused, a pointer the loop sets and nothing reads */
    for (i = 0, unused = player_work; i < GUIDED_PLAYERS; i++) {
        if (player_work[i] == NULL) {
            continue;
        }
        njCalcPoint(NULL, &player_work[i]->pos, &point);
        if (njAbs(point.x) > half_x || njAbs(point.y) > half_y ||
            njAbs(point.z) > half_z) {
            continue;
        }
        mode = player_work[i]->mode;
        if (mode < PLAYER_MODE_SONIC_BOARD ||
            mode > PLAYER_MODE_SONIC_BOARD_CRASH) {
            continue;
        }
        heading = atan2f(work->pos.z + work->scl.z - player_work[i]->pos.z,
                         work->pos.x + work->scl.x - player_work[i]->pos.x) *
                  ANGLE_PER_RADIAN;
        steered =
            player_read_stick(player_work[i], player_records[i], NULL, NULL);
        if (mode == PLAYER_MODE_SONIC_BOARD_CRASH ||
            (!steered && njAbs(player_records[i]->spd.x) < SLOW_SPEED)) {
            player_motion[i]->aim_y = player_work[i]->ang.y =
                angle_approach(player_work[i]->ang.y, heading, TURN_STEP);
        }
        if (angle_distance(heading, player_work[i]->ang.y) < ON_COURSE) {
            if (player_records[i]->spd.x < SPEED_MAX) {
                player_records[i]->spd.x += SPEED_STEP;
            }
        }
    }
    njPopMatrixEx();
}

void guidance_init(struct task *task)
{
    task->step = guidance_step;
    task->draw = guidance_draw;
}
