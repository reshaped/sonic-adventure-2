/* the tutorial: MENU_MODE_TUTORIAL's menu of lessons, each a page of lines
 * that a talk window shows. tutorial_panel.h's panel draws it */
#ifndef ADVERTISE_TUTORIAL_H
#define ADVERTISE_TUTORIAL_H
#include <sg_xpt.h>

/* start the menu, its textures, its messages and its music. the caller sets
 * menu_control.mode */
void tutorial_start(void);
/* the menu's step under MENU_MODE_TUTORIAL, which always returns FALSE.
 * Cancel on the list of lessons closes any open message window and frees
 * the messages. once the panel has faded out, the step goes back to the new
 * menu's extras group */
Bool tutorial_step(void);

#endif
