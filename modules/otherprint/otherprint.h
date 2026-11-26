/* OTHERPRINT.PRS's init, exit and step, which its head names. the program
 * shows the logos or the picture whose textures are named tento_ending */
#ifndef OTHERPRINT_OTHERPRINT_H
#define OTHERPRINT_OTHERPRINT_H
#include <sg_xpt.h>

/* Set when the presentation ends or its assets or task are unavailable.
 * otherprint_init clears it before starting each presentation. */
extern Bool otherprint_done;

/* make each frame last one vertical blank and take one step. start the
 * tasks, the sound, the memory cards and their screens, and set the fade's
 * color to 0. then start the logos where game_state.other_print is
 * OTHER_PRINT_LOGOS, or the picture where it is OTHER_PRINT_ENDING. any
 * other value starts neither, and the step never returns OVERLAY_DONE */
void otherprint_init(void);
/* end what otherprint_init started, and destroy every task.
 * game_state.sound_paused ends at 0 */
void otherprint_exit(void);
/* the program's step, which runs every task a frame. once otherprint_done
 * is TRUE, it returns OVERLAY_DONE. otherwise it returns OVERLAY_RESET where
 * pad_reset_pressed returns 1, or OVERLAY_RUNNING */
int otherprint_step(void);

#endif
