/* Green Forest's sound list. STG03 holds it, and so does STG19, the first
 * fight of Sonic and Shadow */
#include "sound/lists.h"
#include "shared/green_forest_sounds.h"

static const struct sound green_forest_sounds[10] = {
#include "shared/green_forest_sounds/green_forest_sounds.inc"
};
const struct sound_list green_forest_sound_list = {
#include "shared/green_forest_sounds/green_forest_sound_list.inc"
};
