#include "sound/lists.h"
#include "stg26/quarters_sounds.h"

/* the stage's sounds. sounds 0 to 8 play bank 5's programs 0 to 8. sound 9
 * plays bank 6's note 0 */
static const struct sound programs[10] = {
#include "stg26/quarters_sounds/programs.inc"
};

const struct sound_list quarters_sounds = {
#include "stg26/quarters_sounds/quarters_sounds.inc"
};
