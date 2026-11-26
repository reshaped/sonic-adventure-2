/* Final Rush's sound list and SE_TUNNEL task. */
#include "sound/lists.h"
#include "stg30/tunnel_sound.h"

static const struct sound rush_sounds[11] = {
#include "stg30/tunnel_sound/rush_sounds.inc"
};
const struct sound_list final_rush_sound_list = {
#include "stg30/tunnel_sound/final_rush_sound_list.inc"
};

#define FALL_SOUND 1
#include "shared/tunnel_sound_task.h" /* IWYU pragma: keep -- the code it compiles here */
