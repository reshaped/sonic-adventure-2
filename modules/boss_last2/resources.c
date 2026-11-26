#include <sg_xpt.h>
#include "boss_last2/stage.h"
#include "draw/light.h"
#include "sound/lists.h"
#include "object/set.h"

static const struct sound sounds[19] = {
#include "boss_last2/resources/sounds.inc"
};
const struct sound_list finalhazard_sounds = {
#include "boss_last2/resources/finalhazard_sounds.inc"
};
struct light finalhazard_lights[4] = {
    {{0.0F, 0.707F, 0.707F}, 1.0F, 0.5F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.3F, 0.6F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.5F, 0.8F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.0F, 1.0F, 1.0F, 0.5F, 0.5F},
};
Sint32 finalhazard_enabled = 1;
struct set_head *finalhazard_set = NULL;
