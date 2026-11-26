/* Double sign gantry, shared by Radical Highway and Mission Street. */
#ifndef SHARED_ROAD_FURNITURE_DOUBLE_SIGN_GANTRY_H
#define SHARED_ROAD_FURNITURE_DOUBLE_SIGN_GANTRY_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME double_sign_gantry_names[8] = {
#include "shared/road_furniture/double_sign_gantry/double_sign_gantry_names.inc"
};

static NJS_TEXLIST double_sign_gantry_texlist = {
#include "shared/road_furniture/double_sign_gantry/double_sign_gantry_texlist.inc"
};

static Sint16 double_sign_gantry_polygons[833] = {
#include "shared/road_furniture/double_sign_gantry/double_sign_gantry_polygons.inc"
};

static Sint32 double_sign_gantry_vertices[835] = {
#include "shared/road_furniture/double_sign_gantry/double_sign_gantry_vertices.inc"
};

static NJS_CNK_MODEL double_sign_gantry_model = {
#include "shared/road_furniture/double_sign_gantry/double_sign_gantry_model.inc"
};

static NJS_CNK_OBJECT double_sign_gantry = {
#include "shared/road_furniture/double_sign_gantry/double_sign_gantry.inc"
};

#endif
