/* fences 1 to 3 of FRSTAGE, the platform of Final Rush and Final Chase, and
 * the platform's solid. each fence has a model and a solid of its own.
 * frstage.h declares fence 0 */
#ifndef SHARED_ORBITAL_PLATFORM_H
#define SHARED_ORBITAL_PLATFORM_H

#include <ninja.h>

extern NJS_CNK_OBJECT frstage_fence_1;
extern NJS_CNK_OBJECT frstage_fence_2;
extern NJS_CNK_OBJECT frstage_fence_3;
extern NJS_OBJECT frstage_fence_collision_1;
extern NJS_OBJECT frstage_fence_collision_2;
extern NJS_OBJECT frstage_fence_collision_3;
extern NJS_OBJECT frstage_collision;

#endif
