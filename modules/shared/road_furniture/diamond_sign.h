/* Diamond sign, shared by Radical Highway and Mission Street. */
#ifndef SHARED_ROAD_FURNITURE_DIAMOND_SIGN_H
#define SHARED_ROAD_FURNITURE_DIAMOND_SIGN_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME diamond_sign_names[2] = {
#include "shared/road_furniture/diamond_sign/diamond_sign_names.inc"
};

static NJS_TEXLIST diamond_sign_texlist = {
#include "shared/road_furniture/diamond_sign/diamond_sign_texlist.inc"
};

static Sint16 diamond_sign_polygons[83] = {
#include "shared/road_furniture/diamond_sign/diamond_sign_polygons.inc"
};

static Sint32 diamond_sign_vertices[79] = {
#include "shared/road_furniture/diamond_sign/diamond_sign_vertices.inc"
};

static NJS_CNK_MODEL diamond_sign_model = {
#include "shared/road_furniture/diamond_sign/diamond_sign_model.inc"
};

static NJS_CNK_OBJECT diamond_sign = {
#include "shared/road_furniture/diamond_sign/diamond_sign.inc"
};

#endif
