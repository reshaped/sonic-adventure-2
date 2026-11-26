/* STG26.PRS's chunk models, from the disc. its D lies between eye_textures.c
 * and eye_beam_textures.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/guard_art.h"

static Sint16 eye_polygons[469] = {
#include "stg26/patrol_eye/eye_polygons.inc"
};
static Sint32 eye_vertices[339] = {
#include "stg26/patrol_eye/eye_vertices.inc"
};
NJS_CNK_MODEL guard_eye_model = {
#include "stg26/patrol_eye/guard_eye_model.inc"
};
NJS_CNK_OBJECT guard_eye_object = {
#include "stg26/patrol_eye/guard_eye_object.inc"
};
