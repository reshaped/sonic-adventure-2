#ifndef SHARED_CANYON_ART_GEOMETRY_BOARD_LOW_H
#define SHARED_CANYON_ART_GEOMETRY_BOARD_LOW_H

#include <sg_xpt.h>

static Sint16 board_low_polygons[195] = {
#include "shared/canyon_art/geometry/board_low/board_low_polygons.inc"
};

static Sint32 board_low_vertices[139] = {
#include "shared/canyon_art/geometry/board_low/board_low_vertices.inc"
};

#endif
