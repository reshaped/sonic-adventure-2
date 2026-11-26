/* the chunk models of STG40.PRS and STG43.PRS, from the disc. its D precedes
 * sky.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg40/sky.h"

static Sint16 sky_stars_polygons[4377] = {
#include "shared/space_starfield/sky_stars_polygons.inc"
};
static Sint32 sky_stars_vertices[4059] = {
#include "shared/space_starfield/sky_stars_vertices.inc"
};
static NJS_CNK_MODEL sky_stars_mesh = {
#include "shared/space_starfield/sky_stars_mesh.inc"
};
NJS_CNK_OBJECT background_1_object = {
#include "shared/space_starfield/background_1_object.inc"
};
