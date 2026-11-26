/* Single-texture orbit mesh. */
#ifndef SHARED_CELESTIAL_ORBIT_64_H
#define SHARED_CELESTIAL_ORBIT_64_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 sky_orbit_64_polygons[833] = {
#include "shared/celestial/orbit_64/sky_orbit_64_polygons.inc"
};

static Sint32 sky_orbit_64_vertices[459] = {
#include "shared/celestial/orbit_64/sky_orbit_64_vertices.inc"
};

static NJS_CNK_MODEL sky_orbit_64_mesh = {
#include "shared/celestial/orbit_64/sky_orbit_64_mesh.inc"
};

#endif
