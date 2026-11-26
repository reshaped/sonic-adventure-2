#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/arrow.h"
#include "stg26/arrow_textures.h"

static NJS_TEXNAME textures[2] = {
#include "stg26/arrow_green/textures.inc"
};
NJS_TEXLIST arrow_green_texlist = {
#include "stg26/arrow_green/arrow_green_texlist.inc"
};

static Sint16 arrow_green_light_polygons[25] = {
#include "stg26/arrow_green/arrow_green_light_polygons.inc"
};

static Sint32 arrow_green_light_vertices[19] = {
#include "stg26/arrow_green/arrow_green_light_vertices.inc"
};

NJS_CNK_MODEL arrow_green_light = {
#include "stg26/arrow_green/arrow_green_light.inc"
};

static NJS_CNK_OBJECT arrow_green_light_object = {
#include "stg26/arrow_green/arrow_green_light_object.inc"
};

static Sint16 arrow_green_base_polygons[25] = {
#include "stg26/arrow_green/arrow_green_base_polygons.inc"
};

static Sint32 arrow_green_base_vertices[19] = {
#include "stg26/arrow_green/arrow_green_base_vertices.inc"
};

NJS_CNK_MODEL arrow_green_base = {
#include "stg26/arrow_green/arrow_green_base.inc"
};

static NJS_CNK_OBJECT arrow_green_base_object = {
#include "stg26/arrow_green/arrow_green_base_object.inc"
};

static NJS_CNK_OBJECT arrow_green_root = {
#include "stg26/arrow_green/arrow_green_root.inc"
};
