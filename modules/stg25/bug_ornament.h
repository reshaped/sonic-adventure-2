/* the bug, BUG in Death Chamber's object list: a figure of a bug, of six
 * kinds. the players bump into it */
#ifndef STG25_BUG_ORNAMENT_H
#define STG25_BUG_ORNAMENT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the bug's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_bug_ornament_texlists[];

/* compile each kind's model at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the bugs
 * draw the builds once one call has compiled all six. while objects_fog_off
 * isn't 0, the call turns the fog off for the builds and on after them */
Uint8 *chamber_bug_ornament_build(Uint8 *buf);

/* the object list's step for BUG. task needs a work. its pos places the bug,
 * and its ang turns it by njRotateZ, njRotateX and njRotateY. scl.x plus 1
 * scales the bug. the whole part of |scl.z| modulo 6 picks its kind. kinds 0
 * to 2 are red, green and blue, and kinds 3 to 5 the same colors dark. while
 * the whole part of scl.y is odd, the bug takes no part in collisions.
 *
 * the call gives task a collision record of one cylinder. the cylinder pushes
 * players, enemies and objects, and resists their push. it scales with the
 * bug, but for its offset. the heap must have room for the record,
 * unchecked */
void chamber_bug_ornament_init(struct task *task);

#endif
