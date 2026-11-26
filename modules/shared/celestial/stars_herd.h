/* The starfield surrounding Meteor Herd and the nearby duel arena. */
#ifndef SHARED_CELESTIAL_STARS_HERD_H
#define SHARED_CELESTIAL_STARS_HERD_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 stars_polygons[4051] = {
#include "shared/celestial/stars_herd/stars_polygons.inc"
};

static Sint32 stars_vertices[3779] = {
#include "shared/celestial/stars_herd/stars_vertices.inc"
};

static NJS_CNK_MODEL sky_stars_mesh = {
#include "shared/celestial/stars_herd/sky_stars_mesh.inc"
};

#endif
