#ifndef STG44_LIGHTS_H
#define STG44_LIGHTS_H

#include <ninja.h>

struct task;

/* the object list's step for SL, a post whose lamp pulses. it does nothing
 * where task leaves at its placement's range. otherwise it gives task the
 * beacon's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT whose one shape is a capsule. where the heap has no room for
 * the record, the beacon goes on without one.
 *
 * task needs a work, whose pos places the beacon and whose three angles turn
 * it. scl.x's whole part, taken unsigned, modulo 3 picks its model. each later
 * step tests the range as the first did */
void mad_space_sl_init(struct task *task);
/* the object list's step for POLE, a post whose light steps through three
 * heights. it does nothing where task leaves at its placement's range.
 * otherwise it gives task the pole's step, draw and destructor, and a collision
 * record of kind COLLISION_OBJECT whose one shape is a capsule. where the heap
 * has no room for the record, the pole goes on without one.
 *
 * task needs a work, whose pos places the pole and whose three angles turn it.
 * each later step tests the range as the first did */
void mad_space_pole_init(struct task *task);
/* the object list's step for LINE, a strip of light that rises and fades. it
 * does nothing where task leaves at its placement's range. otherwise it gives
 * task the line's step, draw and destructor.
 *
 * task needs a work, whose pos places the strip and whose three angles turn
 * it. scl.z is the share of its rise of 150 that the strip has climbed. each
 * step adds scl.x hundredths of the rise, and a share past 1 goes back to 0.
 * each later step tests the range as the first did */
void mad_space_line_init(struct task *task);
extern NJS_TEXLIST mad_space_sl_first_texlist;
extern NJS_TEXLIST mad_space_sl_second_texlist;
extern NJS_TEXLIST mad_space_sl_third_texlist;
extern NJS_TEXLIST mad_space_pole_texlist;
extern NJS_TEXLIST mad_space_line_texlist;
/* texture lists for texture_load_shared, each ended by null */
extern NJS_TEXLIST *mad_space_sl_textures[];
extern NJS_TEXLIST *mad_space_pole_textures[];
extern NJS_TEXLIST *mad_space_line_textures[];

#endif
