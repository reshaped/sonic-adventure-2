/* Sky Rail's sound list */
#include "sound/lists.h"
#include "stg06/sky_rail_sounds.h"

/* clang-format off */
static const struct sound sky_rail_sounds[] = {
    {5, 0, 0, 0, 0, 0},
    {5, 1, 0, 0, 0, 0},
    {5, 2, 0, 0, 0, 0},
    {5, 3, 0, 0, 0, 0},
    {6, 0, 0, 0, 0, 0},
    {6, 1, 0, 0, 0, 0},
};
/* clang-format on */
const struct sound_list sky_rail_sound_list = {
    sizeof(sky_rail_sounds) / sizeof(sky_rail_sounds[0]), sky_rail_sounds};
