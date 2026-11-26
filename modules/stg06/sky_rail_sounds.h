/* Sky Rail's sound list */
#ifndef STG06_SKY_RAIL_SOUNDS_H
#define STG06_SKY_RAIL_SOUNDS_H

#include "sound/lists.h"

/* the stage's sound list, which sky_rail_init hands to sound_load_stage with
 * se_ac_sr.mlt. sounds 0 to 3 are programs 0 to 3 of bank 5. sounds 4 and 5
 * are sequences 0 and 1 of bank 6 */
extern const struct sound_list sky_rail_sound_list;

#endif
