/* ring sparkles: particles of the textures ring01 to ring08 */
#ifndef SHARED_RING_SPARKLE_H
#define SHARED_RING_SPARKLE_H
#include <ninja.h>
#include <sg_xpt.h>

struct particle;

/* the sparkles' textures, ring01 to ring08: the RING_SPARKLE_FRAMES frames a
 * sparkle ages through */
#define RING_SPARKLE_FRAMES 8
extern NJS_TEXLIST ring_sparkle_texlist;

/* make a sparkle at pos, moving at spd, of size scale, and return its
 * particle. each returns null where the heap can't give one.
 *
 * a sparkle starts opaque white, and adds its color to what lies behind it.
 * it slows, falls and grows, and its quad turns and skews. it shows the
 * RING_SPARKLE_FRAMES textures in turn, then stays on the last. each time it
 * would pass the last, it halves its alpha instead. it ends once its alpha
 * is 0.
 *
 * ring_sparkle_create's sparkle adds 0.4 a step to its frame.
 * ring_sparkle_create_long's adds 0.09, so it lasts longer. it also falls
 * faster and grows more slowly */
struct particle *ring_sparkle_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                     Float scale);
struct particle *ring_sparkle_create_long(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                          Float scale);

#endif
