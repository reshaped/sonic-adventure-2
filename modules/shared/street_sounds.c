/* Mission Street's sound list */
#include "shared/street_sounds.h"
#include "sound/lists.h"

static const struct sound mission_street_sounds[17] = {
#include "shared/street_sounds/mission_street_sounds.inc"
};
const struct sound_list mission_street_sound_list = {
#include "shared/street_sounds/mission_street_sound_list.inc"
};
