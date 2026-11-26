/* the fire mouth, MOUTH in Death Chamber's object list: a stone face with a
 * flame in its mouth */
#ifndef STG25_FIRE_MOUTH_H
#define STG25_FIRE_MOUTH_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the face's texture lists for texture_load_shared, a color's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_fire_mouth_texlists[];

/* compile each color's face at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the
 * faces draw the builds once one call has compiled all three. while
 * objects_fog_off isn't 0, the call turns the fog off for the builds and on
 * after them */
Uint8 *chamber_fire_mouth_build(Uint8 *buf);

/* the object list's step for MOUTH. task needs a work. its pos places the
 * face, and its ang.y turns it. scl.x plus 1 scales the face across, and
 * scl.y plus 1 scales it up. CHAMBER_COLOR of stg25/chamber_color.h picks
 * the flame's color.
 *
 * the players collide with a copy of the face's shape in the collision
 * world, which task->extra holds. world_pool_alloc lends the copy, and the
 * call writes through null where the pool has none. the copy collides only
 * while a player lies near its pos. near is within 10 plus 31.1 times the
 * face's larger scale.
 *
 * the call also gives task a collision record of one cylinder, the flame,
 * which hurts players. the heap must have room for the record, unchecked.
 * the flame's sound plays once, as the face starts. it plays only where
 * player 0 lies within 100 of a point below the face */
void chamber_fire_mouth_init(struct task *task);

#endif
