/* White Jungle's sound list */
#ifndef STG04_WHITE_JUNGLE_SOUNDS_H
#define STG04_WHITE_JUNGLE_SOUNDS_H

#include "sound/lists.h"

/* the stage's sound list, which the stage's setup hands to sound_load_stage.
 * sounds 0 to 4 are programs 0 to 4 of bank 5. sound 5 is sequence 0 of bank
 * 6, which plays at priority 2 */
extern const struct sound_list white_jungle_sound_list;

#endif
