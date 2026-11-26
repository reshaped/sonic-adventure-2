/* STG26.PRS's chunk models, from the disc. its D lies between
 * guard_lens_texture.c and guard_sensor_texture.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/guard_art.h"

static Sint16 lens_polygons[25] = {
#include "stg26/guard_art/lens_polygons.inc"
};
static Sint32 lens_vertices[19] = {
#include "stg26/guard_art/lens_vertices.inc"
};
static NJS_CNK_MODEL lens_model = {
#include "stg26/guard_art/lens_model.inc"
};
NJS_CNK_OBJECT quarters_guard_lens_object = {
#include "stg26/guard_art/quarters_guard_lens_object.inc"
};
