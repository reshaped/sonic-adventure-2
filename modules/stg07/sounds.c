#include "sound/lists.h"
#include "stg07/sounds.h"

/* clang-format off */
static const struct sound stage_sounds[] = {
    {5, 0, 0, 0, 0, 0},
    {5, 1, 0, 0, 0, 0},
    {5, 2, 0, 0, 0, 0},
    {5, 3, 0, 0, 0, 0},
    {5, 4, 0, 0, 0, 0},
    {5, 5, 0, 0, 0, 0},
    {5, 6, 0, 0, 0, 0},
};
/* clang-format on */
const struct sound_list aquatic_mine_sound_list = {
    sizeof(stage_sounds) / sizeof(stage_sounds[0]), stage_sounds};
