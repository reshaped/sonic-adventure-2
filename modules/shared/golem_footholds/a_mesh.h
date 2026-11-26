#ifndef SHARED_GOLEM_FOOTHOLDS_A_MESH_H
#define SHARED_GOLEM_FOOTHOLDS_A_MESH_H

#include <ninja.h>

static Sint16 golem_foothold_a_polygons[699] = {
#include "shared/golem_footholds/a_mesh/golem_foothold_a_polygons.inc"
};

static Sint32 golem_foothold_a_vertices[555] = {
#include "shared/golem_footholds/a_mesh/golem_foothold_a_vertices.inc"
};

#endif
