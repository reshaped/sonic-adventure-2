/* The outer two sky models have compiled draw lists. The two inner
 * models scroll their own reference UVs at one and two units per step. */
#ifndef STG06_BACKGROUND_LAYERS_H
#define STG06_BACKGROUND_LAYERS_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

static NJS_TEXNAME sky_rail_background_0_textures[3] = {
#include "stg06/background/layers/sky_rail_background_0_textures.inc"
};

static NJS_TEXLIST sky_rail_background_0_texlist = {
#include "stg06/background/layers/sky_rail_background_0_texlist.inc"
};

static Sint16 sky_rail_background_0_polygons[507] = {
#include "stg06/background/layers/sky_rail_background_0_polygons.inc"
};

static Sint32 sky_rail_background_0_vertices[267] = {
#include "stg06/background/layers/sky_rail_background_0_vertices.inc"
};

static NJS_CNK_MODEL sky_rail_background_0_model = {
#include "stg06/background/layers/sky_rail_background_0_model.inc"
};

static NJS_CNK_OBJECT sky_rail_background_0_object = {
#include "stg06/background/layers/sky_rail_background_0_object.inc"
};

static NJS_TEXNAME sky_rail_background_1_textures[1] = {
#include "stg06/background/layers/sky_rail_background_1_textures.inc"
};

static NJS_TEXLIST sky_rail_background_1_texlist = {
#include "stg06/background/layers/sky_rail_background_1_texlist.inc"
};

static Sint16 sky_rail_background_1_polygons[19] = {
#include "stg06/background/layers/sky_rail_background_1_polygons.inc"
};

static Sint32 sky_rail_background_1_vertices[19] = {
#include "stg06/background/layers/sky_rail_background_1_vertices.inc"
};

static NJS_CNK_MODEL sky_rail_background_1_model = {
#include "stg06/background/layers/sky_rail_background_1_model.inc"
};

static NJS_CNK_OBJECT sky_rail_background_1_object = {
#include "stg06/background/layers/sky_rail_background_1_object.inc"
};

static NJS_TEXNAME sky_rail_background_2_textures[1] = {
#include "stg06/background/layers/sky_rail_background_2_textures.inc"
};

static NJS_TEXLIST sky_rail_background_2_texlist = {
#include "stg06/background/layers/sky_rail_background_2_texlist.inc"
};

static Sint16 sky_rail_background_2_polygons[19] = {
#include "stg06/background/layers/sky_rail_background_2_polygons.inc"
};

static Sint32 sky_rail_background_2_vertices[19] = {
#include "stg06/background/layers/sky_rail_background_2_vertices.inc"
};

static NJS_CNK_MODEL sky_rail_background_2_model = {
#include "stg06/background/layers/sky_rail_background_2_model.inc"
};

static NJS_CNK_OBJECT sky_rail_background_2_object = {
#include "stg06/background/layers/sky_rail_background_2_object.inc"
};

static Sint16 sky_rail_background_1_uvs[12] = {
#include "stg06/background/layers/sky_rail_background_1_uvs.inc"
};

static struct texture_anim sky_rail_background_1_anim = {
#include "stg06/background/layers/sky_rail_background_1_anim.inc"
};

static Sint16 sky_rail_background_2_uvs[12] = {
#include "stg06/background/layers/sky_rail_background_2_uvs.inc"
};

static struct texture_anim sky_rail_background_2_anim = {
#include "stg06/background/layers/sky_rail_background_2_anim.inc"
};

static NJS_TEXNAME sky_rail_background_3_textures[1] = {
#include "stg06/background/layers/sky_rail_background_3_textures.inc"
};

static NJS_TEXLIST sky_rail_background_3_texlist = {
#include "stg06/background/layers/sky_rail_background_3_texlist.inc"
};

static Sint16 sky_rail_background_3_polygons[61] = {
#include "stg06/background/layers/sky_rail_background_3_polygons.inc"
};

static Sint32 sky_rail_background_3_vertices[67] = {
#include "stg06/background/layers/sky_rail_background_3_vertices.inc"
};

static NJS_CNK_MODEL sky_rail_background_3_model = {
#include "stg06/background/layers/sky_rail_background_3_model.inc"
};

static NJS_CNK_OBJECT sky_rail_background_3_object = {
#include "stg06/background/layers/sky_rail_background_3_object.inc"
};

static NJS_TEXNAME sky_rail_background_pvm_textures[4] = {
#include "stg06/background/layers/sky_rail_background_pvm_textures.inc"
};

static NJS_TEXLIST sky_rail_background_pvm_texlist = {
#include "stg06/background/layers/sky_rail_background_pvm_texlist.inc"
};

#endif
