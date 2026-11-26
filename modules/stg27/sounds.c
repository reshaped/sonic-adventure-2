#include "sound/lists.h"
#include "stg27/sounds.h"

static const struct sound stage_sounds[26] = {
#include "stg27/sounds/stage_sounds.inc"
};

const struct sound_list lost_colony_sounds = {
#include "stg27/sounds/lost_colony_sounds.inc"
};
