/* Earth mesh. */
#ifndef SHARED_CELESTIAL_EARTH_H
#define SHARED_CELESTIAL_EARTH_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 sky_earth_polygons[1285] = {
#include "shared/celestial/earth/sky_earth_polygons.inc"
};

static Sint32 sky_earth_vertices[863] = {
#include "shared/celestial/earth/sky_earth_vertices.inc"
};

static NJS_CNK_MODEL sky_earth_mesh = {
#include "shared/celestial/earth/sky_earth_mesh.inc"
};

#endif
