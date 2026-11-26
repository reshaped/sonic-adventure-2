/* the standing torch, TORCH in Death Chamber's object list: a stick with a
 * blue, green or red flame */
#ifndef STG25_TORCH_H
#define STG25_TORCH_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the torch's texture lists for texture_load_shared, a color's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_torch_texlists[];

/* compile each color's torch at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the
 * torches draw the builds once one call has compiled all three. while
 * objects_fog_off isn't 0, the call turns the fog off for the builds and on
 * after them */
Uint8 *chamber_torch_build(Uint8 *buf);

/* the object list's step for TORCH. task needs a work. the call raises its pos
 * by scl.y, and the torch stands there. a small round shadow lies at the pos
 * the work had. ang.y turns the torch, and the call sets ang.x and ang.z to 0.
 * CHAMBER_COLOR of stg25/chamber_color.h picks the flame's color.
 *
 * the call gives task a collision record of three shapes. the flame's shape
 * hurts players. the stick and a box on its top push the shapes of every group.
 * none of theirs pushes the two. the flame's sound plays while player 0 lies
 * within 100 of a point above the torch */
void chamber_torch_init(struct task *task);

#endif
