#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/arrow.h"
#include "stg26/arrow_textures.h"

static NJS_TEXNAME textures[2] = {
#include "stg26/arrow_blue/textures.inc"
};
NJS_TEXLIST arrow_blue_texlist = {
#include "stg26/arrow_blue/arrow_blue_texlist.inc"
};

static Sint16 arrow_blue_light_polygons[25] = {
#include "stg26/arrow_blue/arrow_blue_light_polygons.inc"
};

static Sint32 arrow_blue_light_vertices[19] = {
#include "stg26/arrow_blue/arrow_blue_light_vertices.inc"
};

NJS_CNK_MODEL arrow_blue_light = {
#include "stg26/arrow_blue/arrow_blue_light.inc"
};

static NJS_CNK_OBJECT arrow_blue_light_object = {
#include "stg26/arrow_blue/arrow_blue_light_object.inc"
};

static Sint16 arrow_blue_base_polygons[25] = {
#include "stg26/arrow_blue/arrow_blue_base_polygons.inc"
};

static Sint32 arrow_blue_base_vertices[19] = {
#include "stg26/arrow_blue/arrow_blue_base_vertices.inc"
};

NJS_CNK_MODEL arrow_blue_base = {
#include "stg26/arrow_blue/arrow_blue_base.inc"
};

static NJS_CNK_OBJECT arrow_blue_base_object = {
#include "stg26/arrow_blue/arrow_blue_base_object.inc"
};

static NJS_CNK_OBJECT arrow_blue_root = {
#include "stg26/arrow_blue/arrow_blue_root.inc"
};
