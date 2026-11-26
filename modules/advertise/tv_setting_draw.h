/* the TV setting's panel, the task tvSettingExec, which menus_init creates.
 * it fades the backdrop and the test picture, and swings the choices in and
 * out, as tv_setting_work asks */
#ifndef ADVERTISE_TV_SETTING_DRAW_H
#define ADVERTISE_TV_SETTING_DRAW_H

#include "advertise/menus.h"

/* the work, menu_works.work_09 */
extern struct tv_setting_work tv_setting_work;

/* create the task, and clear the whole work, its loaded flag too. every
 * part starts hidden. the task's destructor releases the textures */
void tv_setting_draw_create(void);
/* load the textures, the PVM tvSetting, where the work says they aren't
 * loaded */
void tv_setting_draw_load(void);
/* release the textures, where the work says they are loaded */
void tv_setting_draw_release(void);

#endif
