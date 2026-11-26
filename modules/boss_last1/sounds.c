#include "boss_last1/stage.h"
#include "sound/lists.h"

static const struct sound sounds[20] = {
#include "boss_last1/sounds/sounds.inc"
};

const struct sound_list biolizard_sounds = {
#include "boss_last1/sounds/biolizard_sounds.inc"
};
