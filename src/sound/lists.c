/* the sound lists, and the files of each character's sounds and voices */
#include <sg_xpt.h>
#include <string.h>

#include "player/player.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/loader.h"

#define PATH_SIZE 128

/* on STAGE_CHAO, a voice file loads from its name with CHAO_PREFIX in
 * front, where the disc holds that file, and from its own name otherwise */
#include "sound/lists/text.inc"

/* the entries in character_voices and character_voices_2p */
#define VOICES_1P 10
#define VOICES_2P 7

/* the players sound_load_voices takes for the one-player and the two-player
 * voices */
#define ONE_PLAYER 1
#define TWO_PLAYERS 2

/* the bank a voice file loads as. every voice sound names it */
#define VOICE_BANK 3

static const struct sound sounds_0[34] = {
#include "sound/lists/sounds_0.inc"
};
static const struct sound_list sound_list_0 = {
#include "sound/lists/sound_list_0.inc"
};
static const struct sound sounds_1[37] = {
#include "sound/lists/sounds_1.inc"
};
static const struct sound_list sound_list_1 = {
#include "sound/lists/sound_list_1.inc"
};
static const struct sound sounds_ss[20] = {
#include "sound/lists/sounds_ss.inc"
};
static const struct sound_list sound_list_ss = {
#include "sound/lists/sound_list_ss.inc"
};
static const struct sound sounds_kr[23] = {
#include "sound/lists/sounds_kr.inc"
};
static const struct sound_list sound_list_kr = {
#include "sound/lists/sound_list_kr.inc"
};
static const struct sound sounds_wk[25] = {
#include "sound/lists/sounds_wk.inc"
};
static const struct sound_list sound_list_wk = {
#include "sound/lists/sound_list_wk.inc"
};
static const struct sound sounds_te[12] = {
#include "sound/lists/sounds_te.inc"
};
static const struct sound_list sound_list_te = {
#include "sound/lists/sound_list_te.inc"
};
/* the tables below that lack const lie in the image's data. SHC puts a
 * const table among the constants instead. nothing writes them */
static struct sound voice_sounds_ss[31] = {
#include "sound/lists/voice_sounds_ss.inc"
};
static struct sound_list voice_list_ss = {
#include "sound/lists/voice_list_ss.inc"
};
static struct sound voice_sounds_kr[32] = {
#include "sound/lists/voice_sounds_kr.inc"
};
static struct sound_list voice_list_kr = {
#include "sound/lists/voice_list_kr.inc"
};
static struct sound voice_sounds_wk[16] = {
#include "sound/lists/voice_sounds_wk.inc"
};
static const struct sound_list voice_list_wk = {
#include "sound/lists/voice_list_wk.inc"
};
static const struct sound sounds_4[38] = {
#include "sound/lists/sounds_4.inc"
};
static const struct sound_list sound_list_4 = {
#include "sound/lists/sound_list_4.inc"
};
static const struct sound sounds_7[9] = {
#include "sound/lists/sounds_7.inc"
};
static const struct sound_list sound_list_7 = {
#include "sound/lists/sound_list_7.inc"
};

/* the file of character's sounds, and their list */
struct character_sounds {
    Sint8 character;
    const char *file;
    const struct sound_list *list;
};

/* the files of character's voices, and their list */
struct character_voices {
    Sint8 players; /* ONE_PLAYER, or TWO_PLAYERS for the two-player voices */
    Sint8 character;
    /* the file for VOICE_JAPANESE, and for any other voice_language */
    const char *japanese;
    const char *english;
    const struct sound_list *list;
};

static struct character_sounds character_sounds[8] = {
#include "sound/lists/character_sounds.inc"
};
/* characters 9 and 10 have no PLAYER_ name. they take the voices of
 * PLAYER_SONIC and PLAYER_SHADOW */
static struct character_voices character_voices[VOICES_1P] = {
#include "sound/lists/character_voices.inc"
};
static struct character_voices character_voices_2p[VOICES_2P] = {
#include "sound/lists/character_voices_2p.inc"
};

/* The effects registry has eight rows. The original searches as many rows
 * as the one-player voice registry, which also holds two voice aliases. */
#ifdef NONMATCHING
/* this side searches the registry's own rows, and reads none past its end */
#define EFFECTS_SEARCH_COUNT                                                   \
    ((int)(sizeof(character_sounds) / sizeof(character_sounds[0])))
#else
#define EFFECTS_SEARCH_COUNT VOICES_1P
#endif

/* @hack spelling: set_list, an inline function for an assignment, where each
 * call written as its assignment would do */
#pragma inline(set_list)
static void set_list(const struct sound_list **slot,
                     const struct sound_list *list)
{
    *slot = list;
}

/* load voices' file for game_state.voice_language, and set *list to their
 * list */
static void sound_load_voice_entry(struct character_voices *voices,
                                   const struct sound_list **list)
{
    char path[PATH_SIZE];
    char staged[PATH_SIZE];
    const char *file;

    /* @hack spelling: a switch for a test, where an if would do */
    switch (game_state.voice_language) {
    case VOICE_JAPANESE:
        file = voices->japanese;
        break;
    default:
        file = voices->english;
        break;
    }
    if (file != NULL) {
        if (stage_current_id() == STAGE_CHAO) {
            strcpy(staged, CHAO_PREFIX);
            strcat(staged, file);
            if (loader_file_size(staged) == -1) {
                strcpy(path, file);
            } else {
                strcpy(path, staged);
            }
        } else {
            strcpy(path, file);
        }
#ifdef NONMATCHING
        /* this side lists no voices whose bank didn't load */
        if (sound_load_bank(VOICE_BANK, path) != 0) {
            *list = NULL;
            return;
        }
#else
        sound_load_bank(VOICE_BANK, path);
#endif
    }
    /* @hack redundant: the test, whose branches both store voices->list,
     * where one assignment would do */
    if (voices->list != NULL) {
        /* @hack spelling: value, a local for voices->list, not stored
         * directly */
        const struct sound_list *value = voices->list;
        *list = value;
    } else {
        *list = NULL;
    }
}

void sound_load_voices(int players, int character,
                       const struct sound_list **list)
{
    struct character_voices *voices;
    /* @hack type: i as Sint8, where an int would do */
    Sint8 i;
    int other;

    if (players == TWO_PLAYERS) {
        other = player_character(player_find_rival(0));
        voices = character_voices_2p;
        i = 0;
        /* @hack spelling: the search a do-while, where a for loop would do */
        do {
            if (voices->character == character || voices->character == other) {
                break;
            }
            voices++;
        } while (++i < VOICES_2P);
        if (i == VOICES_2P) {
            voices = character_voices_2p;
        }
    } else {
        voices = character_voices;
        i = 0;
        /* @hack spelling: the search a do-while, where a for loop would do */
        do {
            if (voices->character == character) {
                break;
            }
            voices++;
        } while (++i < VOICES_1P);
        if (i == VOICES_1P) {
            voices = character_voices;
        }
    }
    sound_load_voice_entry(voices, list);
}

void sound_load_character(int character, const struct sound_list **list,
                          const struct sound_list **voice_list)
{
    struct character_sounds *sounds;
    struct character_voices *voices;
    /* @hack type: i as Sint8, where an int would do */
    Sint8 i;
    int other;
    const char *file;
    const char *name;

    /* @bug memory: the original counts VOICES_1P entries of character_sounds,
     * which holds 8. a character no entry holds reads entries 8 and 9, past
     * its end, in character_voices. MOV #10,R4 at 0x8c058152 sets the count,
     * and MOV.B @R13,R3 at 0x8c05815c reads */
    for (sounds = character_sounds, i = 0; i < EFFECTS_SEARCH_COUNT;
         i++, sounds++) {
        if (sounds->character == character) {
            break;
        }
    }
    if (i == EFFECTS_SEARCH_COUNT) {
        sounds = character_sounds;
    }
    if (game_state.two_players != 0) {
        other = player_character(player_find_rival(0));
        voices = character_voices_2p;
        i = 0;
        /* @hack spelling: the search a do-while, where a for loop would do */
        do {
            if (voices->character == character || voices->character == other) {
                break;
            }
            voices++;
        } while (++i < VOICES_2P);
        if (i == VOICES_2P) {
            voices = character_voices_2p;
        }
    } else {
        voices = character_voices;
        for (i = 0; i < VOICES_1P; i++, voices++) {
            if (voices->character == character) {
                break;
            }
        }
        if (i == VOICES_1P) {
            voices = character_voices;
        }
    }
    /* @hack spelling: name and file, two locals for sounds->file, where file
     * alone would do */
    name = file = sounds->file;
#ifdef NONMATCHING
    /* this side lists no sounds whose file didn't load */
    if (file == NULL || sound_load_file(name) == 0) {
        *list = sounds->list;
    } else {
        *list = NULL;
    }
#else
    if (file != NULL) {
        sound_load_file(name);
    }
    /* @hack redundant: the test, whose branches both store sounds->list,
     * where one assignment would do */
    if (sounds->list != NULL) {
        /* @hack spelling: value, a local for sounds->list, not stored
         * directly */
        const struct sound_list *value = sounds->list;
        *list = value;
    } else {
        *list = NULL;
    }
#endif
    sound_load_voice_entry(voices, voice_list);
}

void sound_lists_set_0_and_1(const struct sound_list **lists)
{
    set_list(&lists[0], &sound_list_0);
    /* @hack redundant: the test of sounds that are never null */
    if (sound_list_1.sounds != NULL) {
        set_list(&lists[1], &sound_list_1);
    }
}

void sound_lists_set_4_and_7(const struct sound_list **lists)
{
    /* @hack redundant: the tests of sounds that are never null */
    if (sound_list_4.sounds != NULL) {
        set_list(&lists[4], &sound_list_4);
    }
    if (sound_list_7.sounds != NULL) {
        set_list(&lists[7], &sound_list_7);
    }
}

void sound_lists_set_character(int character, const struct sound_list **lists)
{
    struct character_sounds *sounds;
    struct character_voices *voices;
    /* @hack type: i as Sint8, where an int would do */
    Sint8 i;
    const struct sound_list **voice_list;

    set_list(&lists[0], &sound_list_0);
    set_list(&lists[7], &sound_list_7);
    /* @bug memory: the original reads past character_sounds' end, as
     * sound_load_character's does. MOV #10,R6 at 0x8c0582a0 sets the count,
     * and MOV.B @R14,R3 at 0x8c0582a6 reads */
    for (sounds = character_sounds, voices = character_voices, i = 0;
         i < EFFECTS_SEARCH_COUNT; i++, sounds++, voices++) {
        if (sounds->character == character) {
            break;
        }
    }
    if (i == EFFECTS_SEARCH_COUNT) {
        sounds = character_sounds;
        voices = character_voices;
    }
    voice_list = &lists[SOUND_LIST_VOICES];
    sound_load_voice_entry(voices, voice_list);
    if (sounds->list != NULL) {
        set_list(&lists[SOUND_LIST_CHARACTER], sounds->list);
    } else {
        /* @bug logic: sounds without a list clear lists[SOUND_LIST_VOICES],
         * not lists[SOUND_LIST_CHARACTER]. MOV.L R2,@R11 at 0x8c0582de
         * stores through voice_list. no path gets here: every entry the
         * search can find has a list word that isn't null, the two past the
         * table's end included */
        *voice_list = NULL;
    }
}

#ifdef NONMATCHING
/* without unk, as lists.h declares it on this side */
void sound_list_set(const struct sound_list **slot, Sint8 number)
#else
void sound_list_set(const struct sound_list **slot, Sint8 number,
                    struct sound_list_set_unk unk)
#endif
{
    const struct sound_list *list;

    switch (number) {
    case 0:
        list = &sound_list_0;
        break;
    case 1:
        list = &sound_list_1;
        break;
    case 4:
        list = &sound_list_4;
        break;
    case 7:
        list = &sound_list_7;
        break;
    default:
        list = NULL;
        break;
    }
    if (list != NULL) {
        *slot = list;
#ifdef NONMATCHING
        /* this side has no unk to store null in */
    }
#else
    } else {
        /* @hack type: unk, one word that goes on the stack, and the null
         * stored in it, which nothing reads, where neither would do */
        unk.unk_00 = NULL;
    }
#endif
}
