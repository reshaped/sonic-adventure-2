/* STG25.PRS's chunk models, from the disc. its D lies between
 * machinery_straight.c and mecha.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/machinery_assets.h"

static Sint16 machinery_straight_polygons[877] = {
#include "stg25/death_chamber_straight_machinery/machinery_straight_polygons.inc"
};
static Sint32 machinery_straight_vertices[803] = {
#include "stg25/death_chamber_straight_machinery/machinery_straight_vertices.inc"
};
NJS_CNK_MODEL machinery_straight_model = {
#include "stg25/death_chamber_straight_machinery/machinery_straight_model.inc"
};
static NJS_CNK_OBJECT machinery_straight_object = {
#include "stg25/death_chamber_straight_machinery/machinery_straight_object.inc"
};
