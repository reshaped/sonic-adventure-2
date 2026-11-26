/* Geometry and texture slots for highway panel. */
#ifndef STG14_ROAD_PANEL_ART_H
#define STG14_ROAD_PANEL_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME highway_panel_names[1] = {
#include "stg14/road/panel_art/highway_panel_names.inc"
};

static NJS_TEXLIST highway_panel_texlist = {
#include "stg14/road/panel_art/highway_panel_texlist.inc"
};

static Sint16 highway_panel_polygons[77] = {
#include "stg14/road/panel_art/highway_panel_polygons.inc"
};

static Sint32 highway_panel_vertices[83] = {
#include "stg14/road/panel_art/highway_panel_vertices.inc"
};

static NJS_CNK_MODEL highway_panel_model = {
#include "stg14/road/panel_art/highway_panel_model.inc"
};

static NJS_CNK_OBJECT highway_panel_object = {
#include "stg14/road/panel_art/highway_panel_object.inc"
};

#endif
