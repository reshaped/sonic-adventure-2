/* Eternal Engine's reactor and cannon: DORYOKURO and MUJINGUN in its object
 * list. the reactor's lid breaks once none of its nine panels is left */
#ifndef STG24_REACTOR_H
#define STG24_REACTOR_H

struct task;

/* a reactor's lid while it stands, or null. the first lid made while this is
 * null takes it. any lid's break nulls it, and so does that lid's destructor.
 * shared/last_step.c reads it, and the turrets' shots end once it is null */
extern struct task *engine_reactor_lid;

/* the object list's step for DORYOKURO, which stg24/eternal_engine.c's
 * manager also gives the reactor it makes. it does nothing where task leaves
 * at its placement's range. where the heap can't give its body's collision
 * copy, it does nothing, and the next step tries again. otherwise it gives
 * task the reactor's step, draw and destructor. it makes the lid, the nine
 * panels and the two turrets as task's children. it leaves out any it can't
 * make.
 *
 * task needs a work. pos places the reactor, and ang.y turns it. a panel
 * breaks at its second hit. once no panel is left, the lid breaks and the
 * turrets stop. until then, the turrets turn toward player 0 and fire */
void reactor_lid_init(struct task *task);
/* the object list's step for MUJINGUN, which shared/cannon_body.h makes. it
 * does nothing where task leaves at its placement's range. otherwise it gives
 * task the cannon's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT.
 *
 * task needs a work. pos places the cannon, and ang.y turns it. scl.z's whole
 * part is the radius of a circle in x and z. its fraction, times 1000, puts the
 * circle's center that far from pos along the turned -x. while player 0 lies
 * in the circle, the cannon turns toward it. it also fires then, at a chance
 * of scl.x times 0.3 percent a step. the init cuts scl.x to 100. a shot leaves
 * along the barrel's -x at scl.y + 0.5 a step */
void engine_cannon_init(struct task *task);

/* the step, destructor and draw of a smoke puff's task */
void reactor_puff_step(struct task *task);
void reactor_puff_destroy(struct task *task);
void reactor_puff_draw(struct task *task);

#endif
