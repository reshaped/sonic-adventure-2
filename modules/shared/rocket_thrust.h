/* puffs of thrust: particles of the textures roketthrust01 to roketthrust08 */
#ifndef SHARED_ROCKET_THRUST_H
#define SHARED_ROCKET_THRUST_H
#include <ninja.h>
#include <sg_xpt.h>

struct particle;

/* the textures a puff shows in turn as it ages */
extern NJS_TEXLIST rocket_thrust_texlist;

/* make a puff of thrust at pos, moving at spd, of size scale, and return its
 * particle. each returns null where the heap can't give one.
 *
 * a puff adds its color, opaque white, to what lies behind it. it starts
 * turned up to 10 degrees either way. it spins up to half a degree a step
 * either way, at a rate drawn once. each step slows it, speeds it upward a
 * little and grows it by 0.07. it shows the eight textures in turn from a
 * random point in the first. it ends after the last.
 * rocket_thrust_create's puff shows half a texture a step, and
 * rocket_thrust_settle_create's 0.6 of one.
 *
 * rocket_thrust_settle_create's puff settles on floor, a height. it levels
 * out as it falls within 10 of floor. a step that would take it below floor
 * puts it on floor. there it slides level at the speed it had */
struct particle *rocket_thrust_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                      Float scale);
struct particle *rocket_thrust_settle_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                             Float scale, Float floor);
/* rocket_thrust_settle_create, but the puff shows 0.35 of a texture a step,
 * and grows by 0.2. each step slows it more and speeds it upward more. it
 * levels out from 50 above floor. the system draws it up to a larger size on
 * the screen */
struct particle *rocket_thrust_settle_wide_create(NJS_POINT3 *pos,
                                                  NJS_VECTOR *spd, Float scale,
                                                  Float floor);

#endif
