#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/arrow.h"
#include "stg26/arrow_textures.h"

const char arrow_light_texture[12] = {
#include "stg26/arrow_red/arrow_light_texture.inc"
};
const char arrow_base_texture[15] = {
#include "stg26/arrow_red/arrow_base_texture.inc"
};

static NJS_TEXNAME textures[2] = {
#include "stg26/arrow_red/textures.inc"
};
NJS_TEXLIST arrow_red_texlist = {
#include "stg26/arrow_red/arrow_red_texlist.inc"
};

static Sint16 arrow_red_light_polygons[25] = {
#include "stg26/arrow_red/arrow_red_light_polygons.inc"
};

static Sint32 arrow_red_light_vertices[19] = {
#include "stg26/arrow_red/arrow_red_light_vertices.inc"
};

NJS_CNK_MODEL arrow_red_light = {
#include "stg26/arrow_red/arrow_red_light.inc"
};

static NJS_CNK_OBJECT arrow_red_light_object = {
#include "stg26/arrow_red/arrow_red_light_object.inc"
};

static Sint16 arrow_red_base_polygons[25] = {
#include "stg26/arrow_red/arrow_red_base_polygons.inc"
};

static Sint32 arrow_red_base_vertices[19] = {
#include "stg26/arrow_red/arrow_red_base_vertices.inc"
};

NJS_CNK_MODEL arrow_red_base = {
#include "stg26/arrow_red/arrow_red_base.inc"
};

static NJS_CNK_OBJECT arrow_red_base_object = {
#include "stg26/arrow_red/arrow_red_base_object.inc"
};

static NJS_CNK_OBJECT arrow_red_root = {
#include "stg26/arrow_red/arrow_red_root.inc"
};
