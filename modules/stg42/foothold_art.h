/* The moving arena platform and the collision mesh copied for each platform. */
#ifndef STG42_FOOTHOLD_ART_H
#define STG42_FOOTHOLD_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME foothold_textures[7] = {
#include "stg42/foothold_art/foothold_textures.inc"
};

static NJS_TEXLIST foothold_texlist = {
#include "stg42/foothold_art/foothold_texlist.inc"
};

static Sint16 foothold_polygons[555] = {
#include "stg42/foothold_art/foothold_polygons.inc"
};

static Sint32 foothold_vertices[415] = {
#include "stg42/foothold_art/foothold_vertices.inc"
};

static NJS_CNK_MODEL foothold_model = {
#include "stg42/foothold_art/foothold_model.inc"
};

static NJS_CNK_OBJECT foothold_object = {
#include "stg42/foothold_art/foothold_object.inc"
};

static NJS_MATERIAL foothold_collision_materials[1] = {
#include "stg42/foothold_art/foothold_collision_materials.inc"
};

static Sint16 foothold_collision_indices_0[59] = {
#include "stg42/foothold_art/foothold_collision_indices_0.inc"
};

static NJS_MESHSET foothold_collision_meshsets[1] = {
#include "stg42/foothold_art/foothold_collision_meshsets.inc"
};

static NJS_POINT3 foothold_collision_points[24] = {
#include "stg42/foothold_art/foothold_collision_points.inc"
};

static NJS_VECTOR foothold_collision_normals[24] = {
#include "stg42/foothold_art/foothold_collision_normals.inc"
};

static NJS_MODEL foothold_collision_model = {
#include "stg42/foothold_art/foothold_collision_model.inc"
};

static NJS_OBJECT foothold_collision = {
#include "stg42/foothold_art/foothold_collision.inc"
};

#endif
