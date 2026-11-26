/* the Chao world's sounds: their lists, the loads of their files, and the
 * names of its voice cues */
#ifndef CHAO_SOUND_H
#define CHAO_SOUND_H
#include <sg_xpt.h>

#include "sound/lists.h"

/* the Chao world's effects, sequences and voices, for every area */
extern const struct sound_list garden_sounds_1, garden_sounds_4,
    garden_sounds_6;
extern const struct sound_list neutral_garden_sounds, hero_garden_sounds,
    dark_garden_sounds, kindergarten_sounds, race_entrance_sounds,
    race_circuit_sounds;

extern const Uint32 neutral_garden_sound_count, hero_garden_sound_count;
extern const Uint32 dark_garden_sound_count, kindergarten_sound_count;
extern const Uint32 race_entrance_sound_count, race_circuit_sound_count;

/* load three files, between sound_load_begin and sound_load_end. they are
 * the Chao world's common file, stage_character(0)'s one-player voices and
 * the character's Chao file. sound/lists.h's places 0 and 7 then hold lists
 * 0 and 7. places 1, 4 and 6 hold the Chao world's three lists. place
 * SOUND_LIST_CHARACTER holds the character's sounds, and SOUND_LIST_VOICES
 * its voices. where the common file fails to load, places 0 and 7 are empty.
 * where the Chao file fails, place SOUND_LIST_CHARACTER is empty */
void garden_sound_load(void);
/* load chao_current_area()'s sound file as sound_load_chao does, which puts
 * its list in place SOUND_LIST_LOADED where it loads. the lobby loads the
 * race's file. CHAO_AREA_ENTRANCE loads its own file, and
 * CHAO_AREA_RACE_ENTRY loads none */
void garden_area_sound_load(void);

/* the names of the Chao world's ADX voice cues */
#define GARDEN_VOICE_CUES 1072
extern const char *garden_voice_cues[GARDEN_VOICE_CUES];
/* the volume a Chao's voice plays at */
#define GARDEN_VOICE_VOLUME 110
/* the voice a Chao plays as it shows CHAO_MARK_QUESTION */
#define GARDEN_QUESTION_VOICE 0x601c
/* the voice a Chao plays as it has an idea: as it finds an empty plot, and as
 * it thinks to water a plant */
#define GARDEN_IDEA_VOICE 0x601a

#endif
