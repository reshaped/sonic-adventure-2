/* STG26.PRS's chunk models, from the disc. its D lies between
 * guard_exhaust_tex.c and guard_patrol.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/guard_art.h"

static Sint16 exhaust_polygons[27] = {
#include "stg26/patrol_exhaust/exhaust_polygons.inc"
};
static Sint32 exhaust_vertices[19] = {
#include "stg26/patrol_exhaust/exhaust_vertices.inc"
};
static NJS_CNK_MODEL exhaust_model = {
#include "stg26/patrol_exhaust/exhaust_model.inc"
};
NJS_CNK_OBJECT quarters_guard_exhaust_object = {
#include "stg26/patrol_exhaust/quarters_guard_exhaust_object.inc"
};
