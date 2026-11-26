/* End an earthquake on an empty slot, then on a grounded player. */
#include <assert.h>

#include <ninja.h>
#include <sg_xpt.h>

#include "camera/stack.h"
#include "player/knuckles.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

struct game_state game_state;
struct task_work *player_work[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];

static int shakes;
static int knocks;
static int sounds;
static int vibrations;

void camera_shake(int camera, int steps, Float size)
{
    shakes++;
}
void player_knock_back(int slot, Float x, Float y, Float z)
{
    knocks++;
}
Sint8 sound_play_for(int id, const void *key, Sint8 priority, Sint16 volume,
                     Sint16 duration)
{
    assert(key == player_work[1]);
    sounds++;
    return 0;
}
void vibration_start(int player, int power, int freq, int time)
{
    vibrations++;
}
void vibration_start_fading(int player, int power, int freq, int length)
{
    vibrations++;
}

#include "extracted.inc"

int main(void)
{
    struct earthquake quake = {1};
    struct task task = {0};
    struct task_work work = {0};
    struct player record = {0};

    task.extra.p = &quake;
    hunter_earthquake_end(&task);
    assert(!shakes && !knocks && !sounds && !vibrations);
    player_work[1] = &work;
    player_records[1] = &record;
    work.flags = PLAYER_WORK_ON_GROUND;
    hunter_earthquake_end(&task);
    assert(shakes == 1 && knocks == 1 && sounds == 1 && vibrations == 2);
    assert(record.request_arg == ANIM_KNOCKED_DOWN &&
           record.stick_lock == EARTHQUAKE_STICK_LOCK);
    return 0;
}
