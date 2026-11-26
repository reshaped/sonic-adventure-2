/* a seed in the garden: a pickup that the player holds, and a Chao carries
 * and plants */
#ifndef CHAO_SEED_H
#define CHAO_SEED_H
#include <ninja.h>

struct task;
/* create a seed of kind, an index of garden_seed_models, at position.
 * velocity, if not null, starts it moving, and yaw is unused. returns the
 * seed's task, or in the non-matching build null where it can't be made */
struct task *garden_seed_create(int kind, NJS_POINT3 *position, Angle yaw,
                                NJS_VECTOR *velocity);
#endif
