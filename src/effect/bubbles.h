/* bubbles: the air a player breathes out under water, which rises, grows and
 * ends at the surface */
#ifndef EFFECT_BUBBLES_H
#define EFFECT_BUBBLES_H

/* make the bubbles of the player in slot player, a task named ObjectMizuiki in
 * level 1. nothing is made where the heap can't give it. the bubbles take the
 * parameters of player 0's character. where that is none of Tails, Sonic,
 * Knuckles and Rouge, the task kills itself at its second step. any later step
 * that finds player_work[player] null kills it too.
 *
 * the player starts breathing out at the first step that finds its anim's frame
 * at 1 or less. it never stops. while it breathes out, a step gives off a
 * bubble by chance. a drowning player's work's submode holds
 * PLAYER_REQUEST_DROWN. such a player gives one off each step while it breathes
 * out, and by chance while it doesn't.
 *
 * a bubble leaves unk_224 of the player's record along unk_2c0. it leaves only
 * while the player is under WORLD_FLAT_THIN water, and unk_224 less than 500
 * below its surface. a bubble the heap can't give is left out. a bubble rises,
 * wobbles and grows, kept out of a sphere about the player's head. it shrinks
 * to keep below the surface, and ends there */
void bubbles_create(int player);

#endif
