/* the story select's panels, the task storyMainExec, which menus_init
 * creates. it draws the backdrop, each story's panel and the chosen story's
 * list of chapters, as story_select_work asks */
#ifndef ADVERTISE_STORY_SELECT_DRAW_H
#define ADVERTISE_STORY_SELECT_DRAW_H

#include "advertise/menus.h"

/* the work, menu_works.work_03 */
extern struct story_select_work story_select_work;

/* create the task, and clear the whole work, its loaded flag too. the
 * panels then start as story_select_draw_reset leaves them. the task's
 * destructor releases the textures */
void story_select_draw_create(void);
/* clear the work but its loaded flag, and start every part again hidden.
 * each panel starts from the screen's top left corner, and each list at its
 * first chapter */
void story_select_draw_reset(void);
/* load the textures, the PVM storyMain, where the work says they aren't
 * loaded */
void story_select_load(void);
/* release the textures, where the work says they are loaded */
void story_select_release(void);

#endif
