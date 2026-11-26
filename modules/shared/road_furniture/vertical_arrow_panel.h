/* Vertical arrow panel, shared by Radical Highway and Mission Street. */
#ifndef SHARED_ROAD_FURNITURE_VERTICAL_ARROW_PANEL_H
#define SHARED_ROAD_FURNITURE_VERTICAL_ARROW_PANEL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME vertical_arrow_panel_names[2] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_names.inc"
};

static NJS_TEXLIST vertical_arrow_panel_texlist = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_texlist.inc"
};

static Sint16 vertical_arrow_panel_part_3_polygons[25] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_3_polygons.inc"
};

static Sint32 vertical_arrow_panel_part_3_vertices[19] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_3_vertices.inc"
};

static NJS_CNK_MODEL vertical_arrow_panel_part_3_model = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_3_model.inc"
};

static NJS_CNK_OBJECT vertical_arrow_panel_part_3 = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_3.inc"
};

static Sint16 vertical_arrow_panel_part_2_polygons[25] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_2_polygons.inc"
};

static Sint32 vertical_arrow_panel_part_2_vertices[19] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_2_vertices.inc"
};

static NJS_CNK_MODEL vertical_arrow_panel_part_2_model = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_2_model.inc"
};

static NJS_CNK_OBJECT vertical_arrow_panel_part_2 = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_2.inc"
};

static Sint16 vertical_arrow_panel_part_1_polygons[25] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_1_polygons.inc"
};

static Sint32 vertical_arrow_panel_part_1_vertices[19] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_1_vertices.inc"
};

static NJS_CNK_MODEL vertical_arrow_panel_part_1_model = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_1_model.inc"
};

static NJS_CNK_OBJECT vertical_arrow_panel_part_1 = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_part_1.inc"
};

static Sint16 vertical_arrow_panel_polygons[117] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_polygons.inc"
};

static Sint32 vertical_arrow_panel_vertices[107] = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_vertices.inc"
};

static NJS_CNK_MODEL vertical_arrow_panel_model = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel_model.inc"
};

static NJS_CNK_OBJECT vertical_arrow_panel = {
#include "shared/road_furniture/vertical_arrow_panel/vertical_arrow_panel.inc"
};

#endif
