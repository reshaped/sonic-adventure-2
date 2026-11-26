#ifndef SHARED_METEOR_SHAPES_METEOR_ROCK_2_H
#define SHARED_METEOR_SHAPES_METEOR_ROCK_2_H

/* The original Ninja polygons, vertices and model bounds. */
#include <ninja.h>

static Sint16 meteor_rock_2_polygons[237] = {
#include "shared/meteor_shapes/meteor_rock_2/meteor_rock_2_polygons.inc"
};

static Sint32 meteor_rock_2_vertices[135] = {
#include "shared/meteor_shapes/meteor_rock_2/meteor_rock_2_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_2_mesh = {
#include "shared/meteor_shapes/meteor_rock_2/meteor_rock_2_mesh.inc"
};

#endif
