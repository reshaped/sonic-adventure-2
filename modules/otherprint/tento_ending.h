/* the picture whose textures are named tento_ending, which OTHERPRINT.PRS
 * shows under OTHER_PRINT_ENDING */
#ifndef OTHERPRINT_TENTO_ENDING_H
#define OTHERPRINT_TENTO_ENDING_H

/* load the picture's textures and create its task, execTentoEnding. the task
 * fades the picture in, holds it, fades it out and destroys itself. once the
 * task has stepped, destroying it releases the textures and sets
 * otherprint_done */
void tento_ending_start(void);

#endif
