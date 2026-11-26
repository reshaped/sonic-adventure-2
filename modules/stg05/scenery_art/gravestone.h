/* Gravestone. */
#ifndef STG05_SCENERY_ART_GRAVESTONE_H
#define STG05_SCENERY_ART_GRAVESTONE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME gravestone_texlist_names[3] = {
#include "stg05/scenery_art/gravestone/gravestone_texlist_names.inc"
};

static NJS_TEXLIST gravestone_texlist = {
#include "stg05/scenery_art/gravestone/gravestone_texlist.inc"
};

static Sint16 gravestone_swing_polygons[233] = {
#include "stg05/scenery_art/gravestone/gravestone_swing_polygons.inc"
};

static Sint32 gravestone_swing_vertices[103] = {
#include "stg05/scenery_art/gravestone/gravestone_swing_vertices.inc"
};

static NJS_CNK_MODEL gravestone_swing_model = {
#include "stg05/scenery_art/gravestone/gravestone_swing_model.inc"
};

static NJS_CNK_OBJECT gravestone_swing[1] = {
#include "stg05/scenery_art/gravestone/gravestone_swing.inc"
};

static Sint16 gravestone_polygons[157] = {
#include "stg05/scenery_art/gravestone/gravestone_polygons.inc"
};

static Sint32 gravestone_vertices[103] = {
#include "stg05/scenery_art/gravestone/gravestone_vertices.inc"
};

static NJS_CNK_MODEL gravestone_model = {
#include "stg05/scenery_art/gravestone/gravestone_model.inc"
};

static NJS_CNK_OBJECT gravestone_object = {
#include "stg05/scenery_art/gravestone/gravestone_object.inc"
};

static NJS_TEXNAME gravestone_a_texlist_names[3] = {
#include "stg05/scenery_art/gravestone/gravestone_a_texlist_names.inc"
};

static NJS_TEXLIST gravestone_a_texlist = {
#include "stg05/scenery_art/gravestone/gravestone_a_texlist.inc"
};

static Sint16 gravestone_a_swing_polygons[233] = {
#include "stg05/scenery_art/gravestone/gravestone_a_swing_polygons.inc"
};

static Sint32 gravestone_a_swing_vertices[103] = {
#include "stg05/scenery_art/gravestone/gravestone_a_swing_vertices.inc"
};

static NJS_CNK_MODEL gravestone_a_swing_model = {
#include "stg05/scenery_art/gravestone/gravestone_a_swing_model.inc"
};

static NJS_CNK_OBJECT gravestone_a_swing[1] = {
#include "stg05/scenery_art/gravestone/gravestone_a_swing.inc"
};

static Sint16 gravestone_a_polygons[157] = {
#include "stg05/scenery_art/gravestone/gravestone_a_polygons.inc"
};

static Sint32 gravestone_a_vertices[103] = {
#include "stg05/scenery_art/gravestone/gravestone_a_vertices.inc"
};

static NJS_CNK_MODEL gravestone_a_model = {
#include "stg05/scenery_art/gravestone/gravestone_a_model.inc"
};

static NJS_CNK_OBJECT gravestone_a_object = {
#include "stg05/scenery_art/gravestone/gravestone_a_object.inc"
};

static NJS_MATERIAL gravestone_collision_materials = {
#include "stg05/scenery_art/gravestone/gravestone_collision_materials.inc"
};

static Uint16 gravestone_collision_strips[24] = {
#include "stg05/scenery_art/gravestone/gravestone_collision_strips.inc"
};

static NJS_MESHSET gravestone_collision_faces = {
#include "stg05/scenery_art/gravestone/gravestone_collision_faces.inc"
};

static NJS_POINT3 gravestone_collision_points[9] = {
#include "stg05/scenery_art/gravestone/gravestone_collision_points.inc"
};

static NJS_VECTOR gravestone_collision_normals[9] = {
#include "stg05/scenery_art/gravestone/gravestone_collision_normals.inc"
};

static NJS_MODEL gravestone_collision_model = {
#include "stg05/scenery_art/gravestone/gravestone_collision_model.inc"
};

static NJS_OBJECT gravestone_collision = {
#include "stg05/scenery_art/gravestone/gravestone_collision.inc"
};

#endif
