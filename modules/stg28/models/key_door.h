#ifndef STG28_MODELS_KEY_DOOR_H
#define STG28_MODELS_KEY_DOOR_H

#include <ninja.h>
#include <sg_xpt.h>

enum {
    KEY_DOOR_PANEL_BLUE_TEX_DZ_PC_DOORKEY_DOOR128,
    KEY_DOOR_PANEL_BLUE_TEX_KD_PC_AMI2
};

enum {
    KEY_DOOR_PANEL_GREEN_TEX_DZ_PC_DOORKEY_DOOR128,
    KEY_DOOR_PANEL_GREEN_TEX_KD_PC_AMI2
};

enum { KEY_DOOR_FRAME_BLUE_TEX_DZ_PC_DOORKEY_BASEOUT128 };

enum { KEY_DOOR_FRAME_GREEN_TEX_DZ_PC_DOORKEY_BASEOUT128 };

static NJS_TEXNAME cave_key_door_panel_texture_names_blue[2] = {
#include "stg28/models/key_door/cave_key_door_panel_texture_names_blue.inc"
};

static NJS_TEXLIST cave_key_door_panel_texlist_blue = {
#include "stg28/models/key_door/cave_key_door_panel_texlist_blue.inc"
};

static Sint16 cave_key_door_panel_model_blue_polygons[87] = {
#include "stg28/models/key_door/cave_key_door_panel_model_blue_polygons.inc"
};

static Sint32 cave_key_door_panel_model_blue_vertices[83] = {
#include "stg28/models/key_door/cave_key_door_panel_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_key_door_panel_model_blue = {
#include "stg28/models/key_door/cave_key_door_panel_model_blue.inc"
};

static NJS_CNK_OBJECT cave_key_door_panel_blue_object = {
#include "stg28/models/key_door/cave_key_door_panel_blue_object.inc"
};

static NJS_TEXNAME cave_key_door_panel_texture_names_green[2] = {
#include "stg28/models/key_door/cave_key_door_panel_texture_names_green.inc"
};

static NJS_TEXLIST cave_key_door_panel_texlist_green = {
#include "stg28/models/key_door/cave_key_door_panel_texlist_green.inc"
};

static Sint16 cave_key_door_panel_model_green_polygons[87] = {
#include "stg28/models/key_door/cave_key_door_panel_model_green_polygons.inc"
};

static Sint32 cave_key_door_panel_model_green_vertices[83] = {
#include "stg28/models/key_door/cave_key_door_panel_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_key_door_panel_model_green = {
#include "stg28/models/key_door/cave_key_door_panel_model_green.inc"
};

static NJS_CNK_OBJECT cave_key_door_panel_green_object = {
#include "stg28/models/key_door/cave_key_door_panel_green_object.inc"
};

static NJS_TEXNAME cave_key_door_frame_texture_names_blue[1] = {
#include "stg28/models/key_door/cave_key_door_frame_texture_names_blue.inc"
};

static NJS_TEXLIST cave_key_door_frame_texlist_blue = {
#include "stg28/models/key_door/cave_key_door_frame_texlist_blue.inc"
};

static Sint16 cave_key_door_frame_model_blue_polygons[241] = {
#include "stg28/models/key_door/cave_key_door_frame_model_blue_polygons.inc"
};

static Sint32 cave_key_door_frame_model_blue_vertices[195] = {
#include "stg28/models/key_door/cave_key_door_frame_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_key_door_frame_model_blue = {
#include "stg28/models/key_door/cave_key_door_frame_model_blue.inc"
};

static NJS_CNK_OBJECT cave_key_door_frame_blue_object = {
#include "stg28/models/key_door/cave_key_door_frame_blue_object.inc"
};

static NJS_TEXNAME cave_key_door_frame_texture_names_green[1] = {
#include "stg28/models/key_door/cave_key_door_frame_texture_names_green.inc"
};

static NJS_TEXLIST cave_key_door_frame_texlist_green = {
#include "stg28/models/key_door/cave_key_door_frame_texlist_green.inc"
};

static Sint16 cave_key_door_frame_model_green_polygons[241] = {
#include "stg28/models/key_door/cave_key_door_frame_model_green_polygons.inc"
};

static Sint32 cave_key_door_frame_model_green_vertices[195] = {
#include "stg28/models/key_door/cave_key_door_frame_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_key_door_frame_model_green = {
#include "stg28/models/key_door/cave_key_door_frame_model_green.inc"
};

static NJS_CNK_OBJECT cave_key_door_frame_green_object = {
#include "stg28/models/key_door/cave_key_door_frame_green_object.inc"
};

static NJS_MATERIAL cave_key_door_collision_materials[1] = {
#include "stg28/models/key_door/cave_key_door_collision_materials.inc"
};

static Sint16 cave_key_door_collision_indices[21] = {
#include "stg28/models/key_door/cave_key_door_collision_indices.inc"
};

static NJS_MESHSET cave_key_door_collision_meshsets[1] = {
#include "stg28/models/key_door/cave_key_door_collision_meshsets.inc"
};

static NJS_POINT3 cave_key_door_collision_points[8] = {
#include "stg28/models/key_door/cave_key_door_collision_points.inc"
};

static NJS_VECTOR cave_key_door_collision_normals[8] = {
#include "stg28/models/key_door/cave_key_door_collision_normals.inc"
};

static NJS_MODEL cave_key_door_collision_model = {
#include "stg28/models/key_door/cave_key_door_collision_model.inc"
};

static NJS_OBJECT cave_key_door_collision = {
#include "stg28/models/key_door/cave_key_door_collision.inc"
};

#endif
