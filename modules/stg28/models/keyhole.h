#ifndef STG28_MODELS_KEYHOLE_H
#define STG28_MODELS_KEYHOLE_H

#include <ninja.h>
#include <sg_xpt.h>

enum {
    KEYHOLE_PLATE_BLUE_TEX_DZ_PC_DOORKEY_BASEIN128,
    KEYHOLE_PLATE_BLUE_TEX_DZ_PC_DOORKEY_BASENEON64,
    KEYHOLE_PLATE_BLUE_TEX_DZ_PC_DOORKEY_BASEOUT128
};

enum {
    KEYHOLE_PLATE_GREEN_TEX_DZ_PC_DOORKEY_BASEIN128,
    KEYHOLE_PLATE_GREEN_TEX_DZ_PC_DOORKEY_BASENEON64,
    KEYHOLE_PLATE_GREEN_TEX_DZ_PC_DOORKEY_BASEOUT128
};

enum {
    KEYHOLE_BASE_BLUE_TEX_DZ_PC_DOORKEY_BASEOUT128,
    KEYHOLE_BASE_BLUE_TEX_DZ_PC_DOORKEY_NEONBLUE32,
    KEYHOLE_BASE_BLUE_TEX_DZ_PC_MARUKAGE64
};

enum {
    KEYHOLE_BASE_GREEN_TEX_DZ_PC_DOORKEY_BASEOUT128,
    KEYHOLE_BASE_GREEN_TEX_DZ_PC_DOORKEY_NEONGREEN32,
    KEYHOLE_BASE_GREEN_TEX_DZ_PC_MARUKAGE64
};

enum { KEYHOLE_LAMP_0_TEX_DZ_PC_DOORKEY_BASENEON32 };

enum { KEYHOLE_LAMP_1_TEX_DZ_PC_DOORKEY_BASENEON32 };

enum { KEYHOLE_LAMP_2_TEX_DZ_PC_DOORKEY_BASENEON32 };

static NJS_TEXNAME cave_keyhole_plate_texture_names_blue[3] = {
#include "stg28/models/keyhole/cave_keyhole_plate_texture_names_blue.inc"
};

static NJS_TEXLIST cave_keyhole_plate_texlist_blue = {
#include "stg28/models/keyhole/cave_keyhole_plate_texlist_blue.inc"
};

static Sint16 cave_keyhole_plate_model_blue_polygons[463] = {
#include "stg28/models/keyhole/cave_keyhole_plate_model_blue_polygons.inc"
};

static Sint32 cave_keyhole_plate_model_blue_vertices[227] = {
#include "stg28/models/keyhole/cave_keyhole_plate_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_plate_model_blue = {
#include "stg28/models/keyhole/cave_keyhole_plate_model_blue.inc"
};

static NJS_CNK_OBJECT cave_keyhole_plate_blue_object = {
#include "stg28/models/keyhole/cave_keyhole_plate_blue_object.inc"
};

static Sint16 cave_keyhole_plate_glow_model_blue_polygons[31] = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_model_blue_polygons.inc"
};

static Sint32 cave_keyhole_plate_glow_model_blue_vertices[27] = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_plate_glow_model_blue = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_model_blue.inc"
};

static NJS_CNK_OBJECT cave_keyhole_plate_glow_blue_object = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_blue_object.inc"
};

static NJS_CNK_OBJECT cave_keyhole_plate_glow_blue_assembly = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_blue_assembly.inc"
};

static NJS_TEXNAME cave_keyhole_plate_texture_names_green[3] = {
#include "stg28/models/keyhole/cave_keyhole_plate_texture_names_green.inc"
};

static NJS_TEXLIST cave_keyhole_plate_texlist_green = {
#include "stg28/models/keyhole/cave_keyhole_plate_texlist_green.inc"
};

static Sint16 cave_keyhole_plate_model_green_polygons[449] = {
#include "stg28/models/keyhole/cave_keyhole_plate_model_green_polygons.inc"
};

static Sint32 cave_keyhole_plate_model_green_vertices[227] = {
#include "stg28/models/keyhole/cave_keyhole_plate_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_plate_model_green = {
#include "stg28/models/keyhole/cave_keyhole_plate_model_green.inc"
};

static NJS_CNK_OBJECT cave_keyhole_plate_green_object = {
#include "stg28/models/keyhole/cave_keyhole_plate_green_object.inc"
};

static Sint16 cave_keyhole_plate_glow_model_green_polygons[31] = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_model_green_polygons.inc"
};

static Sint32 cave_keyhole_plate_glow_model_green_vertices[27] = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_plate_glow_model_green = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_model_green.inc"
};

static NJS_CNK_OBJECT cave_keyhole_plate_glow_green_object = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_green_object.inc"
};

static NJS_CNK_OBJECT cave_keyhole_plate_glow_green_assembly = {
#include "stg28/models/keyhole/cave_keyhole_plate_glow_green_assembly.inc"
};

static NJS_TEXNAME cave_keyhole_base_texture_names_blue[3] = {
#include "stg28/models/keyhole/cave_keyhole_base_texture_names_blue.inc"
};

static NJS_TEXLIST cave_keyhole_base_texlist_blue = {
#include "stg28/models/keyhole/cave_keyhole_base_texlist_blue.inc"
};

static Sint16 cave_keyhole_base_glow_model_blue_polygons[117] = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_model_blue_polygons.inc"
};

static Sint32 cave_keyhole_base_glow_model_blue_vertices[67] = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_base_glow_model_blue = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_model_blue.inc"
};

static NJS_CNK_OBJECT cave_keyhole_base_glow_blue_object = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_blue_object.inc"
};

static Sint16 cave_keyhole_base_model_blue_polygons[247] = {
#include "stg28/models/keyhole/cave_keyhole_base_model_blue_polygons.inc"
};

static Sint32 cave_keyhole_base_model_blue_vertices[147] = {
#include "stg28/models/keyhole/cave_keyhole_base_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_base_model_blue = {
#include "stg28/models/keyhole/cave_keyhole_base_model_blue.inc"
};

static NJS_CNK_OBJECT cave_keyhole_base_blue_object = {
#include "stg28/models/keyhole/cave_keyhole_base_blue_object.inc"
};

static NJS_TEXNAME cave_keyhole_base_texture_names_green[3] = {
#include "stg28/models/keyhole/cave_keyhole_base_texture_names_green.inc"
};

static NJS_TEXLIST cave_keyhole_base_texlist_green = {
#include "stg28/models/keyhole/cave_keyhole_base_texlist_green.inc"
};

static Sint16 cave_keyhole_base_glow_model_green_polygons[117] = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_model_green_polygons.inc"
};

static Sint32 cave_keyhole_base_glow_model_green_vertices[67] = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_base_glow_model_green = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_model_green.inc"
};

static NJS_CNK_OBJECT cave_keyhole_base_glow_green_object = {
#include "stg28/models/keyhole/cave_keyhole_base_glow_green_object.inc"
};

static Sint16 cave_keyhole_base_model_green_polygons[247] = {
#include "stg28/models/keyhole/cave_keyhole_base_model_green_polygons.inc"
};

static Sint32 cave_keyhole_base_model_green_vertices[147] = {
#include "stg28/models/keyhole/cave_keyhole_base_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_base_model_green = {
#include "stg28/models/keyhole/cave_keyhole_base_model_green.inc"
};

static NJS_CNK_OBJECT cave_keyhole_base_green_object = {
#include "stg28/models/keyhole/cave_keyhole_base_green_object.inc"
};

static NJS_MATERIAL cave_keyhole_collision_materials[1] = {
#include "stg28/models/keyhole/cave_keyhole_collision_materials.inc"
};

static Sint16 cave_keyhole_collision_indices[42] = {
#include "stg28/models/keyhole/cave_keyhole_collision_indices.inc"
};

static NJS_MESHSET cave_keyhole_collision_meshsets[1] = {
#include "stg28/models/keyhole/cave_keyhole_collision_meshsets.inc"
};

static NJS_POINT3 cave_keyhole_collision_points[17] = {
#include "stg28/models/keyhole/cave_keyhole_collision_points.inc"
};

static NJS_VECTOR cave_keyhole_collision_normals[17] = {
#include "stg28/models/keyhole/cave_keyhole_collision_normals.inc"
};

static NJS_MODEL cave_keyhole_collision_model = {
#include "stg28/models/keyhole/cave_keyhole_collision_model.inc"
};

static NJS_OBJECT cave_keyhole_collision = {
#include "stg28/models/keyhole/cave_keyhole_collision.inc"
};

static NJS_MATERIAL cave_keyhole_base_collision_materials[1] = {
#include "stg28/models/keyhole/cave_keyhole_base_collision_materials.inc"
};

static Sint16 cave_keyhole_base_collision_indices[59] = {
#include "stg28/models/keyhole/cave_keyhole_base_collision_indices.inc"
};

static NJS_MESHSET cave_keyhole_base_collision_meshsets[1] = {
#include "stg28/models/keyhole/cave_keyhole_base_collision_meshsets.inc"
};

static NJS_POINT3 cave_keyhole_base_collision_points[24] = {
#include "stg28/models/keyhole/cave_keyhole_base_collision_points.inc"
};

static NJS_VECTOR cave_keyhole_base_collision_normals[24] = {
#include "stg28/models/keyhole/cave_keyhole_base_collision_normals.inc"
};

static NJS_MODEL cave_keyhole_base_collision_model = {
#include "stg28/models/keyhole/cave_keyhole_base_collision_model.inc"
};

static NJS_OBJECT cave_keyhole_base_collision = {
#include "stg28/models/keyhole/cave_keyhole_base_collision.inc"
};

static NJS_TEXNAME cave_keyhole_lamp_texture_names_0[1] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_texture_names_0.inc"
};

static NJS_TEXLIST cave_keyhole_lamp_texlist_0 = {
#include "stg28/models/keyhole/cave_keyhole_lamp_texlist_0.inc"
};

static Sint16 cave_keyhole_lamp_model_0_polygons[117] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_0_polygons.inc"
};

static Sint32 cave_keyhole_lamp_model_0_vertices[131] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_0_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_lamp_model_0 = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_0.inc"
};

static NJS_CNK_OBJECT cave_keyhole_lamp_0_object = {
#include "stg28/models/keyhole/cave_keyhole_lamp_0_object.inc"
};

static NJS_TEXNAME cave_keyhole_lamp_texture_names_1[1] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_texture_names_1.inc"
};

static NJS_TEXLIST cave_keyhole_lamp_texlist_1 = {
#include "stg28/models/keyhole/cave_keyhole_lamp_texlist_1.inc"
};

static Sint16 cave_keyhole_lamp_model_1_polygons[117] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_1_polygons.inc"
};

static Sint32 cave_keyhole_lamp_model_1_vertices[131] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_1_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_lamp_model_1 = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_1.inc"
};

static NJS_CNK_OBJECT cave_keyhole_lamp_1_object = {
#include "stg28/models/keyhole/cave_keyhole_lamp_1_object.inc"
};

static NJS_TEXNAME cave_keyhole_lamp_texture_names_2[1] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_texture_names_2.inc"
};

static NJS_TEXLIST cave_keyhole_lamp_texlist_2 = {
#include "stg28/models/keyhole/cave_keyhole_lamp_texlist_2.inc"
};

static Sint16 cave_keyhole_lamp_model_2_polygons[117] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_2_polygons.inc"
};

static Sint32 cave_keyhole_lamp_model_2_vertices[131] = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_2_vertices.inc"
};

static NJS_CNK_MODEL cave_keyhole_lamp_model_2 = {
#include "stg28/models/keyhole/cave_keyhole_lamp_model_2.inc"
};

static NJS_CNK_OBJECT cave_keyhole_lamp_2_object = {
#include "stg28/models/keyhole/cave_keyhole_lamp_2_object.inc"
};

#endif
