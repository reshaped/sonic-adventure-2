/* Sand Ocean's sound list, which Hidden Base and the two-player Sand Ocean
 * load too */
#include "shared/sand_ocean_sounds.h"
#include "sound/lists.h"

static const struct sound sand_ocean_sounds[9] = {
#include "shared/sand_ocean_sounds/sand_ocean_sounds.inc"
};
const struct sound_list sand_ocean_sound_list = {
#include "shared/sand_ocean_sounds/sand_ocean_sound_list.inc"
};
