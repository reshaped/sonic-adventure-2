/* the logos OTHERPRINT.PRS shows under OTHER_PRINT_LOGOS */
#ifndef OTHERPRINT_LOGOS_H
#define OTHERPRINT_LOGOS_H

/* load the logos' textures and create their task, execSegalogo. the task
 * shows SEGA's logo and then Sonic Team's. under EDITION_TENTO, the picture
 * whose textures are named tento_alert comes first. the ADX and Sofdec
 * logos then come last. the task fades each in, holds it and fades it out,
 * and then destroys itself. SEGA's logo takes its US form where
 * game_state.us_sega_logo isn't 0. a press of Start on port A while SEGA's
 * logo fades in shows it whole at once. destroying the task once it has
 * stepped releases the textures and sets otherprint_done. the Shinobi heap
 * must have room for the task and its work, unchecked */
void logos_start(void);

#endif
