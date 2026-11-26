/* The renderer updates the sea polygon stream with the current texture frame.
 */
#ifndef SHARED_SEA_MESH_H
#define SHARED_SEA_MESH_H

#include <ninja.h>

static Sint16 sea_polygons[677] = {
#include "shared/sea_mesh/sea_polygons.inc"
};

static Sint32 sea_vertices[487] = {
#include "shared/sea_mesh/sea_vertices.inc"
};

static NJS_CNK_MODEL sea_model = {
#include "shared/sea_mesh/sea_model.inc"
};

#endif
