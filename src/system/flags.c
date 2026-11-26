/* the game's flags, a byte each, by number, and the bytes the save keeps as
 * bits */
#include <sg_xpt.h>

#include "system/flags.h"

Uint8 game_flags[GAME_FLAGS];
Uint8 game_bits[GAME_BITS];

void flag_set(int flag)
{
    game_flags[flag] = 1;
}

void flag_clear(int flag)
{
    game_flags[flag] = 0;
}

Uint8 flag_get(int flag)
{
    return game_flags[flag];
}

void flags_clear_all(void)
{
    Uint8 *flag;
    int left;

    /* @hack spelling: each array cleared through a pointer under a count
     * that runs down to 0, where an index that counts up to the array's size
     * would do */
    flag = game_flags;
    for (left = sizeof(game_flags); left != 0; left--) {
        *flag++ = 0;
    }
    flag = game_bits;
    for (left = sizeof(game_bits); left != 0; left--) {
        *flag++ = 0;
    }
}
