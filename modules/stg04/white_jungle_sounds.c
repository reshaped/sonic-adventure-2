/* White Jungle's sound list */
#include "sound/lists.h"
#include "stg04/white_jungle_sounds.h"

/* clang-format off */
static const struct sound white_jungle_sounds[] = {
    {5, 0, 0, 0, 0, 0},
    {5, 1, 0, 0, 0, 0},
    {5, 2, 0, 0, 0, 0},
    {5, 3, 0, 0, 0, 0},
    {5, 4, 0, 0, 0, 0},
    {6, 0, 2, 0, 0, 0},
};
/* clang-format on */
const struct sound_list white_jungle_sound_list = {
    sizeof(white_jungle_sounds) / sizeof(white_jungle_sounds[0]),
    white_jungle_sounds};
