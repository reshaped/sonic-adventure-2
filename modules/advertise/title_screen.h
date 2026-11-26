/* the title screen, MENU_MODE_TITLE: the first menu. it waits for Start over
 * the alpha title, and plays a demo after MENU_IDLE_STEPS without it */
#ifndef ADVERTISE_TITLE_SCREEN_H
#define ADVERTISE_TITLE_SCREEN_H
#include <sg_xpt.h>

/* start the title screen, its textures and its music. the caller sets
 * menu_control.mode */
void title_screen_start(void);
/* fade the alpha title out */
void title_screen_hide(void);
/* if which is TITLE_SCREEN_CONTINUE, have the title screen's next step go
 * on to MENU_MODE_NEW_MENU. any other which does nothing */
#define TITLE_SCREEN_CONTINUE 0
void title_screen_continue(int which);
/* the title screen's step under MENU_MODE_TITLE. Start goes on to
 * MENU_MODE_NEW_MENU where the main file has a drive. without one, Start
 * goes on to MENU_MODE_FILE_SELECT where a socket holds a card. it shows the
 * card warning otherwise. a Start while pad_reset_pressed returns 1
 * does nothing. while the warning shows, a card found goes on to
 * MENU_MODE_FILE_SELECT. the warning's end goes on to MENU_MODE_NEW_MENU,
 * with the main file's drive SAVE_DRIVE_REFUSED.
 *
 * once more than MENU_IDLE_STEPS steps pass without Start, the title fades
 * out and readies the next demo. the step returns TRUE once it has readied
 * one, and FALSE otherwise */
Bool title_screen_step(void);

#endif
