#include "boss_bigbogy/stage.h"
#include "sound/lists.h"

static const struct sound sounds[20] = {
#include "boss_bigbogy/sounds/sounds.inc"
};
const struct sound_list bigbogy_sounds = {
#include "boss_bigbogy/sounds/bigbogy_sounds.inc"
};
