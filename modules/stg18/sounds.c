#include "sound/lists.h"
#include "stg18/sounds.h"

static const struct sound stage_sounds[11] = {
#include "stg18/sounds/stage_sounds.inc"
};

const struct sound_list dry_lagoon_sound_list = {
#include "stg18/sounds/dry_lagoon_sound_list.inc"
};
