/* bursts: particles that show the textures bomb03 to bomb14 in turn */
#ifndef EFFECT_BURST_H
#define EFFECT_BURST_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

/* bomb03 to bomb14, the textures a burst's frame picks */
extern NJS_TEXLIST burst_texlist;
/* null, or a function a stage's program sets. each call below calls it for
 * each burst it makes. it passes the call's pos, spd and scale, and 0.0F */
extern void (*burst_hook)(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale,
                          Float unk_3);

/* make a burst at pos, moving at spd, of size scale. each call returns the
 * burst's particle, or null where the heap can't give one. a burst adds itself
 * to what lies behind it. it starts turned at random up to 40 degrees either
 * way. its frame starts a random part of the way into bomb03. each step moves
 * it by spd, then multiplies spd by its friction. it adds its growth to scale
 * and its frame step to frame. it ends once frame reaches 12, past bomb14.
 *
 * a wobbling burst has friction 0.93, growth 0.1 and frame step 0.3. it spins
 * at a random rate, up to 0.9 degrees a step either way. the angle between its
 * quad's edges, a quarter turn in the other bursts, wobbles. it starts at
 * random from 82 up to 98 degrees, in whole degrees. it climbs a degree every
 * 3.75 steps to 98, falls at that rate to 82, and climbs again */
struct particle *burst_create_wobbling(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                       Float scale);
/* a burst with friction 0.93, growth 0.1 and frame step 0.2. it spins at a
 * random rate, up to 0.05 degrees a step either way. it draws nearer by
 * depth_bias, as PARTICLE_DEPTH of effect/particle.h says */
struct particle *burst_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale,
                              Float depth_bias);
/* burst_create's burst, with friction 0.9, growth 1 and frame step 0.7 */
struct particle *burst_create_fast(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                   Float scale, Float depth_bias);

#endif
