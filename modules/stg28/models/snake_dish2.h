/* Pyramid Cave scaling snake dish and fire models. */
#ifndef STG28_MODELS_SNAKE_DISH2_H
#define STG28_MODELS_SNAKE_DISH2_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_snake_dish2_fire_texture_names[1] = {
#include "stg28/models/snake_dish2/cave_snake_dish2_fire_texture_names.inc"
};

static NJS_TEXLIST cave_snake_dish2_fire_texlist = {
#include "stg28/models/snake_dish2/cave_snake_dish2_fire_texlist.inc"
};

static Sint16 cave_snake_dish2_fire_polygons[25] = {
#include "stg28/models/snake_dish2/cave_snake_dish2_fire_polygons.inc"
};

static Sint32 cave_snake_dish2_fire_vertices[19] = {
#include "stg28/models/snake_dish2/cave_snake_dish2_fire_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_dish2_fire_model = {
#include "stg28/models/snake_dish2/cave_snake_dish2_fire_model.inc"
};

static NJS_CNK_OBJECT cave_snake_dish2_fire_object = {
#include "stg28/models/snake_dish2/cave_snake_dish2_fire_object.inc"
};

static NJS_TEXNAME cave_snake_dish2_texture_names[6] = {
#include "stg28/models/snake_dish2/cave_snake_dish2_texture_names.inc"
};

static NJS_TEXLIST cave_snake_dish2_texlist = {
#include "stg28/models/snake_dish2/cave_snake_dish2_texlist.inc"
};

static Sint16 cave_snake_dish2_polygons[933] = {
#include "stg28/models/snake_dish2/cave_snake_dish2_polygons.inc"
};

static Sint32 cave_snake_dish2_vertices[483] = {
#include "stg28/models/snake_dish2/cave_snake_dish2_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_dish2_model = {
#include "stg28/models/snake_dish2/cave_snake_dish2_model.inc"
};

static NJS_CNK_OBJECT cave_snake_dish2_object = {
#include "stg28/models/snake_dish2/cave_snake_dish2_object.inc"
};

#endif
