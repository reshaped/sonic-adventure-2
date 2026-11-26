#ifndef STG28_MODELS_NEON_H
#define STG28_MODELS_NEON_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_neon_stand_texture_names[1] = {
#include "stg28/models/neon/cave_neon_stand_texture_names.inc"
};

static NJS_TEXLIST cave_neon_stand_texlist = {
#include "stg28/models/neon/cave_neon_stand_texlist.inc"
};

static Sint16 cave_neon_stand_polygons[55] = {
#include "stg28/models/neon/cave_neon_stand_polygons.inc"
};

static Sint32 cave_neon_stand_vertices[51] = {
#include "stg28/models/neon/cave_neon_stand_vertices.inc"
};

static NJS_CNK_MODEL cave_neon_stand_model = {
#include "stg28/models/neon/cave_neon_stand_model.inc"
};

static NJS_CNK_OBJECT cave_neon_stand_object = {
#include "stg28/models/neon/cave_neon_stand_object.inc"
};

static NJS_TEXNAME cave_neon_sign_texture_names[1] = {
#include "stg28/models/neon/cave_neon_sign_texture_names.inc"
};

static NJS_TEXLIST cave_neon_sign_texlist = {
#include "stg28/models/neon/cave_neon_sign_texlist.inc"
};

static Sint16 cave_neon_sign_polygons[253] = {
#include "stg28/models/neon/cave_neon_sign_polygons.inc"
};

static Sint32 cave_neon_sign_vertices[195] = {
#include "stg28/models/neon/cave_neon_sign_vertices.inc"
};

static NJS_CNK_MODEL cave_neon_sign_model = {
#include "stg28/models/neon/cave_neon_sign_model.inc"
};

static NJS_CNK_OBJECT cave_neon_sign_object = {
#include "stg28/models/neon/cave_neon_sign_object.inc"
};

static NJS_TEXNAME cave_neon_light_texture_names[1] = {
#include "stg28/models/neon/cave_neon_light_texture_names.inc"
};

static NJS_TEXLIST cave_neon_light_texlist = {
#include "stg28/models/neon/cave_neon_light_texlist.inc"
};

static Sint16 cave_neon_light_polygons[39] = {
#include "stg28/models/neon/cave_neon_light_polygons.inc"
};

static Sint32 cave_neon_light_vertices[35] = {
#include "stg28/models/neon/cave_neon_light_vertices.inc"
};

static NJS_CNK_MODEL cave_neon_light_model = {
#include "stg28/models/neon/cave_neon_light_model.inc"
};

static NJS_CNK_OBJECT cave_neon_light_object = {
#include "stg28/models/neon/cave_neon_light_object.inc"
};

#endif
