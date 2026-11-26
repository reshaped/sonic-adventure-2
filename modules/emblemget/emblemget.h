/* EMBLEMGET.PRS's init, exit and step, which its head names. the program
 * shows the emblem screen */
#ifndef EMBLEMGET_EMBLEMGET_H
#define EMBLEMGET_EMBLEMGET_H

/* make each frame last one vertical blank and take one step. start the
 * tasks, the sound, the particles, the memory cards and their screens. then
 * load the PVMs efftex_common and objtex_common, and start the emblem screen
 * with the fade set black. game_state's idle_dim ends at 1, and steps at 0 */
void emblemget_init(void);
/* end what emblemget_init started, release the textures it loaded and
 * destroy every task. game_state.sound_paused ends at 0 */
void emblemget_exit(void);
/* the program's step, which runs every task a frame. once
 * emblem_screen_open is 0, it sets game_state.next_mode to GAME_MODE_TO_MENU
 * and returns OVERLAY_DONE. otherwise it draws the fade, and returns
 * OVERLAY_RESET where pad_reset_pressed returns 1, or OVERLAY_RUNNING */
int emblemget_step(void);

#endif
