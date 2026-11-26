/* the tank, TANK in Prison Lane's object list. it stands on a spring with two
 * valves, and bursts once both valves are knocked off */
#ifndef STG09_TANK_H
#define STG09_TANK_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the tank, its pieces and its
 * dust, ended by null */
extern NJS_TEXLIST *tank_texlists[];

/* the object list's step for TANK. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the tank's step, draw
 * and destructor, and a collision record of kind COLLISION_OBJECT. the
 * record's one shape is an upright cylinder. where the heap has no room for
 * it, the tank goes on without one. task gets a child at each valve still on,
 * which an attack can hurt. a valve the heap can't give a task never comes off.
 *
 * task needs a work, whose pos places the tank and whose ang.y turns it. each
 * later step tests the range as the first did. a hit knocks a valve off, and
 * with both off the tank bursts. the burst ends the task, scores 20 points
 * and throws the tank's six pieces. bit n of the placement's state is set
 * where valve n is off. the tank starts with those valves off, and its
 * destructor records the valves that are off then */
void tank_init(struct task *task);

#endif
