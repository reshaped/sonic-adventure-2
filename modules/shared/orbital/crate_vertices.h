/* The three rotating crates use the same eight corners and normals. */
#ifndef SHARED_ORBITAL_CRATE_VERTICES_H
#define SHARED_ORBITAL_CRATE_VERTICES_H

#include <sg_xpt.h>

static Sint32 crate_vertices[51] = {
#include "shared/orbital/crate_vertices/crate_vertices.inc"
};

#endif
