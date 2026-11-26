/* Death Chamber's bats, drawn on the frame the swarm starts, before any has
 * had a step, and again after their first step */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "camera/camera.h"
#include "object/placement.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg25/bats.h"
#include "stg25/chamber_util.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

#define BATS 3

struct game_state game_state;
struct task_work *player_work[PLAYER_SLOTS];
struct task_work *camera_task_work;

/* the point each bat's draw last translated to, in the swarm's order */
static NJS_POINT3 drawn[BATS];
static int draws;

void chamber_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = radius;
    *z = radius;
}

Angle chamber_angle_turn(Angle from, Angle to)
{
    return to;
}

void chamber_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    assert(draws < BATS);
    drawn[draws++] = translation;
}

void direction_angles_xy(Float x, Float y, Float z, Angle *ax, Angle *ay)
{
    *ax = *ay = 0;
}

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    return 0.0F;
}

Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *pos)
{
    return 0;
}

/* a swarm of BATS whose line runs through (40, 20, 40), its hanging bats far
 * from player 0 */
static void swarm_start(struct task *task)
{
    task->work->ang.x = 30;
    task->work->ang.y = BATS;
    task->work->scl.x = 40.0F;
    task->work->scl.y = 20.0F;
    task->work->scl.z = 40.0F;
    chamber_bats_init(task);
}

int main(void)
{
    struct placement placement;
    struct task_work camera, player;
    NJS_POINT3 first[BATS];
    struct task *swarm;
    int i;

    memset(&placement, 0, sizeof(placement));
    memset(&camera, 0, sizeof(camera));
    memset(&player, 0, sizeof(player));
    placement.leave.range = 1.0e6F;
    camera_task_work = &camera;
    player.pos.x = 1.0e4F;
    player_work[0] = &player;
    task_init();
    swarm = object_start(TASK_WORK, swarm_start, &placement);
    assert(swarm != NULL);
    swarm->draw(swarm);
    assert(draws == BATS);
    memcpy(first, drawn, sizeof(first));
    /* the first step hangs each bat at its home */
    task_step_level(OBJECT_LEVEL);
    draws = 0;
    swarm->draw(swarm);
    assert(draws == BATS);
    for (i = 0; i < BATS; i++) {
        assert(first[i].x == drawn[i].x && first[i].y == drawn[i].y &&
               first[i].z == drawn[i].z);
    }
    task_destroy(swarm);
    assert(heap_live == 0);
    return 0;
}
