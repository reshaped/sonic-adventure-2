/* the fire pot, FIREPOT in Death Chamber's object list: a pot with a blue,
 * green or red flame */
#ifndef STG25_FIRE_POT_H
#define STG25_FIRE_POT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the pot's texture lists for texture_load_shared, a color's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_fire_pot_texlists[];

/* compile each color's pot at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the pots
 * draw the builds once one call has compiled all three. while objects_fog_off
 * isn't 0, the call turns the fog off for the builds and on after them */
Uint8 *chamber_fire_pot_build(Uint8 *buf);

/* the object list's step for FIREPOT. task needs a work. its pos places the
 * pot, and its ang.y turns it. the call sets ang.x and ang.z to 0. scl.x plus
 * 1 scales the pot across, and scl.y plus 1 scales it up. CHAMBER_COLOR of
 * stg25/chamber_color.h picks the flame's color.
 *
 * the call gives task a collision record of two cylinders. the pot pushes
 * the shapes of every group, and none of theirs pushes it. the flame above
 * it hurts players. the flame's sound plays while player 0 lies within 100
 * of a point above the pot */
void chamber_fire_pot_init(struct task *task);

#endif
