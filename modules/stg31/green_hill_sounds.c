#include "sound/lists.h"
#include "stg31/green_hill_sounds.h"

/* a row for each enum green_hill_sound, at its value's index */
/* clang-format off */
static const struct sound programs[] = {
    {5, 0, 0, 0, 0, 0},
    {5, 1, 0, 0, 0, 0},
};
/* clang-format on */

const struct sound_list green_hill_sounds = {
    sizeof(programs) / sizeof(programs[0]), programs};
