/* the game's flags, a byte each, by number, and the bytes the save keeps as
 * bits */
#ifndef SYSTEM_FLAGS_H
#define SYSTEM_FLAGS_H
#include <sg_xpt.h>

/* how many flags the game has */
#define GAME_FLAGS 512
/* how many bytes game_bits has. each holds 0 or 1, which the save keeps as
 * a bit */
#define GAME_BITS 610

extern Uint8 game_flags[GAME_FLAGS];
extern Uint8 game_bits[GAME_BITS];

/* the music heard, as bits of the flags from FLAG_MUSIC_HEARD on. music n is
 * the music of index n in sound/adx.h's music_names. it has been heard where
 * bit n % 8 of MUSIC_HEARD_BITS[n / 8] is set */
#define FLAG_MUSIC_HEARD 40
#define MUSIC_HEARD_BITS (game_flags + FLAG_MUSIC_HEARD)

/* set a flag to 1, or clear it to 0. flag must be below GAME_FLAGS,
 * unchecked */
void flag_set(int flag);
void flag_clear(int flag);
/* a flag's byte: 1 or 0 as flag_set and flag_clear leave it */
Uint8 flag_get(int flag);
/* flag_get, under a second name declared to return an int */
#ifdef NONMATCHING
/* flag_get's result as an int: this side's objects don't define
 * flag_get_int, which only symbols.txt places */
#define flag_get_int(flag) ((int)flag_get(flag))
#else
int flag_get_int(int flag);
#endif
/* clear every flag, and every byte of game_bits */
void flags_clear_all(void);

#endif
