/* the low torch, TORCH2 in Death Chamber's object list: a stand with a blue,
 * green or red flame */
#ifndef STG25_TORCH2_H
#define STG25_TORCH2_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the torch's texture lists for texture_load_shared, a color's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_torch2_texlists[];

/* compile each color's torch at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the
 * torches draw the builds once one call has compiled all three. while
 * objects_fog_off isn't 0, the call turns the fog off for the builds and on
 * after them */
Uint8 *chamber_torch2_build(Uint8 *buf);

/* the object list's step for TORCH2. task needs a work. its pos places the
 * torch, and its ang.y turns it. the call sets ang.x and ang.z to 0. scl.x
 * plus 1 scales the torch. CHAMBER_COLOR of stg25/chamber_color.h picks the
 * flame's color.
 *
 * the call gives task a collision record of two cylinders, which scale with
 * the torch. the stand pushes the shapes of every group, and none of theirs
 * pushes it. the flame ahead of the stand hurts players. the flame's sound
 * plays while player 0 lies within 100 of a point above the torch */
void chamber_torch2_init(struct task *task);

#endif
