#ifndef SHARED_METEOR_SHAPES_METEOR_ROCK_0_H
#define SHARED_METEOR_SHAPES_METEOR_ROCK_0_H

/* The original Ninja polygons, vertices and model bounds. */
#include <ninja.h>

static Sint16 meteor_rock_0_polygons[311] = {
#include "shared/meteor_shapes/meteor_rock_0/meteor_rock_0_polygons.inc"
};

static Sint32 meteor_rock_0_vertices[195] = {
#include "shared/meteor_shapes/meteor_rock_0/meteor_rock_0_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_0_mesh = {
#include "shared/meteor_shapes/meteor_rock_0/meteor_rock_0_mesh.inc"
};

#endif
