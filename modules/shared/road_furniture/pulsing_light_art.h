/* Street warning light and its separate glow texture. */
#ifndef SHARED_ROAD_FURNITURE_PULSING_LIGHT_ART_H
#define SHARED_ROAD_FURNITURE_PULSING_LIGHT_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME pulsing_light_textures[1] = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_textures.inc"
};

static NJS_TEXLIST pulsing_light_texlist = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_texlist.inc"
};

static Sint16 pulsing_light_polygons[83] = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_polygons.inc"
};

static Sint32 pulsing_light_vertices[71] = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_vertices.inc"
};

static NJS_CNK_MODEL pulsing_light_model = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_model.inc"
};

static NJS_CNK_OBJECT pulsing_light_object = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_object.inc"
};

static NJS_TEXNAME pulsing_light_glow_textures[1] = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_glow_textures.inc"
};

static NJS_TEXLIST pulsing_light_glow_texlist = {
#include "shared/road_furniture/pulsing_light_art/pulsing_light_glow_texlist.inc"
};

#endif
