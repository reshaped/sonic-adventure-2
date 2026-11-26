/* ENDING.PRS's init, exit and step, which its head names. the program shows
 * the staff roll of the hero or the dark story */
#ifndef ENDING_ENDING_H
#define ENDING_ENDING_H

/* make each frame last one vertical blank and take one step. start the
 * tasks, the sound, the memory cards and their screens. start the staff roll
 * with the hero story's photographs where game_flags[FLAG_SEQUENCE] is
 * SEQUENCE_HERO, or the dark story's otherwise. then play t1_sonic.adx
 * for the hero story, or t1_shado.adx for the dark story, by
 * music_play_next. game_state.idle_dim ends at 0 */
void ending_init(void);
/* end what ending_init started, and destroy every task. game_state's
 * idle_dim ends at 1, and sound_paused at 0 */
void ending_exit(void);
/* the program's step, which runs every task a frame. it keeps the fade's
 * color and game_state.idle_dim at 0. once staff_roll_running is FALSE, it
 * moves the sequence to its next step and returns OVERLAY_DONE. otherwise
 * it returns OVERLAY_RESET where pad_reset_pressed returns 1, or
 * OVERLAY_RUNNING */
int ending_step(void);

#endif
