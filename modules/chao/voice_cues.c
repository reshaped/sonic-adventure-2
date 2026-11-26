/* the names of the Chao world's ADX voice cues, in groups, each under its
 * group's name. no code reads them */
#include "chao/sound.h"
#include "chao/voice_cues/text.inc"

const char *garden_voice_cues[GARDEN_VOICE_CUES] = {
#include "chao/voice_cues/garden_voice_cues.inc"
};
