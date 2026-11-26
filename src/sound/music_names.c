/* the ADX filename of each music number, which also numbers its bit among
 * the save's heard bits. music_heard matches a name by strcmp, so case
 * counts */
#include "sound/adx.h"
#include "sound/music_names/text.inc"

const char *music_names[MUSIC_NAMES] = {
#include "sound/music_names/music_names.inc"
};
