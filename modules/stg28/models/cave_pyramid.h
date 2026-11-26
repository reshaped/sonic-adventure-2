/* Pyramid Cave pyramid, eye and collision shape. */
#ifndef STG28_MODELS_CAVE_PYRAMID_H
#define STG28_MODELS_CAVE_PYRAMID_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_pyramid_texture_names[2] = {
#include "stg28/models/cave_pyramid/cave_pyramid_texture_names.inc"
};

static NJS_TEXLIST cave_pyramid_texlist = {
#include "stg28/models/cave_pyramid/cave_pyramid_texlist.inc"
};

static Sint16 cave_pyramid_polygons[71] = {
#include "stg28/models/cave_pyramid/cave_pyramid_polygons.inc"
};

static Sint32 cave_pyramid_vertices[55] = {
#include "stg28/models/cave_pyramid/cave_pyramid_vertices.inc"
};

static NJS_CNK_MODEL cave_pyramid_model = {
#include "stg28/models/cave_pyramid/cave_pyramid_model.inc"
};

static NJS_CNK_OBJECT cave_pyramid_object = {
#include "stg28/models/cave_pyramid/cave_pyramid_object.inc"
};

#endif
