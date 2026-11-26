/* Geometry and texture slots for highway prop 6. */
#ifndef STG14_ROAD_PROP_6_H
#define STG14_ROAD_PROP_6_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME highway_prop_6_names[4] = {
#include "stg14/road/prop_6/highway_prop_6_names.inc"
};

static NJS_TEXLIST highway_prop_6_texlist = {
#include "stg14/road/prop_6/highway_prop_6_texlist.inc"
};

static Sint16 highway_prop_6_part_2_polygons[25] = {
#include "stg14/road/prop_6/highway_prop_6_part_2_polygons.inc"
};

static Sint32 highway_prop_6_part_2_vertices[19] = {
#include "stg14/road/prop_6/highway_prop_6_part_2_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_6_part_2_model = {
#include "stg14/road/prop_6/highway_prop_6_part_2_model.inc"
};

static NJS_CNK_OBJECT highway_prop_6_part_2 = {
#include "stg14/road/prop_6/highway_prop_6_part_2.inc"
};

static Sint16 highway_prop_6_part_1_polygons[25] = {
#include "stg14/road/prop_6/highway_prop_6_part_1_polygons.inc"
};

static Sint32 highway_prop_6_part_1_vertices[19] = {
#include "stg14/road/prop_6/highway_prop_6_part_1_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_6_part_1_model = {
#include "stg14/road/prop_6/highway_prop_6_part_1_model.inc"
};

static NJS_CNK_OBJECT highway_prop_6_part_1 = {
#include "stg14/road/prop_6/highway_prop_6_part_1.inc"
};

#include "shared/road_furniture/warning_sign_body.h"

static NJS_CNK_OBJECT highway_prop_6 = {
#include "stg14/road/prop_6/highway_prop_6.inc"
};

#endif
