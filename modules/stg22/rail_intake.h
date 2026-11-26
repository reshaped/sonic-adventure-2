/* the intake at the entrance of each of Crazy Gadget's transport rails */
#ifndef STG22_RAIL_INTAKE_H
#define STG22_RAIL_INTAKE_H

struct task;

/* the intake's draw and step. rail.c makes the intake as a child of a rail's
 * exit, with a gadget_rail_sensor in its extra. once player 0 breaks its
 * cover, the intake pulls player 0 in and boards it on that rail. the cover
 * mends after 180 steps */
void gadget_rail_intake_draw(struct task *task);
void gadget_rail_intake_step(struct task *task);

#endif
