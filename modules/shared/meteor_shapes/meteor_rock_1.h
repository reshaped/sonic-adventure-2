#ifndef SHARED_METEOR_SHAPES_METEOR_ROCK_1_H
#define SHARED_METEOR_SHAPES_METEOR_ROCK_1_H

/* The original Ninja polygons, vertices and model bounds. */
#include <ninja.h>

static Sint16 meteor_rock_1_polygons[255] = {
#include "shared/meteor_shapes/meteor_rock_1/meteor_rock_1_polygons.inc"
};

static Sint32 meteor_rock_1_vertices[159] = {
#include "shared/meteor_shapes/meteor_rock_1/meteor_rock_1_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_1_mesh = {
#include "shared/meteor_shapes/meteor_rock_1/meteor_rock_1_mesh.inc"
};

#endif
