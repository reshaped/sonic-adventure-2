/* STG30.PRS's chunk models, from the disc. its D lies between
 * sky_stars_lower.c and sky.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg30/sky.h"

static Sint16 sky_stars_polygons[4377] = {
#include "stg30/final_rush_starfield/sky_stars_polygons.inc"
};
static Sint32 sky_stars_vertices[4059] = {
#include "stg30/final_rush_starfield/sky_stars_vertices.inc"
};
static NJS_CNK_MODEL sky_stars_mesh = {
#include "stg30/final_rush_starfield/sky_stars_mesh.inc"
};
NJS_CNK_OBJECT rush_stars_object = {
#include "stg30/final_rush_starfield/rush_stars_object.inc"
};
