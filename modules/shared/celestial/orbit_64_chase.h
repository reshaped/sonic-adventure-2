/* Single-texture orbit in the chase stages mesh. */
#ifndef SHARED_CELESTIAL_ORBIT_64_CHASE_H
#define SHARED_CELESTIAL_ORBIT_64_CHASE_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 sky_orbit_64_chase_polygons[833] = {
#include "shared/celestial/orbit_64_chase/sky_orbit_64_chase_polygons.inc"
};

static Sint32 sky_orbit_64_chase_vertices[459] = {
#include "shared/celestial/orbit_64_chase/sky_orbit_64_chase_vertices.inc"
};

static NJS_CNK_MODEL sky_orbit_64_chase_mesh = {
#include "shared/celestial/orbit_64_chase/sky_orbit_64_chase_mesh.inc"
};

#endif
