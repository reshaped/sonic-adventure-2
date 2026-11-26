/* the flames: particles of fire that rise and grow through ten textures */
#ifndef SHARED_FLAME_H
#define SHARED_FLAME_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

/* the flames' textures, flame01 to flame10: the FLAME_FRAMES frames a flame
 * ages through */
#define FLAME_FRAMES 10
extern NJS_TEXLIST flame_texlist;

/* make a flame at pos, moving at spd, of size scale, and return its
 * particle. it returns null where the heap can't give one.
 *
 * a flame adds its color to what lies behind it, and lasts 20 steps. it shows
 * flame_texlist's textures in turn, two steps each. each step scales its
 * speed by 0.96, then adds 0.04 to spd.y, and adds 0.22 to its scale. it
 * starts turned up to 40 degrees either way. it turns up to 0.25 degrees a
 * step either way */
struct particle *flame_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale);

#endif
