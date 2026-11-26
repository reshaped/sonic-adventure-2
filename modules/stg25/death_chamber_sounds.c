/* Death Chamber's sound list */
#include "sound/lists.h"
#include "stg25/death_chamber_sounds.h"

static const struct sound death_chamber_sounds[11] = {
#include "stg25/death_chamber_sounds/death_chamber_sounds.inc"
};
const struct sound_list death_chamber_sound_list = {
#include "stg25/death_chamber_sounds/death_chamber_sound_list.inc"
};
