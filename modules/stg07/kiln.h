/* the kiln, MIKILN in Aquatic Mine's object list. Knuckles's or Rouge's attack
 * swings its door open */
#ifndef STG07_KILN_H
#define STG07_KILN_H
#include <ninja.h>

struct task;

/* the texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *kiln_texlists[];

/* the object list's step for MIKILN. it does nothing where task leaves at its
 * placement's range. where the heap can't give its collision object's copy, it
 * does nothing, and the next step tries again. otherwise it gives task the
 * kiln's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT. it puts the copy in the collision world. each later step
 * tests the range as the first did.
 *
 * task needs a work, whose pos places the kiln and whose ang.y turns it. a hit
 * on the kiln strikes it where Knuckles or Rouge touches it in an attack
 * chain. nine steps after the strike, the door's shape stops colliding and the
 * door swings open */
void kiln_init(struct task *task);

#endif
