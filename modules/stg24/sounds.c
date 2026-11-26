#include "sound/lists.h"
#include "stg24/sounds.h"

static const struct sound stage_sounds[29] = {
#include "stg24/sounds/stage_sounds.inc"
};

const struct sound_list engine_sounds = {
#include "stg24/sounds/engine_sounds.inc"
};
