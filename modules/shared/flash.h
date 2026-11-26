/* the flash: a particle that shows eight textures in turn, then ends. a
 * FlashManager task makes one and throws sparks from it */
#ifndef SHARED_FLASH_H
#define SHARED_FLASH_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

/* the flash's textures, exp01 to exp08, in the order it shows them */
extern NJS_TEXLIST flash_texlist;

/* make a flash at pos, moving at spd, of size scale, and return its particle.
 * it returns null where the heap can't give one.
 *
 * a flash adds its color to what lies behind it, and draws nearer by
 * depth_bias. it starts at a random angle, and keeps its angle, speed and
 * size. its frame gains 0.4 a step, and it ends as the frame reaches 8 */
struct particle *flash_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale,
                              Float depth_bias);

/* make a FlashManager task at (x, y, z), at task level 3. its first step
 * makes a flash there of size scale, at rest. each of its next 17 steps
 * throws sparks from there, and the task then ends. where the heap can't give
 * the task, the call makes no flash and throws no spark.
 *
 * flash_manager_create throws 3 sparks a step. the other two each throw
 * the count sparks gives. flash_manager_create_sparks_depth also gives the
 * flash depth_bias. a spark's speed is random, in proportion to scale. its x
 * and z each lie from -0.1375 * scale to 0.1375 * scale. its y lies
 * from -0.1 * scale to 0.175 * scale.
 *
 * each call first calls burst_hook of effect/burst.h, where the hook isn't
 * null. it passes the place, a speed of zero, scale and 0.0F */
void flash_manager_create(Float x, Float y, Float z, Float scale);
void flash_manager_create_sparks(Float x, Float y, Float z, Float scale,
                                 Uint8 sparks);
void flash_manager_create_sparks_depth(Float x, Float y, Float z, Float scale,
                                       Uint8 sparks, Float depth_bias);

#endif
