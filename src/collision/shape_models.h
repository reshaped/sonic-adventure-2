#ifndef COLLISION_SHAPE_MODELS_H
#define COLLISION_SHAPE_MODELS_H

#include <ninja.h>

/* a sphere, an upper hemisphere and an open tube, each of radius 10 about
 * its origin. the tube runs from 10 below the origin to 10 above. no code
 * draws them */
extern NJS_CNK_OBJECT collision_sphere_object;
extern NJS_CNK_OBJECT collision_hemisphere_object;
extern NJS_CNK_OBJECT collision_tube_object;

#endif
