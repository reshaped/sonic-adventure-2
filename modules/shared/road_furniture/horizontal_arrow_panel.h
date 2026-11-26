/* Horizontal arrow panel, shared by Radical Highway and Mission Street. */
#ifndef SHARED_ROAD_FURNITURE_HORIZONTAL_ARROW_PANEL_H
#define SHARED_ROAD_FURNITURE_HORIZONTAL_ARROW_PANEL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME horizontal_arrow_panel_names[2] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_names.inc"
};

static NJS_TEXLIST horizontal_arrow_panel_texlist = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_texlist.inc"
};

static Sint16 horizontal_arrow_panel_part_3_polygons[39] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_3_polygons.inc"
};

static Sint32 horizontal_arrow_panel_part_3_vertices[35] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_3_vertices.inc"
};

static NJS_CNK_MODEL horizontal_arrow_panel_part_3_model = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_3_model.inc"
};

static NJS_CNK_OBJECT horizontal_arrow_panel_part_3 = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_3.inc"
};

static Sint16 horizontal_arrow_panel_part_2_polygons[39] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_2_polygons.inc"
};

static Sint32 horizontal_arrow_panel_part_2_vertices[35] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_2_vertices.inc"
};

static NJS_CNK_MODEL horizontal_arrow_panel_part_2_model = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_2_model.inc"
};

static NJS_CNK_OBJECT horizontal_arrow_panel_part_2 = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_2.inc"
};

static Sint16 horizontal_arrow_panel_part_1_polygons[39] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_1_polygons.inc"
};

static Sint32 horizontal_arrow_panel_part_1_vertices[35] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_1_vertices.inc"
};

static NJS_CNK_MODEL horizontal_arrow_panel_part_1_model = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_1_model.inc"
};

static NJS_CNK_OBJECT horizontal_arrow_panel_part_1 = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_part_1.inc"
};

static Sint16 horizontal_arrow_panel_polygons[221] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_polygons.inc"
};

static Sint32 horizontal_arrow_panel_vertices[211] = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_vertices.inc"
};

static NJS_CNK_MODEL horizontal_arrow_panel_model = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel_model.inc"
};

static NJS_CNK_OBJECT horizontal_arrow_panel = {
#include "shared/road_furniture/horizontal_arrow_panel/horizontal_arrow_panel.inc"
};

#endif
