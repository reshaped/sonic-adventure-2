/* Eternal Engine's reactor manager in the Hero story, at a step without player
 * 0's work, then at one with player 0 where the voice starts */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "player/player.h"
#include "shared/field.h"
#include "shared/fog.h"
#include "sound/adx.h"
#include "stg24/eternal_engine.h"
#include "stg24/reactor.h"
#include "system/flags.h"
#include "system/sequence.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

/* the level the stage's own step runs at, below the manager's */
#define STAGE_LEVEL 1

Uint8 game_flags[GAME_FLAGS];
struct task_work *player_work[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
struct task *kill_areas_task;

/* the voices voice_play has played */
static int voices;

void fog_set(struct fog_setting *setting)
{
}

void field_sample_at(NJS_POINT3 *pos, struct field_sample *sample)
{
    memset(sample, 0, sizeof(*sample));
}

void player_fall_out(int slot)
{
}

void player_set_pos(int slot, Float x, Float y, Float z)
{
}

void reactor_lid_init(struct task *task)
{
}

int voice_play(int id, int priority)
{
    voices++;
    return 0;
}

int main(void)
{
    struct task_work player;

    memset(&player, 0, sizeof(player));
    task_init();
    game_flags[FLAG_SEQUENCE] = SEQUENCE_HERO;
    /* the stage's step makes the manager, which steps in the same pass */
    assert(task_create(TASK_WORK, STAGE_LEVEL, stg24_step, "TEST") != NULL);
    task_step_all();
    assert(voices == 0);
    /* player 0 at eternal_engine.c's voice_position */
    player.pos.x = -9.0F;
    player.pos.y = -1480.0F;
    player.pos.z = -9150.0F;
    player_work[0] = &player;
    task_step_all();
    assert(voices == 1);
    task_destroy_all();
    assert(heap_live == 0);
    return 0;
}
