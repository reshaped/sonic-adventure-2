#ifndef STG07_SCENERY_INFO_H
#define STG07_SCENERY_INFO_H

/* Info. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME info_texlist_names[4] = {
#include "stg07/scenery/info/info_texlist_names.inc"
};

static NJS_TEXLIST info_texlist = {
#include "stg07/scenery/info/info_texlist.inc"
};

static Sint16 info_polygons[291] = {
#include "stg07/scenery/info/info_polygons.inc"
};

static Sint32 info_vertices[295] = {
#include "stg07/scenery/info/info_vertices.inc"
};

static NJS_CNK_MODEL info_model = {
#include "stg07/scenery/info/info_model.inc"
};

static NJS_CNK_OBJECT info_object[1] = {
#include "stg07/scenery/info/info_object.inc"
};

static NJS_MATERIAL info_collision_materials = {
#include "stg07/scenery/info/info_collision_materials.inc"
};

static Uint16 info_collision_strips[25] = {
#include "stg07/scenery/info/info_collision_strips.inc"
};

static NJS_MESHSET info_collision_faces = {
#include "stg07/scenery/info/info_collision_faces.inc"
};

static NJS_POINT3 info_collision_points[14] = {
#include "stg07/scenery/info/info_collision_points.inc"
};

static NJS_VECTOR info_collision_normals[14] = {
#include "stg07/scenery/info/info_collision_normals.inc"
};

static NJS_MODEL info_collision_model = {
#include "stg07/scenery/info/info_collision_model.inc"
};

static NJS_OBJECT info_collision = {
#include "stg07/scenery/info/info_collision.inc"
};

static NJS_TEXNAME info_texture_set_3_names[1] = {
#include "stg07/scenery/info/info_texture_set_3_names.inc"
};

static NJS_TEXLIST info_texture_set_3 = {
#include "stg07/scenery/info/info_texture_set_3.inc"
};

static Sint16 info_face_polygons[39] = {
#include "stg07/scenery/info/info_face_polygons.inc"
};

static Sint32 info_face_vertices[35] = {
#include "stg07/scenery/info/info_face_vertices.inc"
};

static NJS_CNK_MODEL info_face_model = {
#include "stg07/scenery/info/info_face_model.inc"
};

static NJS_CNK_OBJECT info_face_object[1] = {
#include "stg07/scenery/info/info_face_object.inc"
};

static NJS_TEXNAME info_face_texlist_names[3] = {
#include "stg07/scenery/info/info_face_texlist_names.inc"
};

static NJS_TEXLIST info_face_texlist = {
#include "stg07/scenery/info/info_face_texlist.inc"
};

#endif
