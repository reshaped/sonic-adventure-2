#include "sound/lists.h"
#include "stg44/sounds.h"

static const struct sound stage_sounds[13] = {
#include "stg44/sounds/stage_sounds.inc"
};

const struct sound_list mad_space_sounds = {
#include "stg44/sounds/mad_space_sounds.inc"
};
