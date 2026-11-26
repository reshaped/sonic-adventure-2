/* the options' panel, the task optionsExec. it fades each part of
 * options_work in and out, and turns the item and theme wheels */
#ifndef ADVERTISE_OPTIONS_DRAW_H
#define ADVERTISE_OPTIONS_DRAW_H

#include "advertise/menus.h"

/* the options' work, menu_works.work_08 */
extern struct options_work options_work;

/* create the panel's task, and clear the whole work, its loaded flag too.
 * every part's fade starts at 0. the task's destructor releases the
 * textures */
void options_draw_create(void);
/* load the textures, the PVMs options and optionThema, where the work says
 * they aren't loaded */
void options_draw_load(void);
/* release the textures, where the work says they are loaded */
void options_draw_release(void);

#endif
