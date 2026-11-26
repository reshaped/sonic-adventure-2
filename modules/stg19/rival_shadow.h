/* ControlShadow, the task that steers player 1 against player 0 in the first
 * fight of Sonic and Shadow */
#ifndef STG19_RIVAL_SHADOW_H
#define STG19_RIVAL_SHADOW_H

/* start ControlShadow with its record. player 1's gauge becomes 3, and a boss
 * gauge shows it. the call loads the title's textures for player 1's kind,
 * which the task's destructor releases. it starts nothing where the heap
 * can't give the task or its record. slot 1 must hold a player, unchecked.
 *
 * the task steers player 1 through its stick and its jump button, and holds
 * it within the arena. while a mini event runs, player 1's time
 * stands still or the stage is cleared, it does nothing. it takes both pads and
 * the pause for the intro, and gives camera 0 a BOSSINIT mode. at the intro's
 * end it gives back player 0's pad and the pause. once either player has
 * missed, it stops steering player 1. it starts the fight over where
 * game_state.steps is 0, and while the stage fades in past the intro. once
 * player 1's gauge is empty, it turns player 0's look to player 1 */
void rival_shadow_create(void);

#endif
