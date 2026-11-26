/* the versus select's panel, the task twoPVSExec, which menus_init creates.
 * it draws the backdrop, the teams' titles and the lists of games and
 * stages, as versus_select_work asks */
#ifndef ADVERTISE_VERSUS_SELECT_DRAW_H
#define ADVERTISE_VERSUS_SELECT_DRAW_H

#include "advertise/menus.h"

/* the work, menu_works.work_04 */
extern struct versus_select_work versus_select_work;

/* create the task, and clear the whole work, its loaded flag too. every
 * part then starts as versus_select_draw_reset leaves it. the task's
 * destructor releases the textures */
void versus_select_draw_create(void);
/* clear the work but its loaded flag, and start every part again hidden.
 * the hero's title starts from the screen's left edge, and the window at
 * the first stage. it then sets the characters of the work's pairs and
 * stage_pairs */
void versus_select_draw_reset(void);
/* load the textures, the PVMs twoPVS and twoPVSBG, where the work says
 * they aren't loaded */
void versus_select_load(void);
/* release the textures, where the work says they are loaded */
void versus_select_release(void);

#endif
