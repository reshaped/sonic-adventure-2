/* STG23.PRS's texture lists and chunk models, from the disc. its D lies
 * between swing.c and plant.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/scenery.h"
#include "stg23/scenery_textures.h"

static NJS_TEXNAME hidden_base_small_plant_textures[1] = {
#include "stg23/hidden_base_small_plant/hidden_base_small_plant_textures.inc"
};
NJS_TEXLIST base_small_plant_texlist = {
#include "stg23/hidden_base_small_plant/base_small_plant_texlist.inc"
};
static Sint16 base_small_plant_polygons[127] = {
#include "stg23/hidden_base_small_plant/base_small_plant_polygons.inc"
};
static Sint32 base_small_plant_vertices[147] = {
#include "stg23/hidden_base_small_plant/base_small_plant_vertices.inc"
};
static NJS_CNK_MODEL base_small_plant_model = {
#include "stg23/hidden_base_small_plant/base_small_plant_model.inc"
};
NJS_CNK_OBJECT base_small_plant = {
#include "stg23/hidden_base_small_plant/base_small_plant.inc"
};
static NJS_TEXNAME hidden_base_large_plant_textures[1] = {
#include "stg23/hidden_base_small_plant/hidden_base_large_plant_textures.inc"
};
NJS_TEXLIST base_large_plant_texlist = {
#include "stg23/hidden_base_small_plant/base_large_plant_texlist.inc"
};
static Sint16 base_large_plant_polygons[145] = {
#include "stg23/hidden_base_small_plant/base_large_plant_polygons.inc"
};
static Sint32 base_large_plant_vertices[171] = {
#include "stg23/hidden_base_small_plant/base_large_plant_vertices.inc"
};
static NJS_CNK_MODEL base_large_plant_model = {
#include "stg23/hidden_base_small_plant/base_large_plant_model.inc"
};
NJS_CNK_OBJECT base_large_plant = {
#include "stg23/hidden_base_small_plant/base_large_plant.inc"
};
