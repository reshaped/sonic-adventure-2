/* the motions a module adds to the players' */
#ifndef SHARED_MOTIONS_H
#define SHARED_MOTIONS_H
#include "player/player.h"

/* the module's own motions, which each module that links this source
 * defines. an entry whose index is -1 ends the list */
extern struct motion_entry module_motions[];

/* put each of the module's motions, with its nodes, in its slot of
 * motion_table. an entry whose slot is taken, or whose index lies outside
 * the table, is passed over */
void module_motions_register(void);
/* empty each slot of motion_table that still holds one of the module's
 * motions. first, the call stands each of players 0 and 1 that has a
 * record. it ends the player's anim's blend and sets PLAYER_ANIM_NO_BLEND.
 * it then passes the player's work, motion and record to player_stand */
void module_motions_unregister(void);

#endif
