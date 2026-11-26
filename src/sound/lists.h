/* the sound lists, and the files of each character's sounds and voices.
 *
 * sound/sound.h's sound ids name a sound by its place and its index there.
 * the eight places are pointers, each null or a sound list. the calls here
 * take the eight as lists, and fill some of them */
#ifndef SOUND_LISTS_H
#define SOUND_LISTS_H
#include <sg_xpt.h>

/* the places that hold a character's sounds and its voices */
#define SOUND_LIST_CHARACTER 2
#define SOUND_LIST_VOICES 3
/* the count of places, the first that a stage's load empties, and the one
 * a load puts its list in */
#define SOUND_LISTS 8
#define SOUND_LIST_STAGE 4
#define SOUND_LIST_LOADED 5

/* a sound of a list: the bank and number that play, and how */
struct sound {
    Sint8 bank;
    /* the program or sequence of bank that plays, or below 0 for none */
    Sint8 number;
    /* where above 0, it takes the place of the priority a play call gives */
    Sint8 priority;
    Sint8 flags; /* the SOUND_ flags */
    Sint16 unk_04;
    /* the distance from the camera at which the sound's volume, on
     * sound/sound.h's scale, has fallen by 127. a range of 0 takes
     * SOUND_RANGE_DEFAULT */
    Uint16 range;
};
#define SOUND_RANGE_DEFAULT 600.0F

/* a sound's flags. SOUND_SEQUENCE plays the sound as a sequence, and
 * SOUND_NOTE alone as a note. a sound with neither plays as a sequence where
 * its number is 50 or more. so does one of bank 6 while stage_current_id()
 * is below STAGE_CHAO. any other plays as a note */
#define SOUND_SEQUENCE 0x1 /* a sequence, on a MIDI port of its own */
#define SOUND_NOTE 0x2 /* a note, on a channel of a MIDI port */
/* a sound that plays on after a clear. while game_state.cleared is 1, a play
 * call plays only such sounds. while it isn't 0, sound_update starts no
 * other, and stops another's slot once its time is 0 */
#define SOUND_AFTER_CLEAR 0x4
/* a sound with SOUND_RANGE, played at a pos, plays only within its range of
 * a camera */
#define SOUND_RANGE 0x8

/* a list of count sounds */
struct sound_list {
    Uint8 count;
    const struct sound *sounds;
};

/* sound_list_set's unk: one word, which SHC passes on the stack as a struct */
struct sound_list_set_unk {
    void *unk_00;
};

/* load character's voices for game_state.voice_language, and set *list to
 * their list. while players is 2, they are the two-player voices of the
 * first entry for character or for player_character(player_find_rival(0)).
 * a character the table lacks gets its first entry */
void sound_load_voices(int players, int character,
                       const struct sound_list **list);
/* load character's sounds, and set *list to their list. then load its
 * voices as sound_load_voices does, and set *voice_list to theirs. they are
 * the two-player voices while game_state.two_players isn't 0. a character
 * above 7 gets character 0's sounds */
void sound_load_character(int character, const struct sound_list **list,
                          const struct sound_list **voice_list);
/* set places 0 and 1 of lists to sound lists 0 and 1 */
void sound_lists_set_0_and_1(const struct sound_list **lists);
/* set places 4 and 7 of lists to sound lists 4 and 7 */
void sound_lists_set_4_and_7(const struct sound_list **lists);
/* set places 0 and 7 of lists to sound lists 0 and 7. then load character's
 * one-player voices, and put their list in place SOUND_LIST_VOICES. its
 * sounds' list goes in place SOUND_LIST_CHARACTER, but their file doesn't
 * load. a character above 7 gets character 0's */
void sound_lists_set_character(int character, const struct sound_list **lists);
/* set *slot to sound list number, which is 0, 1, 4 or 7. any other number
 * leaves *slot as it was. it writes null instead over the word of the caller's
 * frame where unk would lie. every call passes number 0, so none writes it */
#ifdef NONMATCHING
/* the hack's plain form, without unk, whose store no call reaches. it needs no
 * second name for the calls that pass no unk */
void sound_list_set(const struct sound_list **slot, Sint8 number);
#else
void sound_list_set(const struct sound_list **slot, Sint8 number,
                    struct sound_list_set_unk unk);
/* sound_list_set, under a second name declared without unk, for the calls
 * that pass slot and number alone */
void sound_list_set_without_unk(const struct sound_list **slot, Sint8 number);
#endif

#endif
