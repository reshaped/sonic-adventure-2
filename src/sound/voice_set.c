/* the voice sets, and the call that plays a set's voice */
#include <sg_xpt.h>

#include "sound/adx.h"
#include "sound/voice_set.h"
#include "system/game_state.h"

/* Voice IDs by set and menu theme. Negative IDs mean silence. */
static Sint32 voice_sets[VOICE_SETS][VOICE_SET_VOICES] = {
#include "sound/voice_set/voice_sets.inc"
};

void voice_set_play(int set)
{
    if (voice_sets[set][game_state.theme] >= 0) {
        voice_play_priority_2(voice_sets[set][game_state.theme]);
    }
}
