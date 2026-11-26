/* CHAO.PRS's chunk models, from the disc. its D lies between walk_textures.c
 * and walk_sprites.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/walk_art.h"

static Sint16 walk_vmu_model_polygons[267] = {
#include "chao/walk_art/walk_vmu_model_polygons.inc"
};
static Sint32 walk_vmu_model_vertices[315] = {
#include "chao/walk_art/walk_vmu_model_vertices.inc"
};
static NJS_CNK_MODEL walk_vmu_model_model = {
#include "chao/walk_art/walk_vmu_model_model.inc"
};
NJS_CNK_OBJECT walk_vmu_model = {
#include "chao/walk_art/walk_vmu_model.inc"
};
static Sint16 walk_multi_vmu_model_polygons[207] = {
#include "chao/walk_art/walk_multi_vmu_model_polygons.inc"
};
static Sint32 walk_multi_vmu_model_vertices[171] = {
#include "chao/walk_art/walk_multi_vmu_model_vertices.inc"
};
static NJS_CNK_MODEL walk_multi_vmu_model_model = {
#include "chao/walk_art/walk_multi_vmu_model_model.inc"
};
NJS_CNK_OBJECT walk_multi_vmu_model = {
#include "chao/walk_art/walk_multi_vmu_model.inc"
};
