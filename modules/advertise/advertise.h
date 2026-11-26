/* ADVERTISE.PRS's init and exit, which its head names. the program runs the
 * fake title or the menus */
#ifndef ADVERTISE_ADVERTISE_H
#define ADVERTISE_ADVERTISE_H

/* make each frame last one vertical blank and take one step. start the
 * tasks, the sound, the memory cards and their screens. where player 0 has
 * no lives, run lives_reset. then start the fake title or the menus, and
 * make its step the program's. the fake title starts for the values of
 * game_state.edition that game_state.h names for it. while
 * game_state.display_pending isn't 0, the menus start instead. game_state's
 * four_cameras, single_view and chao_race_menu end at 0 */
void advertise_init(void);
/* end what advertise_init started, free the messages and destroy every
 * task. game_state's idle_dim ends at 1, and sound_paused and card_warning
 * at 0 */
void advertise_exit(void);

#endif
