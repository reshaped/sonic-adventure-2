/* Fan stand and the blades rotated by the stage task. */
#ifndef SHARED_ROAD_FURNITURE_FAN_ART_H
#define SHARED_ROAD_FURNITURE_FAN_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_fan_textures[3] = {
#include "shared/road_furniture/fan_art/street_fan_textures.inc"
};

static NJS_TEXLIST street_fan_texlist = {
#include "shared/road_furniture/fan_art/street_fan_texlist.inc"
};

static Sint16 street_fan_blades_polygons[147] = {
#include "shared/road_furniture/fan_art/street_fan_blades_polygons.inc"
};

static Sint32 street_fan_blades_vertices[131] = {
#include "shared/road_furniture/fan_art/street_fan_blades_vertices.inc"
};

static NJS_CNK_MODEL street_fan_blades_mesh = {
#include "shared/road_furniture/fan_art/street_fan_blades_mesh.inc"
};

static NJS_CNK_OBJECT street_fan_blades = {
#include "shared/road_furniture/fan_art/street_fan_blades.inc"
};

static Sint16 street_fan_polygons[209] = {
#include "shared/road_furniture/fan_art/street_fan_polygons.inc"
};

static Sint32 street_fan_vertices[143] = {
#include "shared/road_furniture/fan_art/street_fan_vertices.inc"
};

static NJS_CNK_MODEL street_fan_mesh = {
#include "shared/road_furniture/fan_art/street_fan_mesh.inc"
};

static NJS_CNK_OBJECT street_fan_object = {
#include "shared/road_furniture/fan_art/street_fan_object.inc"
};

#endif
