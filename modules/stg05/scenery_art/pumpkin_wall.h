/* Pumpkin wall and its two collision pieces. */
#ifndef STG05_SCENERY_ART_PUMPKIN_WALL_H
#define STG05_SCENERY_ART_PUMPKIN_WALL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME pumpkin_wall_texlist_names[5] = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_texlist_names.inc"
};

static NJS_TEXLIST pumpkin_wall_texlist = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_texlist.inc"
};

static Sint16 pumpkin_wall_polygons[321] = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_polygons.inc"
};

static Sint32 pumpkin_wall_vertices[323] = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_vertices.inc"
};

static NJS_CNK_MODEL pumpkin_wall_model = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_model.inc"
};

static NJS_CNK_OBJECT pumpkin_wall_object = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_object.inc"
};

static NJS_MATERIAL pumpkin_wall_collision_materials = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_collision_materials.inc"
};

static Uint16 pumpkin_wall_collision_strips[75] = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_collision_strips.inc"
};

static NJS_MESHSET pumpkin_wall_collision_faces = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_collision_faces.inc"
};

static NJS_POINT3 pumpkin_wall_collision_points[60] = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_collision_points.inc"
};

static NJS_VECTOR pumpkin_wall_collision_normals[60] = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_collision_normals.inc"
};

static NJS_MODEL pumpkin_wall_collision_model = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_collision_model.inc"
};

static NJS_OBJECT pumpkin_wall_collision = {
#include "stg05/scenery_art/pumpkin_wall/pumpkin_wall_collision.inc"
};

static NJS_TEXNAME pumsasi_texlist_names[5] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_texlist_names.inc"
};

static NJS_TEXLIST pumsasi_texlist = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_texlist.inc"
};

static Sint16 pumsasi_polygons[407] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_polygons.inc"
};

static Sint32 pumsasi_vertices[271] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_vertices.inc"
};

static NJS_CNK_MODEL pumsasi_model = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_model.inc"
};

static NJS_CNK_OBJECT pumsasi_object = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_object.inc"
};

static NJS_MATERIAL pumsasi_collision_materials = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_collision_materials.inc"
};

static Uint16 pumsasi_collision_strips[92] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_collision_strips.inc"
};

static NJS_MESHSET pumsasi_collision_faces = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_collision_faces.inc"
};

static NJS_POINT3 pumsasi_collision_points[36] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_collision_points.inc"
};

static NJS_VECTOR pumsasi_collision_normals[36] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_collision_normals.inc"
};

static NJS_MODEL pumsasi_collision_model = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_collision_model.inc"
};

static NJS_OBJECT pumsasi_collision = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_collision.inc"
};

static NJS_MATERIAL pumsasi_child_collision_materials = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_child_collision_materials.inc"
};

static Uint16 pumsasi_child_collision_strips[20] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_child_collision_strips.inc"
};

static NJS_MESHSET pumsasi_child_collision_faces = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_child_collision_faces.inc"
};

static NJS_POINT3 pumsasi_child_collision_points[16] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_child_collision_points.inc"
};

static NJS_VECTOR pumsasi_child_collision_normals[16] = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_child_collision_normals.inc"
};

static NJS_MODEL pumsasi_child_collision_model = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_child_collision_model.inc"
};

static NJS_OBJECT pumsasi_child_collision = {
#include "stg05/scenery_art/pumpkin_wall/pumsasi_child_collision.inc"
};

#endif
