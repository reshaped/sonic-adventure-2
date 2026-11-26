#ifndef SHARED_METEOR_SHAPES_METEOR_FRAGMENT_H
#define SHARED_METEOR_SHAPES_METEOR_FRAGMENT_H

/* The original Ninja polygons, vertices and model bounds. */
#include <ninja.h>

static Sint16 meteor_fragment_polygons[115] = {
#include "shared/meteor_shapes/meteor_fragment/meteor_fragment_polygons.inc"
};

static Sint32 meteor_fragment_vertices[63] = {
#include "shared/meteor_shapes/meteor_fragment/meteor_fragment_vertices.inc"
};

static NJS_CNK_MODEL meteor_fragment_mesh = {
#include "shared/meteor_shapes/meteor_fragment/meteor_fragment_mesh.inc"
};

#endif
