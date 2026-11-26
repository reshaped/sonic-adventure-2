/* Metal Harbor's sound list */
#include "sound/lists.h"
#include "stg10/metal_harbor_sounds.h"

static const struct sound metal_harbor_sounds[13] = {
#include "stg10/metal_harbor_sounds/metal_harbor_sounds.inc"
};
const struct sound_list metal_harbor_sound_list = {
#include "stg10/metal_harbor_sounds/metal_harbor_sound_list.inc"
};
