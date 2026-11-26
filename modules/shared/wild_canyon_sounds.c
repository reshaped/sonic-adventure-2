#include "sound/lists.h"
#include "shared/wild_canyon_sounds.h"

static const struct sound stage_sounds[11] = {
#include "shared/wild_canyon_sounds/stage_sounds.inc"
};

const struct sound_list wild_canyon_sound_list = {
#include "shared/wild_canyon_sounds/wild_canyon_sound_list.inc"
};
