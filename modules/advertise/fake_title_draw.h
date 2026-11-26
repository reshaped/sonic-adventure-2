/* the fake title's draw: a task that fades and draws the backdrop and pages
 * as fake_title.h's works ask */
#ifndef ADVERTISE_FAKE_TITLE_DRAW_H
#define ADVERTISE_FAKE_TITLE_DRAW_H

/* create the task, niseTitleExec, and load fake_title_texlist from
 * FORCUS.PRS once Ninja's free texture memory is gathered. the backdrop,
 * the prompt and every page start hidden and dark, each page at row 0. the
 * task's destructor releases the textures */
void fake_title_draw_create(void);

#endif
