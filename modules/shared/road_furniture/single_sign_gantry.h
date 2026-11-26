/* Single sign gantry, shared by Radical Highway and Mission Street. */
#ifndef SHARED_ROAD_FURNITURE_SINGLE_SIGN_GANTRY_H
#define SHARED_ROAD_FURNITURE_SINGLE_SIGN_GANTRY_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME single_sign_gantry_names[7] = {
#include "shared/road_furniture/single_sign_gantry/single_sign_gantry_names.inc"
};

static NJS_TEXLIST single_sign_gantry_texlist = {
#include "shared/road_furniture/single_sign_gantry/single_sign_gantry_texlist.inc"
};

static Sint16 single_sign_gantry_polygons[671] = {
#include "shared/road_furniture/single_sign_gantry/single_sign_gantry_polygons.inc"
};

static Sint32 single_sign_gantry_vertices[607] = {
#include "shared/road_furniture/single_sign_gantry/single_sign_gantry_vertices.inc"
};

static NJS_CNK_MODEL single_sign_gantry_model = {
#include "shared/road_furniture/single_sign_gantry/single_sign_gantry_model.inc"
};

static NJS_CNK_OBJECT single_sign_gantry = {
#include "shared/road_furniture/single_sign_gantry/single_sign_gantry.inc"
};

#endif
