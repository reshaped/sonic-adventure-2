/* the mech bosses' sounds: the bank that Big Foot, Hot Shot and Flying Dog
 * load */

#include "shared/mech_boss_sounds.h"
#include "sound/lists.h"
#include "sound/sound.h"

static const struct sound mech_boss_sound_table[20] = {
#include "shared/mech_boss_sounds/mech_boss_sound_table.inc"
};
static const struct sound_list mech_boss_sound_list = {
#include "shared/mech_boss_sounds/mech_boss_sound_list.inc"
};

void mech_boss_sounds_load(void)
{
    sound_load_stage("se_ac_bf.mlt", &mech_boss_sound_list);
}
