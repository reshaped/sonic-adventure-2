/* STG26.PRS's chunk models, from the disc. its D lies between
 * eye_beam_textures.c and guard_eye.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/guard_art.h"

static Sint16 eye_beam_polygons[39] = {
#include "stg26/patrol_beam/eye_beam_polygons.inc"
};
static Sint32 eye_beam_vertices[35] = {
#include "stg26/patrol_beam/eye_beam_vertices.inc"
};
NJS_CNK_MODEL guard_eye_beam_model = {
#include "stg26/patrol_beam/guard_eye_beam_model.inc"
};
NJS_CNK_OBJECT guard_eye_beam_object = {
#include "stg26/patrol_beam/guard_eye_beam_object.inc"
};
