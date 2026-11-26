/* Mission Street's sound list */
#ifndef SHARED_STREET_SOUNDS_H
#define SHARED_STREET_SOUNDS_H

#include "sound/lists.h"

/* the list the stage's setup hands to sound_load_stage. sound ids 0 to 16
 * index it. they play bank 5's numbers 0 to 12, then bank 6's 0 to 3 */
extern const struct sound_list mission_street_sound_list;

#endif
