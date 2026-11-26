/* The intact FALLASHIBA floor drawn by crumbling_floor.c. */

#include "stg31/prop_art.h"
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 floor_polygons[493] = {
#include "stg31/floor_art/floor_polygons.inc"
};

static Sint32 floor_vertices[327] = {
#include "stg31/floor_art/floor_vertices.inc"
};

static NJS_CNK_MODEL floor_model = {
#include "stg31/floor_art/floor_model.inc"
};

NJS_CNK_OBJECT green_hill_floor_object[1] = {
#include "stg31/floor_art/green_hill_floor_object.inc"
};
