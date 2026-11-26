/* the alpha title: the title's backdrop behind the menus, and the prompt the
 * title screen blinks over it */
#ifndef ADVERTISE_ALPHA_TITLE_H
#define ADVERTISE_ALPHA_TITLE_H
#include <sg_xpt.h>

#include "system/menu_snapshot.h" /* IWYU pragma: export */
/* the work that menu_works.alpha_title points to */
extern struct alpha_title_work alpha_title_work;

/* create the task, alphaTitleExec, with the work cleared and the backdrop
 * dark. the task's destructor releases the textures */
void alpha_title_create(void);
/* load the textures, or release them, and set loaded to match. neither
 * does anything where loaded matches already */
void alpha_title_load(void);
void alpha_title_release(void);

#endif
