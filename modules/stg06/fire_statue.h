/* the fire statue, FIRZIZ in Sky Rail's object list: a statue that breathes
 * puffs of fire between rests */
#ifndef STG06_FIRE_STATUE_H
#define STG06_FIRE_STATUE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the statue and of its puffs,
 * ended by null */
extern NJS_TEXLIST *fire_statue_texlists[];

/* the object list's step for FIRZIZ. it does nothing where task leaves at its
 * placement's range. otherwise the call gives task the statue's step, draw and
 * destructor. it gives task a record in task->motion from syCalloc, which the
 * destructor frees. where syCalloc fails, the call does nothing, and the next
 * step tries again.
 *
 * task needs a work. its pos places the statue, and its ang.y turns it. each
 * later step tests the range as the first did. the statue fires,
 then rests, each for 121 steps plus scl.y. a fire waits for the
 * mouth to open, and each fire but the first waits 30 steps more. a rest waits
 * for the mouth to shut. while it fires, it puffs once every n + 1 steps, n the
 * low byte of ang.z. its fire sounds the stage's sound 3. scl.x + 1 scales the
 * puffs' speed. bit 0 of ang.x keeps the statue from firing, and holds its part
 * out.
 *
 * the call also gives task a collision record of kind COLLISION_OBJECT. its two
 * shapes are the body and a part that hurts players. the heap must have room
 * for the record, unchecked. a hit on the part pushes it out and stops the
 * fire. the part stays out 300 steps, then slides back in a thirtieth of the
 way a
 * step. the statue then fires again.
 each puff is a task of its own at level 3, whose shape
 * hurts players, enemies and objects. a puff lasts up to 20 steps, and ends
 * sooner at its first touch. a time stop holds the statue and its puffs still,
 * but their shapes stay in the collision checks */
void fire_statue_init(struct task *task);

#endif
