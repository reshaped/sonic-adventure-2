#ifndef STG28_MODELS_KEY_H
#define STG28_MODELS_KEY_H

#include <ninja.h>
#include <sg_xpt.h>

enum {
    KEY_BLUE_TEX_DZ_PC_DOORKEY_KANKYOU64,
    KEY_BLUE_TEX_DZ_PC_IRONBAR_DISH128
};

enum {
    KEY_GREEN_TEX_DZ_PC_DOORKEY_KANKYOU64,
    KEY_GREEN_TEX_DZ_PC_IRONBAR_DISH128
};

enum { KEY_STAND_BLUE_TEX_DZ_PC_HASIRA128, KEY_STAND_BLUE_TEX_KD_PC_HASIRA3 };

enum { KEY_STAND_GREEN_TEX_DZ_PC_HASIRA128, KEY_STAND_GREEN_TEX_KD_PC_HASIRA3 };

static NJS_TEXNAME cave_key_texture_names_blue[2] = {
#include "stg28/models/key/cave_key_texture_names_blue.inc"
};

static NJS_TEXLIST cave_key_texlist_blue = {
#include "stg28/models/key/cave_key_texlist_blue.inc"
};

static Sint16 cave_key_model_blue_polygons[225] = {
#include "stg28/models/key/cave_key_model_blue_polygons.inc"
};

static Sint32 cave_key_model_blue_vertices[123] = {
#include "stg28/models/key/cave_key_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_key_model_blue = {
#include "stg28/models/key/cave_key_model_blue.inc"
};

static NJS_CNK_OBJECT cave_key_blue_object = {
#include "stg28/models/key/cave_key_blue_object.inc"
};

static Sint16 cave_key_unlit_model_blue_polygons[69] = {
#include "stg28/models/key/cave_key_unlit_model_blue_polygons.inc"
};

static Sint32 cave_key_unlit_model_blue_vertices[147] = {
#include "stg28/models/key/cave_key_unlit_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_key_unlit_model_blue = {
#include "stg28/models/key/cave_key_unlit_model_blue.inc"
};

static NJS_CNK_OBJECT cave_key_unlit_blue_object = {
#include "stg28/models/key/cave_key_unlit_blue_object.inc"
};

static NJS_CNK_OBJECT cave_key_unlit_blue_assembly = {
#include "stg28/models/key/cave_key_unlit_blue_assembly.inc"
};

static NJS_TEXNAME cave_key_texture_names_green[2] = {
#include "stg28/models/key/cave_key_texture_names_green.inc"
};

static NJS_TEXLIST cave_key_texlist_green = {
#include "stg28/models/key/cave_key_texlist_green.inc"
};

static Sint16 cave_key_model_green_polygons[225] = {
#include "stg28/models/key/cave_key_model_green_polygons.inc"
};

static Sint32 cave_key_model_green_vertices[123] = {
#include "stg28/models/key/cave_key_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_key_model_green = {
#include "stg28/models/key/cave_key_model_green.inc"
};

static NJS_CNK_OBJECT cave_key_green_object = {
#include "stg28/models/key/cave_key_green_object.inc"
};

static Sint16 cave_key_unlit_model_green_polygons[69] = {
#include "stg28/models/key/cave_key_unlit_model_green_polygons.inc"
};

static Sint32 cave_key_unlit_model_green_vertices[147] = {
#include "stg28/models/key/cave_key_unlit_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_key_unlit_model_green = {
#include "stg28/models/key/cave_key_unlit_model_green.inc"
};

static NJS_CNK_OBJECT cave_key_unlit_green_object = {
#include "stg28/models/key/cave_key_unlit_green_object.inc"
};

static NJS_CNK_OBJECT cave_key_unlit_green_assembly = {
#include "stg28/models/key/cave_key_unlit_green_assembly.inc"
};

static NJS_TEXNAME cave_key_stand_texture_names_blue[2] = {
#include "stg28/models/key/cave_key_stand_texture_names_blue.inc"
};

static NJS_TEXLIST cave_key_stand_texlist_blue = {
#include "stg28/models/key/cave_key_stand_texlist_blue.inc"
};

static Sint16 cave_key_stand_model_blue_polygons[355] = {
#include "stg28/models/key/cave_key_stand_model_blue_polygons.inc"
};

static Sint32 cave_key_stand_model_blue_vertices[199] = {
#include "stg28/models/key/cave_key_stand_model_blue_vertices.inc"
};

static NJS_CNK_MODEL cave_key_stand_model_blue = {
#include "stg28/models/key/cave_key_stand_model_blue.inc"
};

static NJS_CNK_OBJECT cave_key_stand_blue_object = {
#include "stg28/models/key/cave_key_stand_blue_object.inc"
};

static NJS_TEXNAME cave_key_stand_texture_names_green[2] = {
#include "stg28/models/key/cave_key_stand_texture_names_green.inc"
};

static NJS_TEXLIST cave_key_stand_texlist_green = {
#include "stg28/models/key/cave_key_stand_texlist_green.inc"
};

static Sint16 cave_key_stand_model_green_polygons[355] = {
#include "stg28/models/key/cave_key_stand_model_green_polygons.inc"
};

static Sint32 cave_key_stand_model_green_vertices[199] = {
#include "stg28/models/key/cave_key_stand_model_green_vertices.inc"
};

static NJS_CNK_MODEL cave_key_stand_model_green = {
#include "stg28/models/key/cave_key_stand_model_green.inc"
};

static NJS_CNK_OBJECT cave_key_stand_green_object = {
#include "stg28/models/key/cave_key_stand_green_object.inc"
};

#endif
