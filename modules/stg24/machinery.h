/* Eternal Engine's lamps, panel machines and updrafts */
#ifndef STG24_MACHINERY_H
#define STG24_MACHINERY_H

struct task;

/* L_BLUE, a lamp whose lens scrolls by the whole part of scl.y a step.
 * L_ORANGE, a lamp that blinks between dark and lit every 38 steps. each
 * faces ang.y */
void blue_lamp_init(struct task *task);
void orange_lamp_init(struct task *task);

/* MACHINE and DITCH step two copies of a panel back and forth every 11
 * steps. MACHINE's move along x and DITCH's along z. one copy fades out over
 * the first 5 steps of each wait. MACHINE's belt scrolls by the whole part of
 * scl.z a step */
void engine_machine_init(struct task *task);
void engine_ditch_init(struct task *task);

/* the object list's steps for WIND and WIND2, the updraft: an area of rising
 * air. unless placement_try_leave(task) returns TRUE, each gives task the
 * updraft's own step and destructor.
 *
 * task needs a work. pos places the area, and ang.y turns it. WIND's area is a
 * column whose top is pos. it reaches scl.x each way along its own x, scl.z
 * along its z, and down without end. WIND2's is a box that reaches scl each
 * way from pos along its own axes.
 *
 * each step does nothing where placement_try_leave(task) returns TRUE.
 * otherwise it looks at the players in slots 0 and 1. a step renews the
 * updraft of a player whose work's pos lies in the area. it doesn't renew a
 * player rising faster than 1. a renewal gives the player 9 steps left of the
 * updraft. each step a player has steps left uses one. on that step,
 * player_updraft gives the player an acc of 0.4 along y. player 0 must have a
 * work, and a player found in the area must have a motion. the step tests
 * neither */
void engine_updraft_init(struct task *task);
void engine_updraft_box_init(struct task *task);

#endif
