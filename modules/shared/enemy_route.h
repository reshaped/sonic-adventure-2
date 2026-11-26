/* the route of an enemy that rides a path, and the colors its route draw
 * blends */
#ifndef SHARED_ENEMY_ROUTE_H
#define SHARED_ENEMY_ROUTE_H
#include <sg_xpt.h>

struct path;

/* an enemy's route, in its task's extra. frames counts the calls of the
 * enemy's route draw */
struct enemy_route {
    Uint8 unk_00[2];
    Sint16 frames;
    Uint8 unk_04[0x34];
    struct path *path;
};

/* the bits of the enemy's ang.y that turn its path about y */
#define PATH_TURN_BITS 0xff00

/* a route triangle's tip stands TIP_LENGTH from its point, and grows by
 * TIP_GROWTH every 1 << TIP_SHIFT frames. the growth counts
 * frames & TIP_FRAMES, so it starts over every TIP_FRAMES + 1 frames */
#define TIP_LENGTH 2.0F
#define TIP_GROWTH 0.3F
#define TIP_FRAMES 0x3f
#define TIP_SHIFT 2

/* BLEND weighs to by phase and from by rest, each out of BLEND_SCALE. rest must
 * be BLEND_SCALE - phase. its result is the blended channel times BLEND_SCALE
 */
#define BLEND(from, to, phase, rest) (((to) * (phase)) + ((from) * (rest)))
#define BLEND_SHIFT 8
#define BLEND_SCALE (1 << BLEND_SHIFT)
/* an opaque ARGB color's alpha, and the places of its channels */
#define BLEND_OPAQUE 0xff000000
#define BLEND_RED 0x00ff0000
#define BLEND_GREEN 0x0000ff00
#define BLEND_BLUE 0x000000ff
/* the opaque color phase / BLEND_SCALE of the way from (r0, g0, b0) to (r1, g1,
 * b1). a channel's BLEND holds it BLEND_SHIFT bits up, in BLEND_GREEN's place.
 * red moves up BLEND_SHIFT bits from there, and blue down. the macro holds only
 * while BLEND_SHIFT is 8, a channel's width */
#define BLEND_COLOR(r0, g0, b0, r1, g1, b1, phase, rest)                       \
    (BLEND_OPAQUE |                                                            \
     ((BLEND(r0, r1, phase, rest) << BLEND_SHIFT) & BLEND_RED) |               \
     (BLEND(g0, g1, phase, rest) & BLEND_GREEN) |                              \
     ((BLEND(b0, b1, phase, rest) >> BLEND_SHIFT) & BLEND_BLUE))

#endif
