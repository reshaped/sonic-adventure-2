/* sparkles: particles of the textures kiran01 to kiran04 */
#ifndef SHARED_SPARKLE_H
#define SHARED_SPARKLE_H
#include <ninja.h>
#include <sg_xpt.h>

struct particle;

extern NJS_TEXLIST sparkle_texlist;

/* make a sparkle at pos, moving at spd, of size scale, and return its
 * particle. it returns null where the heap can't give one.
 *
 * a sparkle starts opaque white, and adds its color to what lies behind it.
 * it slows, falls and grows, and its quad turns and skews. it shows the four
 * textures in turn, then stays on the last. each time it would pass the last,
 * it halves its alpha instead. it ends once its alpha is 0 */
struct particle *sparkle_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale);

#endif
