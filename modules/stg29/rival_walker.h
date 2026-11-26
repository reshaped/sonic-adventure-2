/* ControlTWalker, the task that steers player 1's walker against player 0 in
 * the walkers' second fight, STAGE_VERSUS_MECH_2 */
#ifndef STG29_RIVAL_WALKER_H
#define STG29_RIVAL_WALKER_H

/* camera 0's BOSSPOINT mode, which versus_mech_2_step and the end of
 * ControlTWalker's intro each push. it keeps a point RIVAL_LIFT above player
 * 1's walker beyond player 0 */
#define RIVAL_CAMERA_DIST 50.0F
#define RIVAL_CAMERA_HEIGHT 50.0F
#define RIVAL_CAMERA_PART 0.5F
#define RIVAL_CAMERA_LIFT 20.0F
#define RIVAL_LIFT 18.0F

/* start ControlTWalker with its record, and a boss gauge that shows half
 * player 1's gauge. the call loads the title's textures for player 1's kind,
 * which the task's destructor releases. it starts nothing where the heap
 * can't give the task. where the heap can't give the record, it kills the
 * task. slot 1 must hold a player, unchecked.
 *
 * the task steers player 1 through its stick and its action button. it gives
 * player 1 the second and third ring levels. it holds player 1 within the
 * arena, and player 0 no lower than the arena's floor. it does nothing while a
 * mini event runs or player 1's time stands still. it does nothing once the
 * stage is cleared.
 *
 * at STAGE_PLAY the task takes both pads and the pause, and shows player 1's
 * title. camera 0 then takes a BOSSINIT mode. the intro ends once the task has
 * counted more than 480 steps. the count takes in its wait for STAGE_PLAY.
 * player 0's pad and the pause then come back, and camera 0 takes the
 * BOSSPOINT mode above. the task starts over at game_state.steps 0, and in
 * STAGE_FADE_IN once it has left its wait. a start over fills player 1's
 * gauge */
void rival_walker_create(void);

#endif
