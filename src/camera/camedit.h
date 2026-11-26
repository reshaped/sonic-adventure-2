/* the camera editor: the player who drives it, and the calls the camera task
 * makes into it */
#ifndef CAMERA_CAMEDIT_H
#define CAMERA_CAMEDIT_H
#include <sg_xpt.h>

/* the player whose pad drives the camera editor, or CAMEDIT_NO_PLAYER.
 * camedit_init sets it to CAMEDIT_NO_PLAYER, and no other code sets it */
#define CAMEDIT_NO_PLAYER (-1)
extern Sint32 camedit_player;

/* the camera editor's calls from the camera task's step and draw. each does
 * nothing */
void camedit_update(void);
void camedit_draw(void);
/* clear camera_editor whole, and set camedit_player to CAMEDIT_NO_PLAYER */
void camedit_init(void);

#endif
