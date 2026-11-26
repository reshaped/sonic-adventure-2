/* Radical Highway's lane, whose collision record holds one shape: neither its
 * init nor its step, cracking or falling, may write past it */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "collision/collision.h"
#include "effect/burst.h"
#include "object/placement.h"
#include "player/player.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg14/lane.h"
#include "system/game_state.h"
#include "system/task.h"
#include "tests/stages/object_world.h"
#include "vmu/vibration.h"

/* lane.c's MODE_QUAKE_ON, a quake's mode while a player stands in it, and its
 * FALL_STEP, the quake's timer from which a cracking lane falls */
#define QUAKE_ON 1
#define FALL_STEP 120

struct game_state game_state;
struct task_work *player_work[PLAYER_SLOTS];

struct particle *burst_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale,
                              Float depth_bias)
{
    return NULL;
}

void dust_rect(NJS_POINT3 *pos, Float half_x, Float half_z, struct angles *ang,
               Float scale, Float density, Float speed)
{
}

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    return 0.0F;
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    return 0;
}

void vibration_start(int player, int power, int freq, int time)
{
}

int main(void)
{
    struct placement placement;
    struct task *lane;
    struct task *quake;

    memset(&placement, 0, sizeof(placement));
    task_init();
    lane = object_start(TASK_WORK | TASK_FORCE, highway_lane_init, &placement);
    assert(lane != NULL && lane->work->collision->count == 1);
    assert(collision_tail_intact(lane));
    /* the quake of the lane's id, 0, on a level of its own */
    quake = task_create(TASK_WORK, OBJECT_LEVEL + 1, highway_lane_quake_init,
                        "QUAKE");
    assert(quake != NULL);
    task_step_level(OBJECT_LEVEL + 1);
    quake->work->mode = QUAKE_ON;
    /* the lane's next step cracks it, and turns its shape off */
    task_step_level(OBJECT_LEVEL);
    assert(lane->work->collision->shapes[0].flags & SHAPE_OFF);
    assert(collision_tail_intact(lane));
    /* past FALL_STEP, the step after drops it, and turns its shape on */
    quake->work->timer = FALL_STEP;
    task_step_level(OBJECT_LEVEL);
    assert(!(lane->work->collision->shapes[0].flags & SHAPE_OFF));
    assert(collision_tail_intact(lane));
    task_destroy(lane);
    task_destroy(quake);
    assert(heap_live == 0 && world_live == 0);
    return 0;
}
