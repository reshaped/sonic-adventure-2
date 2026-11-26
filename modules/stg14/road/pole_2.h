/* Geometry and texture slots for highway pole 2. */
#ifndef STG14_ROAD_POLE_2_H
#define STG14_ROAD_POLE_2_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME highway_pole_2_names[7] = {
#include "stg14/road/pole_2/highway_pole_2_names.inc"
};

static NJS_TEXLIST highway_pole_2_texlist = {
#include "stg14/road/pole_2/highway_pole_2_texlist.inc"
};

static Sint16 highway_pole_2_part_2_polygons[25] = {
#include "stg14/road/pole_2/highway_pole_2_part_2_polygons.inc"
};

static Sint32 highway_pole_2_part_2_vertices[19] = {
#include "stg14/road/pole_2/highway_pole_2_part_2_vertices.inc"
};

static NJS_CNK_MODEL highway_pole_2_part_2_model = {
#include "stg14/road/pole_2/highway_pole_2_part_2_model.inc"
};

static NJS_CNK_OBJECT highway_pole_2_part_2 = {
#include "stg14/road/pole_2/highway_pole_2_part_2.inc"
};

static Sint16 highway_pole_2_part_1_polygons[25] = {
#include "stg14/road/pole_2/highway_pole_2_part_1_polygons.inc"
};

static Sint32 highway_pole_2_part_1_vertices[19] = {
#include "stg14/road/pole_2/highway_pole_2_part_1_vertices.inc"
};

static NJS_CNK_MODEL highway_pole_2_part_1_model = {
#include "stg14/road/pole_2/highway_pole_2_part_1_model.inc"
};

static NJS_CNK_OBJECT highway_pole_2_part_1 = {
#include "stg14/road/pole_2/highway_pole_2_part_1.inc"
};

static Sint16 highway_pole_2_polygons[693] = {
#include "stg14/road/pole_2/highway_pole_2_polygons.inc"
};

static Sint32 highway_pole_2_vertices[675] = {
#include "stg14/road/pole_2/highway_pole_2_vertices.inc"
};

static NJS_CNK_MODEL highway_pole_2_model = {
#include "stg14/road/pole_2/highway_pole_2_model.inc"
};

static NJS_CNK_OBJECT highway_pole_2 = {
#include "stg14/road/pole_2/highway_pole_2.inc"
};

#endif
