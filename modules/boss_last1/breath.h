/* the Biolizard's breath, in puffs of the dust texture */
#ifndef BOSS_LAST1_BREATH_H
#define BOSS_LAST1_BREATH_H
#include <ninja.h>

/* send a puff out from near origin along direction's heading. the heading
 * turns at random by up to 6144 either way, of a turn's 65536. only
 * direction's x and z count. the puff slows and rises once it has moved about
 * limit from origin. it is gone after 48 steps. biolizard_black_breath's puff
 * is black and quicker, and turns up to twice as far. where the heap is full,
 * there is no puff */
void biolizard_white_breath(const NJS_POINT3 *origin,
                            const NJS_VECTOR *direction, Float limit);
void biolizard_black_breath(const NJS_POINT3 *origin,
                            const NJS_VECTOR *direction, Float limit);
#endif
