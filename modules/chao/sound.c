/* the loads of the Chao world's sound files */
#include <sg_xpt.h>

#include "chao/sound.h"

#include "chao/garden.h"
#include "player/player.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "stage/stage.h"

void garden_sound_load(void)
{
    Sint8 character = stage_character(0);
    Bool common_loaded;
    const char *character_file;

    sound_load_begin();
    /* @hack spelling: an if and else that set common_loaded, where assigning
     * the comparison would do */
    if (sound_load_file("chao_sound_std.mlt") == 0) {
        common_loaded = TRUE;
    } else {
        common_loaded = FALSE;
    }
    sound_set_character_lists(character);
    if (!common_loaded) {
        sound_set_list(NULL, 0);
        sound_set_list(NULL, 7);
    }
    switch (character) {
    /* @hack redundant: the cases of Sonic and Shadow, whose file the default
     * selects too */
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
        character_file = "chao_chara_ss.mlt";
        break;
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        character_file = "chao_chara_kr.mlt";
        break;
    case PLAYER_TAILS:
    case PLAYER_EGGMAN:
        character_file = "chao_chara_te.mlt";
        break;
    default:
        character_file = "chao_chara_ss.mlt";
        break;
    }
    if (sound_load_file(character_file) != 0) {
        sound_set_list(NULL, SOUND_LIST_CHARACTER);
    }
    sound_set_list(&garden_sounds_1, 1);
    sound_set_list(&garden_sounds_4, 4);
    sound_set_list(&garden_sounds_6, 6);
    sound_load_end();
}

void garden_area_sound_load(void)
{
    switch (chao_current_area()) {
    case CHAO_AREA_NEUTRAL:
        sound_load_chao("chao_neutral_garden.mlt", &neutral_garden_sounds);
        break;
    case CHAO_AREA_HERO:
        sound_load_chao("chao_hero_garden.mlt", &hero_garden_sounds);
        break;
    case CHAO_AREA_DARK:
        sound_load_chao("chao_dark_garden.mlt", &dark_garden_sounds);
        break;
    case CHAO_AREA_KINDERGARTEN:
        sound_load_chao("chao_kindergarten.mlt", &kindergarten_sounds);
        break;
    case CHAO_AREA_ENTRANCE:
        sound_load_chao("chao_race_entrance.mlt", &race_entrance_sounds);
        break;
    case CHAO_AREA_RACE:
        sound_load_chao("chao_race_circuit.mlt", &race_circuit_sounds);
        break;
    /* @hack spelling: a case of its own for the lobby, where its label on the
     * race's case would do */
    case CHAO_AREA_LOBBY:
        sound_load_chao("chao_race_circuit.mlt", &race_circuit_sounds);
        break;
    }
}
