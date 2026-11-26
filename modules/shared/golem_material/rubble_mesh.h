/* The same rubble mesh is stored in both Egg Golem attachment banks. */
#ifndef SHARED_GOLEM_MATERIAL_RUBBLE_MESH_H
#define SHARED_GOLEM_MATERIAL_RUBBLE_MESH_H

#include <sg_xpt.h>

static Sint16 rubble_polygons[71] = {
#include "shared/golem_material/rubble_mesh/rubble_polygons.inc"
};

static Sint32 rubble_vertices[45] = {
#include "shared/golem_material/rubble_mesh/rubble_vertices.inc"
};

#endif
