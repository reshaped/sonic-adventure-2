/* Egg Quarters' sound list, which the stage loads with se_ac_eq.mlt */
#ifndef STG26_QUARTERS_SOUNDS_H
#define STG26_QUARTERS_SOUNDS_H
#include "sound/lists.h"

/* the list's footstep sound, and the water's. the water plays while player 0
 * is in a pool, at a volume that player 0's height sets */
enum quarters_sound { QUARTERS_FOOTSTEP = 1, QUARTERS_WATER = 9 };
extern const struct sound_list quarters_sounds;

#endif
