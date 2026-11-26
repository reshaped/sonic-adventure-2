#ifndef SHARED_METEOR_SHAPES_METEOR_FALLING_H
#define SHARED_METEOR_SHAPES_METEOR_FALLING_H

/* The rotating rock thrown by Meteor Herd and Final Chase. */
#include <ninja.h>

static Sint16 meteor_falling_polygons[255] = {
#include "shared/meteor_shapes/meteor_falling/meteor_falling_polygons.inc"
};

static Sint32 meteor_falling_vertices[107] = {
#include "shared/meteor_shapes/meteor_falling/meteor_falling_vertices.inc"
};

static NJS_CNK_MODEL meteor_falling_mesh = {
#include "shared/meteor_shapes/meteor_falling/meteor_falling_mesh.inc"
};

#endif
