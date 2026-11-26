/* STG26.PRS's chunk models, from the disc. its D lies between
 * guard_textures.c and guard_lens_texture.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/guard_patrol.h"

static Sint16 object_polygons[1661] = {
#include "stg26/patrol_body/object_polygons.inc"
};
static Sint32 object_vertices[2127] = {
#include "stg26/patrol_body/object_vertices.inc"
};
static NJS_CNK_MODEL object_model = {
#include "stg26/patrol_body/object_model.inc"
};
NJS_CNK_OBJECT quarters_guard_object = {
#include "stg26/patrol_body/quarters_guard_object.inc"
};
