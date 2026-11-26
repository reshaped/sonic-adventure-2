/* Metal Harbor's sound list */
#ifndef STG10_METAL_HARBOR_SOUNDS_H
#define STG10_METAL_HARBOR_SOUNDS_H

#include "sound/lists.h"

/* the stage's sound list, which the stage's setup hands to sound_load_stage.
 * sounds 0 to 9 are programs 0 to 9 of bank 5. sounds 10 to 12 are sequences
 * 0 to 2 of bank 6. sound 11 plays at priority 2 */
extern const struct sound_list metal_harbor_sound_list;

#endif
