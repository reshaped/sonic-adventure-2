/* Geometry and texture slots for highway prop 5. */
#ifndef STG14_ROAD_PROP_5_H
#define STG14_ROAD_PROP_5_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME highway_prop_5_names[9] = {
#include "stg14/road/prop_5/highway_prop_5_names.inc"
};

static NJS_TEXLIST highway_prop_5_texlist = {
#include "stg14/road/prop_5/highway_prop_5_texlist.inc"
};

static Sint16 highway_prop_5_part_1_polygons[83] = {
#include "stg14/road/prop_5/highway_prop_5_part_1_polygons.inc"
};

static Sint32 highway_prop_5_part_1_vertices[51] = {
#include "stg14/road/prop_5/highway_prop_5_part_1_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_5_part_1_model = {
#include "stg14/road/prop_5/highway_prop_5_part_1_model.inc"
};

static NJS_CNK_OBJECT highway_prop_5_part_1 = {
#include "stg14/road/prop_5/highway_prop_5_part_1.inc"
};

static Sint16 highway_prop_5_polygons[655] = {
#include "stg14/road/prop_5/highway_prop_5_polygons.inc"
};

static Sint32 highway_prop_5_vertices[655] = {
#include "stg14/road/prop_5/highway_prop_5_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_5_model = {
#include "stg14/road/prop_5/highway_prop_5_model.inc"
};

static NJS_CNK_OBJECT highway_prop_5 = {
#include "stg14/road/prop_5/highway_prop_5.inc"
};

#endif
