/* the new menu's screen, MENU_MODE_NEW_MENU: the groups of buttons after the
 * title screen. the other menus start from them */
#ifndef ADVERTISE_NEW_MENU_SCREEN_H
#define ADVERTISE_NEW_MENU_SCREEN_H
#include <sg_xpt.h>

/* the groups new_menu_screen_reopen opens. their numbers aren't new_menu.h's
 * numbers of the groups */
enum new_menu_reopen {
    NEW_MENU_REOPEN_SINGLE,
    NEW_MENU_REOPEN_VERSUS,
    NEW_MENU_REOPEN_EXTRAS,
    NEW_MENU_REOPEN_TOP
};

/* start the screen, its textures and its music, with the alpha title dim.
 * the caller sets menu_control.mode. it refuses each button whose game the
 * save hasn't opened. it also refuses NEW_MENU_TOP_INTERNET if the
 * console's network information isn't found, and NEW_MENU_EXTRAS_RANKING
 * always */
void new_menu_screen_start(void);
/* open group, a NEW_MENU_REOPEN_ value, which lights once it has swung in.
 * the single or versus group needs two buttons to choose, or the top group
 * opens instead. any other value does nothing. call it after
 * new_menu_screen_start, whose first step it replaces */
void new_menu_screen_reopen(int group);
/* the screen's step under MENU_MODE_NEW_MENU. Choose on a button that
 * isn't refused opens its group, or starts its menu and sets
 * menu_control.mode. a single or versus group with fewer than two buttons
 * to choose doesn't open. the menu of the group's first button starts
 * instead. Cancel goes back to the top group, and from the top group to
 * the title screen. while player 1 has no pad, NEW_MENU_TOP_VERSUS is
 * refused.
 *
 * the step returns TRUE once the Chao race or the Internet ends the menus,
 * and FALSE otherwise. the Internet sets menus_leave_game */
Bool new_menu_screen_step(void);
/* restore the menus' snapshot, and start the screen under
 * MENU_MODE_NEW_MENU at the versus group's swing. menus_resume calls it for
 * MENU_RESUME_VERSUS, which the screen sets as it starts the Chao race */
void new_menu_screen_resume(void);

#endif
