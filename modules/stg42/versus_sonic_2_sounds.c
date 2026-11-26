/* the sound list of the second fight of Sonic and Shadow */
#include "sound/lists.h"
#include "stg42/versus_sonic_2_sounds.h"

static const struct sound versus_sonic_2_sounds[9] = {
#include "stg42/versus_sonic_2_sounds/versus_sonic_2_sounds.inc"
};
const struct sound_list versus_sonic_2_sound_list = {
#include "stg42/versus_sonic_2_sounds/versus_sonic_2_sound_list.inc"
};
