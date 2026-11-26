/* ControlTWalker, the task that steers player 1's walker against player 0 in
 * the first walker duel, STAGE_VERSUS_MECH_1 */
#ifndef STG20_RIVAL_WALKER_H
#define STG20_RIVAL_WALKER_H

/* start ControlTWalker with its record, and a boss gauge that shows half
 * player 1's gauge. where player 1's kind is PLAYER_TAILS_MECH or
 * PLAYER_EGGMAN_MECH, the call loads that walker's title textures. the task's
 * destructor releases them. it starts nothing where the heap can't give the
 * task. where the heap can't give the record, it kills the task. slots 0 and
 * 1 must hold players while the task lives. the call and the task's steps
 * read their records and works untested.
 *
 * the task writes player 1's stick and action flags, and holds player 1
 * within the arena's box. it does nothing while a mini event runs, player 1's
 * time stands still or the stage is cleared.
 *
 * the task starts over at each step where player 0's record holds
 * PLAYER_MISSED, or game_state.steps is 0. it also starts over in the stage
 * modes STAGE_MISS_FADE, STAGE_RESTART_FADE and STAGE_RETRY_FADE. a start
 * over destroys the boss gauge, sets player 1's gauge to 10 and waits for
 * STAGE_PLAY again. a step that finds no boss gauge makes one while player
 * 1's gauge is above 0. a step where either player's record holds
 * PLAYER_MISSED stops the steering until a start over.
 *
 * at STAGE_PLAY the task disables both players' pads and the pause. it shows
 * player 1's title, and calls camera_bossinit_start for camera 0. the intro
 * ends a fixed count of steps after the task's start or start over. the wait
 * for STAGE_PLAY counts among them. its end enables player 0's pad and the
 * pause, and calls camera_bossinit_end for camera 0. after a retry, the end
 * also summons a messenger */
void first_rival_walker_create(void);

#endif
