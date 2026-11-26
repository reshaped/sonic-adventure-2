/* the versus select, MENU_MODE_VERSUS_SELECT. the two players choose their
 * teams, then a game or a stage of the list of stages.
 * versus_select_draw.h's task draws it */
#ifndef ADVERTISE_VERSUS_SELECT_H
#define ADVERTISE_VERSUS_SELECT_H
#include <sg_xpt.h>

/* start the versus select at its first step, select its textures and play
 * advsng_2.adx. it sets the work's english_names and last_stage_open, as
 * menu_snapshot.h gives them. it leaves menu_control.mode to the caller */
void versus_select_start(void);
/* the versus select's step, under MENU_MODE_VERSUS_SELECT. on the teams,
 * Left or Right on either pad swaps them, and clears both players'
 * match_wins. Choose on either pad opens the list of games. player 0's
 * Cancel clears match_wins too, and goes back to the new menu's versus
 * group under MENU_MODE_NEW_MENU. at any step, the loss of player 1's pad
 * goes back the same way, once in each start.
 *
 * on either list, Up and Down on either pad move the cursor. each pad's
 * Left and Right change its team's look among those its character has
 * open. Cancel on either pad goes back a list, and from the list of games
 * to the teams. Choose on VERSUS_SELECT_STAGES_GAME opens the list of
 * stages. a pair's game plays one of the pair's stages, and the random game
 * one of any pair's. game_state.steps picks it. a stage of the list of
 * stages plays that stage.
 *
 * it returns TRUE to end the menus for the game, and FALSE otherwise. the
 * stage and the players' characters are set, and game_state.next_mode is
 * GAME_MODE_STAGE. menu_snapshot_save has kept the menus' works, and
 * menu_resume_target is the list to resume at, MENU_RESUME_VERSUS_GAMES or
 * MENU_RESUME_VERSUS_STAGES */
Bool versus_select_step(void);
/* come back to the versus select after its game, under
 * MENU_MODE_VERSUS_SELECT. it restores the menus' works from
 * menu_snapshot_save's snapshot, and starts the menu with
 * versus_select_start. the step goes on after a wait, at the list
 * menu_resume_target names. any value but MENU_RESUME_VERSUS_GAMES and
 * MENU_RESUME_VERSUS_STAGES starts nothing */
void versus_select_resume(void);

#endif
