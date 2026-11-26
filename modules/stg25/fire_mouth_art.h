/* The three carved faces, their flames and collision mesh. */
#ifndef STG25_FIRE_MOUTH_ART_H
#define STG25_FIRE_MOUTH_ART_H

#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg25/prop_stage.h"

#include "stg25/chamber_color.h"

static NJS_TEXNAME fire_mouth_blue_texture_names[3] = {
#include "stg25/fire_mouth_art/fire_mouth_blue_texture_names.inc"
};

static NJS_TEXLIST fire_mouth_blue_texlist = {
#include "stg25/fire_mouth_art/fire_mouth_blue_texlist.inc"
};

static Sint16 fire_mouth_blue_polygons[475] = {
#include "stg25/fire_mouth_art/fire_mouth_blue_polygons.inc"
};

static Sint32 fire_mouth_blue_vertices[243] = {
#include "stg25/fire_mouth_art/fire_mouth_blue_vertices.inc"
};

static NJS_CNK_MODEL fire_mouth_blue_model = {
#include "stg25/fire_mouth_art/fire_mouth_blue_model.inc"
};

static NJS_CNK_OBJECT fire_mouth_blue_object = {
#include "stg25/fire_mouth_art/fire_mouth_blue_object.inc"
};

static NJS_TEXNAME fire_mouth_green_texture_names[3] = {
#include "stg25/fire_mouth_art/fire_mouth_green_texture_names.inc"
};

static NJS_TEXLIST fire_mouth_green_texlist = {
#include "stg25/fire_mouth_art/fire_mouth_green_texlist.inc"
};

static Sint16 fire_mouth_green_polygons[475] = {
#include "stg25/fire_mouth_art/fire_mouth_green_polygons.inc"
};

static Sint32 fire_mouth_green_vertices[243] = {
#include "stg25/fire_mouth_art/fire_mouth_green_vertices.inc"
};

static NJS_CNK_MODEL fire_mouth_green_model = {
#include "stg25/fire_mouth_art/fire_mouth_green_model.inc"
};

static NJS_CNK_OBJECT fire_mouth_green_object = {
#include "stg25/fire_mouth_art/fire_mouth_green_object.inc"
};

static NJS_TEXNAME fire_mouth_red_texture_names[3] = {
#include "stg25/fire_mouth_art/fire_mouth_red_texture_names.inc"
};

static NJS_TEXLIST fire_mouth_red_texlist = {
#include "stg25/fire_mouth_art/fire_mouth_red_texlist.inc"
};

static Sint16 fire_mouth_red_polygons[475] = {
#include "stg25/fire_mouth_art/fire_mouth_red_polygons.inc"
};

static Sint32 fire_mouth_red_vertices[267] = {
#include "stg25/fire_mouth_art/fire_mouth_red_vertices.inc"
};

static NJS_CNK_MODEL fire_mouth_red_model = {
#include "stg25/fire_mouth_art/fire_mouth_red_model.inc"
};

static NJS_CNK_OBJECT fire_mouth_red_object = {
#include "stg25/fire_mouth_art/fire_mouth_red_object.inc"
};

static NJS_TEXNAME fire_mouth_blue_flame_texture_names[1] = {
#include "stg25/fire_mouth_art/fire_mouth_blue_flame_texture_names.inc"
};

static NJS_TEXLIST fire_mouth_blue_flame_texlist = {
#include "stg25/fire_mouth_art/fire_mouth_blue_flame_texlist.inc"
};

static Sint16 fire_mouth_blue_flame_polygons[25] = {
#include "stg25/fire_mouth_art/fire_mouth_blue_flame_polygons.inc"
};

static Sint32 fire_mouth_blue_flame_vertices[19] = {
#include "stg25/fire_mouth_art/fire_mouth_blue_flame_vertices.inc"
};

static NJS_CNK_MODEL fire_mouth_blue_flame_model = {
#include "stg25/fire_mouth_art/fire_mouth_blue_flame_model.inc"
};

static NJS_CNK_OBJECT fire_mouth_blue_flame_object = {
#include "stg25/fire_mouth_art/fire_mouth_blue_flame_object.inc"
};

static NJS_TEXNAME fire_mouth_green_flame_texture_names[1] = {
#include "stg25/fire_mouth_art/fire_mouth_green_flame_texture_names.inc"
};

static NJS_TEXLIST fire_mouth_green_flame_texlist = {
#include "stg25/fire_mouth_art/fire_mouth_green_flame_texlist.inc"
};

static Sint16 fire_mouth_green_flame_polygons[25] = {
#include "stg25/fire_mouth_art/fire_mouth_green_flame_polygons.inc"
};

static Sint32 fire_mouth_green_flame_vertices[19] = {
#include "stg25/fire_mouth_art/fire_mouth_green_flame_vertices.inc"
};

static NJS_CNK_MODEL fire_mouth_green_flame_model = {
#include "stg25/fire_mouth_art/fire_mouth_green_flame_model.inc"
};

static NJS_CNK_OBJECT fire_mouth_green_flame_object = {
#include "stg25/fire_mouth_art/fire_mouth_green_flame_object.inc"
};

static NJS_TEXNAME fire_mouth_red_flame_texture_names[1] = {
#include "stg25/fire_mouth_art/fire_mouth_red_flame_texture_names.inc"
};

static NJS_TEXLIST fire_mouth_red_flame_texlist = {
#include "stg25/fire_mouth_art/fire_mouth_red_flame_texlist.inc"
};

static Sint16 fire_mouth_red_flame_polygons[25] = {
#include "stg25/fire_mouth_art/fire_mouth_red_flame_polygons.inc"
};

static Sint32 fire_mouth_red_flame_vertices[19] = {
#include "stg25/fire_mouth_art/fire_mouth_red_flame_vertices.inc"
};

static NJS_CNK_MODEL fire_mouth_red_flame_model = {
#include "stg25/fire_mouth_art/fire_mouth_red_flame_model.inc"
};

static NJS_CNK_OBJECT fire_mouth_red_flame_object = {
#include "stg25/fire_mouth_art/fire_mouth_red_flame_object.inc"
};

static NJS_MATERIAL fire_mouth_collision_materials[1] = {
#include "stg25/fire_mouth_art/fire_mouth_collision_materials.inc"
};

static Sint16 fire_mouth_collision_indices[90] = {
#include "stg25/fire_mouth_art/fire_mouth_collision_indices.inc"
};

static NJS_MESHSET fire_mouth_collision_meshsets[1] = {
#include "stg25/fire_mouth_art/fire_mouth_collision_meshsets.inc"
};

static NJS_POINT3 fire_mouth_collision_points[28] = {
#include "stg25/fire_mouth_art/fire_mouth_collision_points.inc"
};

static NJS_VECTOR fire_mouth_collision_normals[28] = {
#include "stg25/fire_mouth_art/fire_mouth_collision_normals.inc"
};

static NJS_MODEL fire_mouth_collision_model = {
#include "stg25/fire_mouth_art/fire_mouth_collision_model.inc"
};

static NJS_OBJECT fire_mouth_collision = {
#include "stg25/fire_mouth_art/fire_mouth_collision.inc"
};

static NJS_CNK_MODEL *chamber_fire_mouth_models[CHAMBER_COLORS] = {
#include "stg25/fire_mouth_art/chamber_fire_mouth_models.inc"
};

static NJS_CNK_MODEL *chamber_fire_mouth_flame_models[CHAMBER_COLORS] = {
#include "stg25/fire_mouth_art/chamber_fire_mouth_flame_models.inc"
};

#endif
