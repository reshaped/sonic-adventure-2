/* the screen fade: a cover over the whole screen. the menus fade it in and
 * out across a change of menu */
#ifndef ADVERTISE_SCREEN_FADE_H
#define ADVERTISE_SCREEN_FADE_H

#include "advertise/menus.h"

/* menu_works.work_07 points here. */
extern struct screen_fade_work screen_fade_work;

/* create the task, fadeManagerExec, with the work cleared and the cover
 * shown and opaque. the task's destructor clears loaded.
 *
 * while shown is 1, the cover fades in to opaque. once shown is 0, it holds
 * its alpha for 30 steps, with fading 0, and then fades out */
void screen_fade_create(void);
/* set loaded, or clear it. the cover has no textures */
void screen_fade_load(void);
void screen_fade_release(void);

#endif
