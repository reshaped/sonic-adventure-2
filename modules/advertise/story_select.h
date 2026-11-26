/* the story select, MENU_MODE_STORY_SELECT. the player picks a story's
 * panel, then a chapter of the story's list. story_select_draw.h's task
 * draws it */
#ifndef ADVERTISE_STORY_SELECT_H
#define ADVERTISE_STORY_SELECT_H
#include <sg_xpt.h>

/* start the story select at its first step, select its textures and play
 * advsng_2.adx. it leaves menu_control.mode to the caller */
void story_select_start(void);
/* if which is STORY_SELECT_REOPEN, send the story select back to its first
 * step. any other which does nothing */
#define STORY_SELECT_REOPEN 0
void story_select_reopen(int which);
/* the story select's step, under MENU_MODE_STORY_SELECT. the last story's
 * panel shows once save_data.awards holds the hero and the dark story's
 * awards. Choose on a story whose award isn't given starts the story at
 * the step save_data.progress keeps. the step then starts the synopsis's
 * screen for it, and sets menu_control.mode to MENU_MODE_SYNOPSIS. Choose
 * on a story whose award is given opens its list of chapters.
 *
 * it returns TRUE to end the menus for the chapter, once the panels have
 * gone, and FALSE otherwise. the story's sequence has started at the
 * chapter, and game_state.next_mode is GAME_MODE_SEQUENCE.
 *
 * Cancel on the stories goes back to the new menu's single-player group,
 * and sets menu_control.mode to MENU_MODE_NEW_MENU. while
 * story_select_work's from_emblems is set, Cancel goes back to the emblem
 * results' menu instead, under MENU_MODE_EMBLEM_RESULTS. from_emblems then
 * stays set. Cancel on a list goes back to the stories */
Bool story_select_step(void);

#endif
