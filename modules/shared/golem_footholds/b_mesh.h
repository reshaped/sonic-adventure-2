#ifndef SHARED_GOLEM_FOOTHOLDS_B_MESH_H
#define SHARED_GOLEM_FOOTHOLDS_B_MESH_H

#include <ninja.h>

static Sint16 golem_foothold_b_polygons[667] = {
#include "shared/golem_footholds/b_mesh/golem_foothold_b_polygons.inc"
};

static Sint32 golem_foothold_b_vertices[555] = {
#include "shared/golem_footholds/b_mesh/golem_foothold_b_vertices.inc"
};

#endif
