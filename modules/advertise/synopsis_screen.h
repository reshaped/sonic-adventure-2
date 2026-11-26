/* the synopsis's screen, MENU_MODE_SYNOPSIS: synopsis.h's synopsis of the
 * story the story select chose, before the story plays */
#ifndef ADVERTISE_SYNOPSIS_SCREEN_H
#define ADVERTISE_SYNOPSIS_SCREEN_H
#include <sg_xpt.h>

/* start the screen and its textures. the caller sets menu_control.mode */
void synopsis_screen_start(void);
/* the screen's step under MENU_MODE_SYNOPSIS. it shows the synopsis, and
 * scrolls it once the backdrop is in. Cancel goes back to the story select
 * once screen_fade.h's cover is opaque. going back stops the voices, and
 * restores the sequence step menu_start_sequence_at kept. Choose or the
 * scroll's end plays the sequence once the cover is opaque. where
 * synopsis_from_start is TRUE, the sequence plays with no synopsis. it
 * plays once the cover and the story select are still.
 *
 * it returns TRUE once the sequence is to play, and FALSE otherwise */
Bool synopsis_screen_step(void);

#endif
