/* Death Chamber's sound list */
#ifndef STG25_DEATH_CHAMBER_SOUNDS_H
#define STG25_DEATH_CHAMBER_SOUNDS_H

#include "sound/lists.h"

/* the stage's sound list, which death_chamber_init gives sound_load_stage.
 * ids 0 to 9 play numbers 0 to 9 of bank 5. id 10 plays number 0 of bank 6.
 * each plays as a note */
extern const struct sound_list death_chamber_sound_list;

#endif
