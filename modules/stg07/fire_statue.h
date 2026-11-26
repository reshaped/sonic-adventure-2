/* the fire statue, FIRZIZ and firziz in Aquatic Mine's object list. it breathes
 * puffs of fire between rests, above the water of stg07/water.h */
#ifndef STG07_FIRE_STATUE_H
#define STG07_FIRE_STATUE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the statue and of its puffs,
 * ended by null */
extern NJS_TEXLIST *fire_statue_texlists[];

/* the object list's step for FIRZIZ and firziz. it does nothing where task
 * leaves at its placement's range. where the heap can't give its record, it
 * does nothing, and the next step tries again. otherwise it gives task the
 * statue's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT. each later step tests the range as the first did.
 *
 * task needs a work, whose pos places the statue and whose ang.y turns it.
 * bit 0 of ang.x keeps the statue from firing. its first fire starts
 * at once. a fire puffs for scl.y + 121 steps once its mouth is open. it puffs
 * once every n + 1 steps, n the low byte of ang.z. a rest lasts scl.y + 121
 * steps once the mouth is shut. a puff starts only with the mouth above the
 * water, and scl.x + 1 scales its speed. each puff is a task at level 3 that
 * hurts players, enemies and objects.
 *
 * a hit on the statue's part pushes it out. the statue then rests until the
 * part slides back in. the statue then fires again only where the mouth clears
 * the water. where it doesn't, the statue never fires again until another hit
 */
void fire_statue_init(struct task *task);

#endif
