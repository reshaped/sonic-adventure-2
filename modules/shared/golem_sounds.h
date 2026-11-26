/* the Egg Golem fights' sound list, which BOSS_GOLEM and BOSS_GOLEM_E both
 * load */
#ifndef SHARED_GOLEM_SOUNDS_H
#define SHARED_GOLEM_SOUNDS_H

#include "sound/lists.h"

/* the list each fight's setup hands to sound_load_stage, with se_ac_sgl.mlt
 * or se_ac_egl.mlt. sound ids 0 to 15 index it. they play bank 5's numbers 0
 * to 15 */
extern const struct sound_list golem_sound_list;

#endif
