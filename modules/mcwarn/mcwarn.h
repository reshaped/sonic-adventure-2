/* MCWARN.PRS's init, exit and step, which its head names. the program shows
 * the memory card warning that game_state.card_warning names */
#ifndef MCWARN_MCWARN_H
#define MCWARN_MCWARN_H

/* make each frame last one vertical blank and take one step. start the
 * tasks and the sound, and load the player sounds. then pick the warning for
 * game_state.card_warning, and start the memory cards. CARD_WARNING_NO_CARD
 * picks no_card_warning_create's, and CARD_WARNING_NO_ROOM
 * no_room_warning_create's. SAVE_NOT_READY, SAVE_NO_ROOM and SAVE_NOT_OURS
 * pick save_warning_create's. game_state's four_cameras and single_view end
 * at 0.
 *
 * any other card_warning starts no memory cards. the first step then still
 * starts no_card_warning_create's warning, and returns OVERLAY_DONE */
void mcwarn_init(void);
/* end what mcwarn_init started, and destroy every task */
void mcwarn_exit(void);
/* the program's step. the first starts the warning, with the fade set black
 * and the pads enabled. each later step runs every task a frame, and
 * returns OVERLAY_RESET where pad_reset_pressed returns 1. once the
 * warning's task has destroyed itself, the pads are disabled and the screen
 * fades out. the step that ends the fade returns OVERLAY_DONE, and any
 * other OVERLAY_RUNNING */
int mcwarn_step(void);

#endif
