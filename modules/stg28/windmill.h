/* the windmill, WINDMILL in Pyramid Cave's object list: a base with a rotor
 * beside it. the rotor turns about the base's x axis */
#ifndef STG28_WINDMILL_H
#define STG28_WINDMILL_H
#include <ninja.h>

struct task;

/* the windmill's texture lists for texture_load_shared, the rotor's and the
 * base's, ended by null */
extern NJS_TEXLIST *cave_windmill_texlists[];

/* the object list's step for WINDMILL. task needs a work. its pos places the
 * base, and its ang.y turns it. the rotor stands scl.x along the base's x
 * axis, at a random angle. the low 16 bits of ang.z are the Angle the rotor
 * turns a step. the call moves them to the work's timer, and sets ang.z to 0.
 *
 * the players collide with a copy of the rotor's shape in the collision
 * world, which task->extra holds. world_pool_alloc lends the copy, and the
 * call writes through null where the pool has none. the copy collides only
 * while a player lies within 88.74 of its pos.
 *
 * the call also gives task a collision record of three shapes. a thin box
 * runs 200 along the base's x axis through pos, with a sphere at each end.
 * they push the shapes of players, enemies and objects, and none of theirs
 * pushes them */
void cave_windmill_init(struct task *task);

#endif
