#ifndef SHARED_METEOR_SHAPES_METEOR_FIREBALL_H
#define SHARED_METEOR_SHAPES_METEOR_FIREBALL_H

/* The original Ninja polygons, vertices and model bounds. */
#include <ninja.h>

static Sint16 meteor_fireball_polygons[319] = {
#include "shared/meteor_shapes/meteor_fireball/meteor_fireball_polygons.inc"
};

static Sint32 meteor_fireball_vertices[155] = {
#include "shared/meteor_shapes/meteor_fireball/meteor_fireball_vertices.inc"
};

static NJS_CNK_MODEL meteor_fireball_mesh = {
#include "shared/meteor_shapes/meteor_fireball/meteor_fireball_mesh.inc"
};

#endif
