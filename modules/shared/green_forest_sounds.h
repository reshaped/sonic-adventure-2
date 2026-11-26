/* Green Forest's sound list. STG03 holds it, and so does STG19, the first
 * fight of Sonic and Shadow */
#ifndef SHARED_GREEN_FOREST_SOUNDS_H
#define SHARED_GREEN_FOREST_SOUNDS_H

#include "sound/lists.h"

/* the list each stage's setup hands to sound_load_stage with se_ac_gf.mlt.
 * sound ids 0 to 9 index it. they play bank 5's numbers 0 to 9 */
extern const struct sound_list green_forest_sound_list;

#endif
