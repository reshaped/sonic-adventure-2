/* the emblem results' menu, MENU_MODE_EMBLEM_RESULTS. it drives
 * emblem_results.h's screen */
#ifndef ADVERTISE_EMBLEM_RESULTS_MENU_H
#define ADVERTISE_EMBLEM_RESULTS_MENU_H
#include <sg_xpt.h>

/* start the menu at its first step, select its textures, gather the
 * screen's lines and play advsng_3.adx. it leaves menu_control.mode to the
 * caller */
void emblem_results_menu_start(void);
/* the menu's step, under MENU_MODE_EMBLEM_RESULTS. left and right turn the
 * pages round. they skip EMBLEM_RESULTS_LAST while the hero or the dark
 * story's award isn't given. each turn puts the cursor on the first line.
 * up and down move the cursor over the page's lines.
 *
 * a choose button starts what emblem_results_choose finds, once the screen
 * has faded out. the step refuses EMBLEM_RESULTS_NOTHING. it refuses the
 * kart race too, until either record of cart_stage has a rank in its first
 * mission. the kart race starts for one player. it and the boss attack
 * start with their from_emblems set, so that each comes back here.
 *
 * it returns TRUE to end the menus for a stage, a story or the Chao world.
 * game_state.next_mode is then set to play it. it returns FALSE otherwise. for
 * a stage or the Chao world, the step has kept the menus' snapshot with
 * menu_snapshot_save. menu_resume_target is then MENU_RESUME_EMBLEM_RESULTS.
 * the Chao world plays for one player, as Sonic. a story's line plays the
 * page's story from its progress in save_data, and sets the story select's
 * from_emblems.
 *
 * a cancel goes back to the new menu's extras group, once the screen has
 * faded out */
Bool emblem_results_menu_step(void);
/* come back to the menu after its stage or the Chao world, and set
 * menu_control.mode to MENU_MODE_EMBLEM_RESULTS. it restores the menus'
 * works from menu_snapshot_save's snapshot. the step took it with the
 * screen open at the line chosen. it starts the menu as
 * emblem_results_menu_start does, but at the wait for the screen to fade
 * in */
void emblem_results_menu_resume(void);

#endif
