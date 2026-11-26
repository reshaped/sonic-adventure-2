/* Geometry and texture slots for highway prop 9. */
#ifndef STG14_ROAD_PROP_9_H
#define STG14_ROAD_PROP_9_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME highway_prop_9_names[2] = {
#include "stg14/road/prop_9/highway_prop_9_names.inc"
};

static NJS_TEXLIST highway_prop_9_texlist = {
#include "stg14/road/prop_9/highway_prop_9_texlist.inc"
};

static Sint16 highway_prop_9_polygons[581] = {
#include "stg14/road/prop_9/highway_prop_9_polygons.inc"
};

static Sint32 highway_prop_9_vertices[315] = {
#include "stg14/road/prop_9/highway_prop_9_vertices.inc"
};

static NJS_CNK_MODEL highway_prop_9_model = {
#include "stg14/road/prop_9/highway_prop_9_model.inc"
};

static NJS_CNK_OBJECT highway_prop_9 = {
#include "stg14/road/prop_9/highway_prop_9.inc"
};

#endif
