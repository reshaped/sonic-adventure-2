/* the Chao world's sound lists. the effects, sequences and voices serve
 * every area. each of six areas loads its own .mlt file with its own list.
 * an area's list plays programs of bank 12 */
#include <sg_xpt.h>

#include "chao/sound.h"
#include "sound/lists.h"
#include "system/util.h"

static const struct sound effects[51] = {
#include "chao/sound_banks/effects.inc"
};
const struct sound_list garden_sounds_1 = {
#include "chao/sound_banks/garden_sounds_1.inc"
};

static const struct sound sequences[16] = {
#include "chao/sound_banks/sequences.inc"
};
const struct sound_list garden_sounds_4 = {
#include "chao/sound_banks/garden_sounds_4.inc"
};

static const struct sound voices[161] = {
#include "chao/sound_banks/voices.inc"
};
const struct sound_list garden_sounds_6 = {
#include "chao/sound_banks/garden_sounds_6.inc"
};

static const struct sound neutral[4] = {
#include "chao/sound_banks/neutral.inc"
};
const Uint32 neutral_garden_sound_count = ARRAY_COUNT(neutral);
const struct sound_list neutral_garden_sounds = {
#include "chao/sound_banks/neutral_garden_sounds.inc"
};

static const struct sound hero[9] = {
#include "chao/sound_banks/hero.inc"
};
const Uint32 hero_garden_sound_count = ARRAY_COUNT(hero);
const struct sound_list hero_garden_sounds = {
#include "chao/sound_banks/hero_garden_sounds.inc"
};

static const struct sound dark[9] = {
#include "chao/sound_banks/dark.inc"
};
const Uint32 dark_garden_sound_count = ARRAY_COUNT(dark);
const struct sound_list dark_garden_sounds = {
#include "chao/sound_banks/dark_garden_sounds.inc"
};

static const struct sound kindergarten[2] = {
#include "chao/sound_banks/kindergarten.inc"
};
const Uint32 kindergarten_sound_count = ARRAY_COUNT(kindergarten);
const struct sound_list kindergarten_sounds = {
#include "chao/sound_banks/kindergarten_sounds.inc"
};

static const struct sound entrance[4] = {
#include "chao/sound_banks/entrance.inc"
};
const Uint32 race_entrance_sound_count = ARRAY_COUNT(entrance);
const struct sound_list race_entrance_sounds = {
#include "chao/sound_banks/race_entrance_sounds.inc"
};

/* The final two circuit entries both select program 11. */
static const struct sound circuit[13] = {
#include "chao/sound_banks/circuit.inc"
};
const Uint32 race_circuit_sound_count = ARRAY_COUNT(circuit);
const struct sound_list race_circuit_sounds = {
#include "chao/sound_banks/race_circuit_sounds.inc"
};
