#ifndef STG28_MODELS_TORCH_CUP_H
#define STG28_MODELS_TORCH_CUP_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_torch_cup_flame_texture_names[1] = {
#include "stg28/models/torch_cup/cave_torch_cup_flame_texture_names.inc"
};

static NJS_TEXLIST cave_torch_cup_flame_texlist = {
#include "stg28/models/torch_cup/cave_torch_cup_flame_texlist.inc"
};

static Sint16 cave_torch_cup_flame_polygons[25] = {
#include "stg28/models/torch_cup/cave_torch_cup_flame_polygons.inc"
};

static Sint32 cave_torch_cup_flame_vertices[19] = {
#include "stg28/models/torch_cup/cave_torch_cup_flame_vertices.inc"
};

static NJS_CNK_MODEL cave_torch_cup_flame_model[1] = {
#include "stg28/models/torch_cup/cave_torch_cup_flame_model.inc"
};

static NJS_CNK_OBJECT cave_torch_cup_flame_object = {
#include "stg28/models/torch_cup/cave_torch_cup_flame_object.inc"
};

static NJS_TEXNAME cave_torch_cup_texture_names[1] = {
#include "stg28/models/torch_cup/cave_torch_cup_texture_names.inc"
};

static NJS_TEXLIST cave_torch_cup_texlist = {
#include "stg28/models/torch_cup/cave_torch_cup_texlist.inc"
};

static Sint16 cave_torch_cup_polygons[129] = {
#include "stg28/models/torch_cup/cave_torch_cup_polygons.inc"
};

static Sint32 cave_torch_cup_vertices[79] = {
#include "stg28/models/torch_cup/cave_torch_cup_vertices.inc"
};

static NJS_CNK_MODEL cave_torch_cup_model = {
#include "stg28/models/torch_cup/cave_torch_cup_model.inc"
};

static NJS_CNK_OBJECT cave_torch_cup_object = {
#include "stg28/models/torch_cup/cave_torch_cup_object.inc"
};

#endif
