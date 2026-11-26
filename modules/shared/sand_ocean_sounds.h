/* Sand Ocean's sound list, which Hidden Base and the two-player Sand Ocean
 * load too */
#ifndef SHARED_SAND_OCEAN_SOUNDS_H
#define SHARED_SAND_OCEAN_SOUNDS_H

#include "sound/lists.h"

/* the list each stage's setup hands to sound_load_stage. sound ids 0 to 8
 * index it. they play bank 5's numbers 0 to 7, then bank 6's 0 under
 * SOUND_NOTE */
extern const struct sound_list sand_ocean_sound_list;

#endif
