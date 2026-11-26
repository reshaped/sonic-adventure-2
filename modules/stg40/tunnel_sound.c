/* Final Chase's sound list and SE_TUNNEL task. */
#include "sound/lists.h"
#include "stg40/tunnel_sound.h"

static const struct sound final_chase_sounds[13] = {
#include "stg40/tunnel_sound/final_chase_sounds.inc"
};
const struct sound_list final_chase_sound_list = {
#include "stg40/tunnel_sound/final_chase_sound_list.inc"
};

#define FALL_SOUND 9
#include "shared/tunnel_sound_task.h" /* IWYU pragma: keep -- the code it compiles here */
