/* Chevron curve 2, shared by Radical Highway and Mission Street. */
#ifndef SHARED_ROAD_FURNITURE_CHEVRON_CURVE_2_H
#define SHARED_ROAD_FURNITURE_CHEVRON_CURVE_2_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME chevron_curve_2_names[3] = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_names.inc"
};

static NJS_TEXLIST chevron_curve_2_texlist = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_texlist.inc"
};

static Sint16 chevron_curve_2_part_2_polygons[65] = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_2_polygons.inc"
};

static Sint32 chevron_curve_2_part_2_vertices[67] = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_2_vertices.inc"
};

static NJS_CNK_MODEL chevron_curve_2_part_2_model = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_2_model.inc"
};

static NJS_CNK_OBJECT chevron_curve_2_part_2 = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_2.inc"
};

static Sint16 chevron_curve_2_part_1_polygons[51] = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_1_polygons.inc"
};

static Sint32 chevron_curve_2_part_1_vertices[51] = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_1_vertices.inc"
};

static NJS_CNK_MODEL chevron_curve_2_part_1_model = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_1_model.inc"
};

static NJS_CNK_OBJECT chevron_curve_2_part_1 = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_part_1.inc"
};

static Sint16 chevron_curve_2_polygons[473] = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_polygons.inc"
};

static Sint32 chevron_curve_2_vertices[507] = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_vertices.inc"
};

static NJS_CNK_MODEL chevron_curve_2_model = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2_model.inc"
};

static NJS_CNK_OBJECT chevron_curve_2 = {
#include "shared/road_furniture/chevron_curve_2/chevron_curve_2.inc"
};

#endif
