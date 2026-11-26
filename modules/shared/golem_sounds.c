/* the Egg Golem fights' sound list, which BOSS_GOLEM and BOSS_GOLEM_E both
 * load */
#include "shared/golem_sounds.h"
#include "sound/lists.h"

static const struct sound golem_sounds[16] = {
#include "shared/golem_sounds/golem_sounds.inc"
};
const struct sound_list golem_sound_list = {
#include "shared/golem_sounds/golem_sound_list.inc"
};
