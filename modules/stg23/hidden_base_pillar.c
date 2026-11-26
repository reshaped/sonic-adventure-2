/* STG23.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between staff.c and hidden_base_tower_platform_filenames.c, and
 * its D lies between staff.c and pillar.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/scenery.h"
#include "stg23/scenery_textures.h"

#include "stg23/hidden_base_pillar/text.inc"

static NJS_TEXNAME textures[5] = {
#include "stg23/hidden_base_pillar/textures.inc"
};
NJS_TEXLIST base_pillar_texlist = {
#include "stg23/hidden_base_pillar/base_pillar_texlist.inc"
};
static Sint16 base_pillar_polygons[261] = {
#include "stg23/hidden_base_pillar/base_pillar_polygons.inc"
};
static Sint32 base_pillar_vertices[179] = {
#include "stg23/hidden_base_pillar/base_pillar_vertices.inc"
};
NJS_CNK_MODEL base_pillar_model = {
#include "stg23/hidden_base_pillar/base_pillar_model.inc"
};
static NJS_CNK_OBJECT base_pillar_object = {
#include "stg23/hidden_base_pillar/base_pillar_object.inc"
};
