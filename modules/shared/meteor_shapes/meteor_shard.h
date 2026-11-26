#ifndef SHARED_METEOR_SHAPES_METEOR_SHARD_H
#define SHARED_METEOR_SHAPES_METEOR_SHARD_H

/* The original Ninja polygons, vertices and model bounds. */
#include <ninja.h>

static Sint16 meteor_shard_polygons[91] = {
#include "shared/meteor_shapes/meteor_shard/meteor_shard_polygons.inc"
};

static Sint32 meteor_shard_vertices[39] = {
#include "shared/meteor_shapes/meteor_shard/meteor_shard_vertices.inc"
};

static NJS_CNK_MODEL meteor_shard_mesh = {
#include "shared/meteor_shapes/meteor_shard/meteor_shard_mesh.inc"
};

#endif
