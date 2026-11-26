/* Pyramid Cave illuminated blue and green boards. */
#ifndef STG28_MODELS_CAVE_BOARD_H
#define STG28_MODELS_CAVE_BOARD_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_board_blue_texture_names[2] = {
#include "stg28/models/cave_board/cave_board_blue_texture_names.inc"
};

static NJS_TEXLIST cave_board_blue_texlist = {
#include "stg28/models/cave_board/cave_board_blue_texlist.inc"
};

static Sint16 cave_board_blue_glow_polygons[31] = {
#include "stg28/models/cave_board/cave_board_blue_glow_polygons.inc"
};

static Sint32 cave_board_blue_glow_vertices[27] = {
#include "stg28/models/cave_board/cave_board_blue_glow_vertices.inc"
};

static NJS_CNK_MODEL cave_board_blue_glow_model = {
#include "stg28/models/cave_board/cave_board_blue_glow_model.inc"
};

static NJS_CNK_OBJECT cave_board_blue_object_sibling = {
#include "stg28/models/cave_board/cave_board_blue_object_sibling.inc"
};

static Sint16 cave_board_blue_polygons[205] = {
#include "stg28/models/cave_board/cave_board_blue_polygons.inc"
};

static Sint32 cave_board_blue_vertices[151] = {
#include "stg28/models/cave_board/cave_board_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_board_blue_model = {
#include "stg28/models/cave_board/cave_board_blue_model.inc"
};

static NJS_CNK_OBJECT cave_board_blue_object = {
#include "stg28/models/cave_board/cave_board_blue_object.inc"
};

static NJS_CNK_OBJECT cave_board_blue_assembly = {
#include "stg28/models/cave_board/cave_board_blue_assembly.inc"
};

static NJS_TEXNAME cave_board_green_texture_names[2] = {
#include "stg28/models/cave_board/cave_board_green_texture_names.inc"
};

static NJS_TEXLIST cave_board_green_texlist = {
#include "stg28/models/cave_board/cave_board_green_texlist.inc"
};

static Sint16 cave_board_green_glow_polygons[31] = {
#include "stg28/models/cave_board/cave_board_green_glow_polygons.inc"
};

static Sint32 cave_board_green_glow_vertices[27] = {
#include "stg28/models/cave_board/cave_board_green_glow_vertices.inc"
};

static NJS_CNK_MODEL cave_board_green_glow_model = {
#include "stg28/models/cave_board/cave_board_green_glow_model.inc"
};

static NJS_CNK_OBJECT cave_board_green_object_sibling = {
#include "stg28/models/cave_board/cave_board_green_object_sibling.inc"
};

static Sint16 cave_board_green_polygons[211] = {
#include "stg28/models/cave_board/cave_board_green_polygons.inc"
};

static Sint32 cave_board_green_vertices[151] = {
#include "stg28/models/cave_board/cave_board_green_vertices.inc"
};

static NJS_CNK_MODEL cave_board_green_model = {
#include "stg28/models/cave_board/cave_board_green_model.inc"
};

static NJS_CNK_OBJECT cave_board_green_object = {
#include "stg28/models/cave_board/cave_board_green_object.inc"
};

static NJS_CNK_OBJECT cave_board_green_assembly = {
#include "stg28/models/cave_board/cave_board_green_assembly.inc"
};

#endif
