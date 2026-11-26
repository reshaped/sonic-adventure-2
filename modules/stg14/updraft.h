/* the updraft, WIND and WIND2 in Radical Highway's object list: an area of
 * rising air. it holds up the players in it */
#ifndef STG14_UPDRAFT_H
#define STG14_UPDRAFT_H

struct task;

/* the object list's steps for WIND and WIND2. unless placement_try_leave(task)
 * returns TRUE, each gives task the updraft's own step and destructor.
 *
 * task needs a work. pos places the area, and ang.y turns it. WIND's area is a
 * column whose top is pos. it reaches scl.x each way along its own x, scl.z
 * along its z, and down without end. WIND2's is a box that reaches scl each
 * way from pos along its own axes.
 *
 * each step does nothing where placement_try_leave(task) returns TRUE.
 * otherwise it looks at the players in slots 0 and 1. it passes over a player
 * whose bit of game_state.time_stopped is set. a step renews the updraft of a
 * player whose work's pos lies in the area. it doesn't renew a player rising
 * faster than 1. a renewal gives the player 9 steps left of the updraft. each
 * step a player has steps left uses one. on that step, player_updraft gives the
 * player an acc of 0.4 along y. player 0 must have a work, and a player found
 * in the area must have a motion. the step tests neither.
 *
 * player 1's pad vibrates on a step that renews its updraft from 0 steps left.
 * player 0's vibrates on each step that renews its updraft while player 1 has
 * 0 steps left */
void highway_updraft_init(struct task *task);
void highway_updraft_box_init(struct task *task);

#endif
