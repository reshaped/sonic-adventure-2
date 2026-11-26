#include "sound/lists.h"
#include "stg22/sound_list.h"

static const struct sound entries[28] = {
#include "stg22/sound_list/entries.inc"
};
const struct sound_list gadget_sounds = {
#include "stg22/sound_list/gadget_sounds.inc"
};
