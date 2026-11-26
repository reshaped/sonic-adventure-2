/* Geometry and texture slots for highway prop 7. */
#ifndef STG14_ROAD_PROP_7_H
#define STG14_ROAD_PROP_7_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME highway_prop_7_names[4] = {
#include "stg14/road/prop_7/highway_prop_7_names.inc"
};

static NJS_TEXLIST highway_prop_7_texlist = {
#include "stg14/road/prop_7/highway_prop_7_texlist.inc"
};

static Sint16 highway_prop_7_part_2_polygons[25] = {
#include "stg14/road/prop_7/highway_prop_7_part_2_polygons.inc"
};

static Sint32 highway_prop_7_part_2_vertices[19] = {
#include "stg14/road/prop_7/highway_prop_7_part_2_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_7_part_2_model = {
#include "stg14/road/prop_7/highway_prop_7_part_2_model.inc"
};

static NJS_CNK_OBJECT highway_prop_7_part_2 = {
#include "stg14/road/prop_7/highway_prop_7_part_2.inc"
};

static Sint16 highway_prop_7_part_1_polygons[25] = {
#include "stg14/road/prop_7/highway_prop_7_part_1_polygons.inc"
};

static Sint32 highway_prop_7_part_1_vertices[19] = {
#include "stg14/road/prop_7/highway_prop_7_part_1_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_7_part_1_model = {
#include "stg14/road/prop_7/highway_prop_7_part_1_model.inc"
};

static NJS_CNK_OBJECT highway_prop_7_part_1 = {
#include "stg14/road/prop_7/highway_prop_7_part_1.inc"
};

static Sint16 highway_prop_7_polygons[357] = {
#include "stg14/road/prop_7/highway_prop_7_polygons.inc"
};

static Sint32 highway_prop_7_vertices[379] = {
#include "stg14/road/prop_7/highway_prop_7_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_7_model = {
#include "stg14/road/prop_7/highway_prop_7_model.inc"
};

static NJS_CNK_OBJECT highway_prop_7 = {
#include "stg14/road/prop_7/highway_prop_7.inc"
};

#endif
