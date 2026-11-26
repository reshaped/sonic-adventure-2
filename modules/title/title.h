/* TITLE.PRS's init, exit and step, which its head names. the program shows
 * the title screen and its test menu. under EDITION_TAIKEN it shows the
 * trial's title and the screens before it instead. no code of the main
 * executable names TITLE.PRS */
#ifndef TITLE_TITLE_H
#define TITLE_TITLE_H

/* make each frame last one vertical blank and take one step. start the
 * tasks, load the player sounds and empty the item stocks. game_state's
 * four_cameras and single_view end at 0.
 *
 * the first screen is the title screen. under EDITION_TAIKEN, game_state
 * picks the screens instead, which end on the trial's title. the first of
 * these that it holds picks them:
 * - card_warning CARD_WARNING_ALERT: the alert, SEGA's and Sonic Team's
 *   logos, the Sofdec logo;
 * - trial_screen TRIAL_SCREEN_TITLE: the trial's title alone;
 * - trial_screen TRIAL_SCREEN_COMING_SOON: the coming soon screen, the
 *   anniversary logo, SEGA's and Sonic Team's logos;
 * - next_demo even: SEGA's and Sonic Team's logos;
 * - next_demo odd: the trial's title alone.
 * trial_screen then ends at 0 */
void title_init(void);
/* do nothing. title_step has released each screen's textures and destroyed its
 * tasks by the time it returns OVERLAY_DONE or OVERLAY_RESET */
void title_exit(void);
/* the program's step. a screen's first step starts it, and each step after
 * that runs its tasks a frame. once the screen ends, the next step picks the
 * screen after it.
 *
 * the step that ends the title screen's fade out, or the trial title's,
 * returns OVERLAY_DONE. so does each step after it. a step that runs the
 * tasks returns OVERLAY_RESET where pad_reset_pressed returns 1. on the coming
 * soon screen and the anniversary logo, a reset goes on to the trial's title
 * instead. any other step returns OVERLAY_RUNNING.
 *
 * by OVERLAY_DONE, game_state.next_mode is GAME_MODE_DEMO, GAME_MODE_STAGE,
 * GAME_MODE_EVENT or GAME_MODE_SEQUENCE. the title's choices change more of
 * game_state:
 * - an attract demo sets the stage, the characters, two_players and
 *   split_screen. it sets demo to DEMO_ON, and steps next_demo on;
 * - the trial's attract demo also clears every game flag. its second starts
 *   SEQUENCE_TAIKEN_EVENT;
 * - the test menu's games and the demos it records set the stage, the
 *   characters, two_players and split_screen;
 * - each of the test menu's demos sets demo, and each of its plays starts its
 *   sequence;
 * - the trial title's Start starts SEQUENCE_TAIKEN.
 * the test menu's X steps game_state.mission. its Y sets or clears the game
 * flags of every upgrade but UPGRADE_UNK_15.
 *
 * starting the title screen or the trial's title sets game_state.demo to 0
 * and resets the lives. where card_warning is CARD_WARNING_ALERT, the step
 * after Sonic Team's logo picks the Sofdec logo. that step sets card_warning
 * to 0 */
int title_step(void);

#endif
