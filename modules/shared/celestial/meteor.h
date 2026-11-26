/* Both Meteor Herd and Mad Space use the same rotating meteor. */
#ifndef SHARED_CELESTIAL_METEOR_H
#define SHARED_CELESTIAL_METEOR_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 meteor_polygons[2767] = {
#include "shared/celestial/meteor/meteor_polygons.inc"
};

static Sint32 meteor_vertices[1663] = {
#include "shared/celestial/meteor/meteor_vertices.inc"
};

static NJS_CNK_MODEL sky_meteor_mesh = {
#include "shared/celestial/meteor/sky_meteor_mesh.inc"
};

#endif
