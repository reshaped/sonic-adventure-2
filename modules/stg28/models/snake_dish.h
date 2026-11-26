/* Pyramid Cave snake dish and fire models. */
#ifndef STG28_MODELS_SNAKE_DISH_H
#define STG28_MODELS_SNAKE_DISH_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_snake_dish_fire_texture_names[1] = {
#include "stg28/models/snake_dish/cave_snake_dish_fire_texture_names.inc"
};

static NJS_TEXLIST cave_snake_dish_fire_texlist = {
#include "stg28/models/snake_dish/cave_snake_dish_fire_texlist.inc"
};

static Sint16 cave_snake_dish_fire_polygons[25] = {
#include "stg28/models/snake_dish/cave_snake_dish_fire_polygons.inc"
};

static Sint32 cave_snake_dish_fire_vertices[19] = {
#include "stg28/models/snake_dish/cave_snake_dish_fire_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_dish_fire_model = {
#include "stg28/models/snake_dish/cave_snake_dish_fire_model.inc"
};

static NJS_CNK_OBJECT cave_snake_dish_fire_object = {
#include "stg28/models/snake_dish/cave_snake_dish_fire_object.inc"
};

static NJS_TEXNAME cave_snake_dish_texture_names[6] = {
#include "stg28/models/snake_dish/cave_snake_dish_texture_names.inc"
};

static NJS_TEXLIST cave_snake_dish_texlist = {
#include "stg28/models/snake_dish/cave_snake_dish_texlist.inc"
};

static Sint16 cave_snake_dish_polygons[1045] = {
#include "stg28/models/snake_dish/cave_snake_dish_polygons.inc"
};

static Sint32 cave_snake_dish_vertices[523] = {
#include "stg28/models/snake_dish/cave_snake_dish_vertices.inc"
};

static NJS_CNK_MODEL cave_snake_dish_model = {
#include "stg28/models/snake_dish/cave_snake_dish_model.inc"
};

static NJS_CNK_OBJECT cave_snake_dish_object = {
#include "stg28/models/snake_dish/cave_snake_dish_object.inc"
};

#endif
