/* STG23.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between corner.c and mat.c, and its D lies between corner.c and
 * mat.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/scenery.h"
#include "stg23/scenery_textures.h"

#include "stg23/hidden_base_mat/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg23/hidden_base_mat/textures.inc"
};
NJS_TEXLIST base_mat_texlist = {
#include "stg23/hidden_base_mat/base_mat_texlist.inc"
};
static Sint16 base_mat_polygons[25] = {
#include "stg23/hidden_base_mat/base_mat_polygons.inc"
};
static Sint32 base_mat_vertices[19] = {
#include "stg23/hidden_base_mat/base_mat_vertices.inc"
};
NJS_CNK_MODEL base_mat_model = {
#include "stg23/hidden_base_mat/base_mat_model.inc"
};
static NJS_CNK_OBJECT base_mat_object = {
#include "stg23/hidden_base_mat/base_mat_object.inc"
};
