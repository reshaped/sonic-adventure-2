/* the sound list of Security Hall, which security_hall.c loads with
 * se_ac_sh.mlt */
#include "sound/lists.h"
#include "stg08/sounds.h"

/* clang-format off */
static const struct sound programs[] = {
    {5, 0, 0, SOUND_RANGE, 0, 0},
    {5, 1, 0, 0, 0, 0},
    {5, 2, 0, 0, 0, 0},
    {5, 3, 0, 0, 0, 0},
    {5, 51, 0, 0, 0, 0},
    {6, 0, 0, SOUND_RANGE, 0, 0},
    {6, 1, 0, 0, 0, 0},
};
/* clang-format on */
const struct sound_list security_sounds = {
    sizeof(programs) / sizeof(programs[0]), programs};
