/* sprite sparks: sparks that fly, fall and bounce, each drawn as a trail of
 * sprites. one task, SpSparkExec, moves and draws them all */
#ifndef SHARED_SPARK_SPRITE_H
#define SHARED_SPARK_SPRITE_H
#include <ninja.h>

/* the sprites' texture list, which names one texture, p_exp */
extern NJS_TEXLIST spark_sprite_texlist;

/* throw a spark from pos. it flies at spd plus a random part, up to spread
 * either way along each axis. it slows, falls, and bounces up off the height
 * floor_y. it lasts 80 steps.
 *
 * the sparks share one task at task level 2, which ends once none is left. they
 * also share a ring buffer with room for 254. a spark thrown while 254 fly
 * drops the oldest. where the heap can't give the task or the buffer, the call
 * throws no spark */
void spark_sprite_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float spread,
                         Float floor_y);

#endif
