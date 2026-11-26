/* Pyramid Cave's sound list */
#include "sound/lists.h"
#include "stg28/pyramid_cave_sounds.h"

static const struct sound pyramid_cave_sounds[15] = {
#include "stg28/pyramid_cave_sounds/pyramid_cave_sounds.inc"
};
const struct sound_list pyramid_cave_sound_list = {
#include "stg28/pyramid_cave_sounds/pyramid_cave_sound_list.inc"
};
