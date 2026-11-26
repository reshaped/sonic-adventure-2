/* WARPCOLI in Egg Quarters' object list: an unseen cylinder that takes
 * player 0 elsewhere */
#ifndef STG26_WARP_H
#define STG26_WARP_H
struct task;

/* the object list's step for WARPCOLI. task needs a work. its pos places the
 * cylinder, and scl is where the warp takes player 0 and camera 0. ang.z is
 * the yaw player 0 takes there, as player_set_angles takes one. the call
 * moves ang.z to the work's timer, and sets ang.z to 0.
 *
 * a player's touch fades the screen out. the warp then moves player 0 and
 * camera 0 there, and the screen fades back in. pause and the idle fade stay
 * off from the touch until the warp is done. the warp then waits for another
 * touch */
void quarters_warp_init(struct task *task);

#endif
