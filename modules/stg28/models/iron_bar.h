#ifndef STG28_MODELS_IRON_BAR_H
#define STG28_MODELS_IRON_BAR_H

#include <ninja.h>
#include <sg_xpt.h>

enum { IRON_BAR_TEX_DZ_PC_CHAIN8, IRON_BAR_TEX_DZ_PC_IRONBAR_DISH128 };

static Float cave_iron_bar_camera_reach = {
#include "stg28/models/iron_bar/cave_iron_bar_camera_reach.inc"
};

static Float cave_iron_bar_camera_drop = {
#include "stg28/models/iron_bar/cave_iron_bar_camera_drop.inc"
};

static NJS_TEXNAME cave_iron_bar_texture_names[2] = {
#include "stg28/models/iron_bar/cave_iron_bar_texture_names.inc"
};

static NJS_TEXLIST cave_iron_bar_texlist = {
#include "stg28/models/iron_bar/cave_iron_bar_texlist.inc"
};

static Sint16 cave_iron_bar_frame_polygons[167] = {
#include "stg28/models/iron_bar/cave_iron_bar_frame_polygons.inc"
};

static Sint32 cave_iron_bar_frame_vertices[207] = {
#include "stg28/models/iron_bar/cave_iron_bar_frame_vertices.inc"
};

static NJS_CNK_MODEL cave_iron_bar_frame_model = {
#include "stg28/models/iron_bar/cave_iron_bar_frame_model.inc"
};

static NJS_CNK_OBJECT cave_iron_bar_frame_object = {
#include "stg28/models/iron_bar/cave_iron_bar_frame_object.inc"
};

static Sint16 cave_iron_bar_turning_polygons[713] = {
#include "stg28/models/iron_bar/cave_iron_bar_turning_polygons.inc"
};

static Sint32 cave_iron_bar_turning_vertices[543] = {
#include "stg28/models/iron_bar/cave_iron_bar_turning_vertices.inc"
};

static NJS_CNK_MODEL cave_iron_bar_turning_model = {
#include "stg28/models/iron_bar/cave_iron_bar_turning_model.inc"
};

static NJS_CNK_OBJECT cave_iron_bar_turning_object = {
#include "stg28/models/iron_bar/cave_iron_bar_turning_object.inc"
};

static NJS_CNK_OBJECT cave_iron_bar_turning_assembly = {
#include "stg28/models/iron_bar/cave_iron_bar_turning_assembly.inc"
};

static Sint16 cave_iron_bar_chain_polygons[257] = {
#include "stg28/models/iron_bar/cave_iron_bar_chain_polygons.inc"
};

struct cave_iron_bar_chain_vertices {
    Sint32 head[2];
    NJS_POINT3 vertices[44];
    Sint32 end;
};

static struct cave_iron_bar_chain_vertices cave_iron_bar_chain_vertices = {
#include "stg28/models/iron_bar/cave_iron_bar_chain_vertices.inc"
};

static NJS_CNK_MODEL cave_iron_bar_chain_model = {
#include "stg28/models/iron_bar/cave_iron_bar_chain_model.inc"
};

static NJS_CNK_OBJECT cave_iron_bar_chain_object = {
#include "stg28/models/iron_bar/cave_iron_bar_chain_object.inc"
};

static Sint16 cave_iron_bar_arc_polygons[33] = {
#include "stg28/models/iron_bar/cave_iron_bar_arc_polygons.inc"
};

static Sint32 cave_iron_bar_arc_vertices[91] = {
#include "stg28/models/iron_bar/cave_iron_bar_arc_vertices.inc"
};

static NJS_CNK_MODEL cave_iron_bar_arc_model[1] = {
#include "stg28/models/iron_bar/cave_iron_bar_arc_model.inc"
};

static NJS_CNK_OBJECT cave_iron_bar_arc_object = {
#include "stg28/models/iron_bar/cave_iron_bar_arc_object.inc"
};

#endif
