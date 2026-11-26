/* the congratulations: the messages for an award, which the emblem screen
 * shows after the emblem */
#ifndef SHARED_CONGRATULATIONS_H
#define SHARED_CONGRATULATIONS_H

/* start the congratulations for game_state.unlock_id's award, which must be
 * 1 to 18. In the port the messages can run without the banner art. If
 * task creation fails, the caller regains control of the emblem screen.
 *
 * the task shows each of the award's messages for 300 steps, then fades out
 * and ends. Its end releases acquired textures and clears emblem_screen_open.
 * awards 6 to 18 play a voice with each message. they also show the title
 * over a scrolling rainbow, but award 18 doesn't on STAGE_CHAO */
void congratulations_start(void);

#endif
