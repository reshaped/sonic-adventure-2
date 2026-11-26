#include "shared/core_sound.h"
#include "sound/lists.h"

static const struct sound core_sounds[24] = {
#include "shared/core_sound/core_sounds.inc"
};
const struct sound_list core_sound_list = {
#include "shared/core_sound/core_sound_list.inc"
};
