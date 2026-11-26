/* the sandglass's four hooks, which shared/sandglass.h declares, as King Boom
 * Boo's stage defines them. sandglass_shadow_draw draws nothing: the boss's
 * draw calls bigbogy_sandglass_shadow */
#include <sg_xpt.h>

#include "shared/sandglass.h"
#include "sound/sound.h"

/* ids in bigbogy_sounds */
#define LAND_SOUND 3
#define TICK_SOUND 4
#define HIT_SOUND 5
#define SOUND_PRIORITY 0

static Sint32 hit_volume = 100;
static Sint32 tick_volume = 100;
static Sint32 land_volume = 100;

void sandglass_hit_sound(struct task *task)
{
    sound_play(HIT_SOUND, task, SOUND_PRIORITY, 0);
    sound_set_volume(task, SOUND_ANY_ID, hit_volume);
}

void sandglass_tick_sound(struct task *task)
{
    sound_play(TICK_SOUND, task, SOUND_PRIORITY, 0);
    sound_set_volume(task, SOUND_ANY_ID, tick_volume);
}

void sandglass_land_sound(struct task *task)
{
    sound_play(LAND_SOUND, task, SOUND_PRIORITY, 0);
    sound_set_volume(task, SOUND_ANY_ID, land_volume);
}

void sandglass_shadow_draw(struct task *task)
{
}
