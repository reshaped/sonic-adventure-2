/* Two-texture orbit mesh. */
#ifndef SHARED_CELESTIAL_ORBIT_128_H
#define SHARED_CELESTIAL_ORBIT_128_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 sky_orbit_128_polygons[835] = {
#include "shared/celestial/orbit_128/sky_orbit_128_polygons.inc"
};

static Sint32 sky_orbit_128_vertices[459] = {
#include "shared/celestial/orbit_128/sky_orbit_128_vertices.inc"
};

static NJS_CNK_MODEL sky_orbit_128_mesh = {
#include "shared/celestial/orbit_128/sky_orbit_128_mesh.inc"
};

#endif
