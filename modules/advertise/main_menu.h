/* the main menu's panels, the task mainMenuExec, which menus_init creates.
 * it swings each group of panels on to the screen or off it, as
 * main_menu_work asks */
#ifndef ADVERTISE_MAIN_MENU_H
#define ADVERTISE_MAIN_MENU_H

#include "advertise/menus.h"

/* the work, menu_works.work_01 */
extern struct main_menu_work main_menu_work;

/* create the task, mainMenuExec, and clear the whole work, its loaded flag
 * too. every group starts swung out. the task's destructor releases the
 * textures */
void main_menu_create(void);
/* clear the work but its loaded flag, and swing every group out at once */
void main_menu_reset(void);
/* load the textures, the PVM mainMenu, where the work says they aren't
 * loaded */
void main_menu_load(void);
/* release the textures, where the work says they are loaded */
void main_menu_release(void);

#endif
