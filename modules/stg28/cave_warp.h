/* the warp, WARP in Pyramid Cave's object list: a box whose touch moves
 * player 0 behind a fade */
#ifndef STG28_CAVE_WARP_H
#define STG28_CAVE_WARP_H

struct task;

/* the object list's step for WARP. task needs a work. its pos places the box,
 * and its ang.y turns it. scl is where player 0 comes out, and ang.z the angle
 * it comes out at, about y. the call sets ang.z to 0. the warp draws nothing.
 *
 * the call gives task a collision record of one box, 50 wide and deep and 20
 * high. its center lies 10 below pos. the box pushes no shape of a player, an
 * enemy, an object or a pickup. it resists the push of players, enemies and
 * objects. the heap must have room for the record, unchecked.
 *
 * a warp starts once any player touches the box. it sets game_state's
 * pause_disabled to 1 and its idle_dim to 0, and the screen fades out. 74 steps
 * after the touch's step, player 0 moves to scl, whichever player touched the
 * box. the screen then fades in. player 0 takes PLAYER_REQUEST_RELEASE, and
 * both players take the light at light_camera's index. 41 steps after the move,
 * the warp sets pause_disabled to 0 and idle_dim to 1.
 *
 * each step plays sound 11 of the stage's list at the box. it plays only while
 * player 0 lies within 120. player 0's work must not be null, unchecked */
void cave_warp_init(struct task *task);

#endif
