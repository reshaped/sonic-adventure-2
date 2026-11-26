/* the fire pot, FIREPOT in Pyramid Cave's object list: a solid pot with a fire
 * burning on it */
#ifndef STG28_FIREPOT_H
#define STG28_FIREPOT_H
#include <ninja.h>

struct task;

/* the pot's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_firepot_texlists[];

/* the object list's step for FIREPOT. task needs a work. its pos places the
 * pot, and its ang.y turns it. the call sets ang.x and ang.z to 0. scl.x plus
 * 1 scales the pot across, and scl.y plus 1 scales it up.
 *
 * the players collide with a copy of the pot's shape in the collision world,
 * which task->extra holds. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while
 * a player lies near its pos. near is within 10 plus 70.58 times the pot's
 * larger scale.
 *
 * the call also gives task a collision record of two shapes. the fire's hurts
 * players. the pot's pushes the shapes of every group, and none of theirs
 * pushes it. the fire's sound plays while player 0 lies within 100 of a point
 * above the pot. player 0's work must not be null, unchecked */
void cave_firepot_init(struct task *task);

#endif
