/* the updraft, WIND and WIND2 in a stage's object list: an area of rising air.
 * it holds up the players in it */
#ifndef SHARED_UPDRAFT_H
#define SHARED_UPDRAFT_H

struct task;

/* the areas, in the work's submode: WIND's column and WIND2's box */
enum updraft_area { AREA_COLUMN, AREA_BOX };

/* the object list's steps for WIND and WIND2. unless placement_try_leave(task)
 * returns TRUE, each gives task the updraft's own step, draw and destructor.
 * the draw draws nothing.
 *
 * task needs a work. pos places the area, and ang.y turns it. WIND's area is a
 * column whose top is 20 above pos. it reaches scl.x each way along its own x,
 * scl.z along its z, and down without end. WIND2's is a box that reaches scl
 * each way from pos along its own axes.
 *
 * each step looks at the players in slots 0 and 1. a step renews the updraft of
 * a player whose work's pos lies in the area. it doesn't renew a player rising
 * faster than 1. a renewal gives the player 9 steps left of the updraft. each
 * step a player has steps left uses one. on that step, player_updraft gives the
 * player an acc of 0.4 along y. player 0 must have a work, unchecked.
 *
 * player 0's pad vibrates on a step that renews its updraft from 0 steps left.
 * player 1's vibrates on each step that renews its updraft while player 0 has
 * 0 steps left */
void updraft_init(struct task *task);
void updraft_box_init(struct task *task);

#endif
