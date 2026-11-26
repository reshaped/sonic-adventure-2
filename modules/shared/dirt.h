/* dirt: particles of earth, thrown up and left to fall */
#ifndef SHARED_DIRT_H
#define SHARED_DIRT_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

/* the particles' texture list, which names one texture, tsuchi_yama */
extern NJS_TEXLIST dirt_texlist;

/* throw a piece of dirt from pos at speed spd and size scale. it returns the
 * piece's particle, or null where the heap can't give one.
 *
 * the piece lasts 49 to 61 steps. each step multiplies its speed by 0.98,
 * then adds 0.12 down y. its scale falls by 0.001 a step. it starts at a
 * random angle, and turns at a random rate of under 0.4 degrees a step */
struct particle *dirt_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale);

#endif
