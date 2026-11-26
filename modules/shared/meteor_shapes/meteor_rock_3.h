#ifndef SHARED_METEOR_SHAPES_METEOR_ROCK_3_H
#define SHARED_METEOR_SHAPES_METEOR_ROCK_3_H

/* The original Ninja polygons, vertices and model bounds. */
#include <ninja.h>

static Sint16 meteor_rock_3_polygons[569] = {
#include "shared/meteor_shapes/meteor_rock_3/meteor_rock_3_polygons.inc"
};

static Sint32 meteor_rock_3_vertices[369] = {
#include "shared/meteor_shapes/meteor_rock_3/meteor_rock_3_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_3_mesh = {
#include "shared/meteor_shapes/meteor_rock_3/meteor_rock_3_mesh.inc"
};

#endif
