#ifndef STG07_SCENERY_MIMOCL_H
#define STG07_SCENERY_MIMOCL_H

/* Mimocl. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME mimocl_texlist_names[7] = {
#include "stg07/scenery/mimocl/mimocl_texlist_names.inc"
};

static NJS_TEXLIST mimocl_texlist = {
#include "stg07/scenery/mimocl/mimocl_texlist.inc"
};

static Sint16 mimocl_object_child_polygons[237] = {
#include "stg07/scenery/mimocl/mimocl_object_child_polygons.inc"
};

static Sint32 mimocl_object_child_vertices[215] = {
#include "stg07/scenery/mimocl/mimocl_object_child_vertices.inc"
};

static NJS_CNK_MODEL mimocl_object_child_model = {
#include "stg07/scenery/mimocl/mimocl_object_child_model.inc"
};

static NJS_CNK_OBJECT mimocl_object_child = {
#include "stg07/scenery/mimocl/mimocl_object_child.inc"
};

static Sint16 mimocl_polygons[409] = {
#include "stg07/scenery/mimocl/mimocl_polygons.inc"
};

static Sint32 mimocl_vertices[295] = {
#include "stg07/scenery/mimocl/mimocl_vertices.inc"
};

static NJS_CNK_MODEL mimocl_model = {
#include "stg07/scenery/mimocl/mimocl_model.inc"
};

static NJS_CNK_OBJECT mimocl_object = {
#include "stg07/scenery/mimocl/mimocl_object.inc"
};

static NJS_MATERIAL mimocl_collision_materials = {
#include "stg07/scenery/mimocl/mimocl_collision_materials.inc"
};

static Uint16 mimocl_collision_strips[29] = {
#include "stg07/scenery/mimocl/mimocl_collision_strips.inc"
};

static NJS_MESHSET mimocl_collision_faces = {
#include "stg07/scenery/mimocl/mimocl_collision_faces.inc"
};

static NJS_POINT3 mimocl_collision_points[18] = {
#include "stg07/scenery/mimocl/mimocl_collision_points.inc"
};

static NJS_VECTOR mimocl_collision_normals[18] = {
#include "stg07/scenery/mimocl/mimocl_collision_normals.inc"
};

static NJS_MODEL mimocl_collision_model = {
#include "stg07/scenery/mimocl/mimocl_collision_model.inc"
};

static NJS_OBJECT mimocl_collision = {
#include "stg07/scenery/mimocl/mimocl_collision.inc"
};

#endif
