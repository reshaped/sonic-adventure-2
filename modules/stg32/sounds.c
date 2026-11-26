#include "sound/lists.h"
#include "stg32/sounds.h"

static const struct sound stage_sounds[12] = {
#include "stg32/sounds/stage_sounds.inc"
};

const struct sound_list meteor_herd_sounds = {
#include "stg32/sounds/meteor_herd_sounds.inc"
};
