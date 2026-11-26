#include "sound/lists.h"
#include "stg43/sounds.h"

static const struct sound stage_sounds[19] = {
#include "stg43/sounds/stage_sounds.inc"
};

const struct sound_list cosmic_wall_sounds = {
#include "stg43/sounds/cosmic_wall_sounds.inc"
};
