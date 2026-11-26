#include "shared/carrier_sounds.h"
#include "sound/lists.h"

/* clang-format off */
static const struct sound carrier_sounds[] = {
    {5, 0, 0, 0, 0, 0},
    {5, 1, 0, 0, 0, 0},
    {5, 2, 0, 0, 0, 0},
    {5, 3, 0, 0, 0, 0},
    {5, 4, 0, 0, 0, 0},
    {5, 51, 0, 0, 0, 0},
    {6, 0, 0, 0, 0, 0},
    {6, 1, 0, 0, 0, 0},
};
/* clang-format on */
const struct sound_list carrier_sound_list = {
    sizeof(carrier_sounds) / sizeof(carrier_sounds[0]), carrier_sounds};
