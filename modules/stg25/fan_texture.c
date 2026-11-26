#include <ninja.h>
#include <sg_xpt.h>

#include "shared/machinery_assets.h"
#include "stg25/fan_texture/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg25/fan_texture/textures.inc"
};
NJS_TEXLIST fan_texlist = {
#include "stg25/fan_texture/fan_texlist.inc"
};

static Sint16 fan_polygons[129] = {
#include "stg25/fan_texture/fan_polygons.inc"
};

static Sint32 fan_vertices[103] = {
#include "stg25/fan_texture/fan_vertices.inc"
};

NJS_CNK_MODEL fan_model = {
#include "stg25/fan_texture/fan_model.inc"
};

static NJS_CNK_OBJECT fan_object = {
#include "stg25/fan_texture/fan_object.inc"
};
