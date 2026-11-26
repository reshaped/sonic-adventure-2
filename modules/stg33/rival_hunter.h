/* ControlRouge, the task that steers player 1 against player 0 in the fight
 * of Knuckles and Rouge */
#ifndef STG33_RIVAL_HUNTER_H
#define STG33_RIVAL_HUNTER_H
#include <ninja.h>

/* the slot of the player, and of the rival ControlRouge steers */
#define PLAYER_SLOT 0
#define RIVAL_SLOT 1

/* start ControlRouge with its record. player 1's gauge becomes 3, and a boss
 * gauge shows it. the call loads the title's textures for player 1's kind,
 * which the task's destructor releases. it starts nothing where the heap
 * can't give the task. where the heap can't give the record, it kills the
 * task. RIVAL_SLOT must hold a player, unchecked.
 *
 * the task steers player 1 through its stick and its jump and action buttons.
 * it gives player 1 the earthquake and the earthflame through
 * game_state.ring_levels[1] and ring_level_timer[1]. it holds both players
 * within the arena's box. while a mini event runs, player 1's time stands
 * still or the stage is cleared, it does nothing. it takes both pads and the
 * pause for the intro, and gives camera 0 a BOSSINIT mode. at the intro's end
 * it gives back player 0's pad and the pause. camera 0 then takes a BOSSPOINT
 * mode. once either player has missed, it stops steering player 1. it starts
 * the fight over where game_state.steps is 0, and while the stage fades in
 * past the intro. a start over fills player 1's gauge. for part of each of
 * its cycles, the task's wind lifts both players and shakes player 0's pad.
 * the task draws the vent the wind rises from. once player 1's gauge is
 * empty, it turns player 0's look to player 1 */
void rival_hunter_create(void);

/* the vent's texture list, which the stage's shared texture groups fill */
extern NJS_TEXLIST vent_texlist;

#endif
