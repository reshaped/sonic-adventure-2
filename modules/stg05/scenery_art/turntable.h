/* Turntable. */
#ifndef STG05_SCENERY_ART_TURNTABLE_H
#define STG05_SCENERY_ART_TURNTABLE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME turntable_texlist_names[2] = {
#include "stg05/scenery_art/turntable/turntable_texlist_names.inc"
};

static NJS_TEXLIST turntable_texlist = {
#include "stg05/scenery_art/turntable/turntable_texlist.inc"
};

static Sint16 turntable_polygons[237] = {
#include "stg05/scenery_art/turntable/turntable_polygons.inc"
};

static Sint32 turntable_vertices[147] = {
#include "stg05/scenery_art/turntable/turntable_vertices.inc"
};

static NJS_CNK_MODEL turntable_model = {
#include "stg05/scenery_art/turntable/turntable_model.inc"
};

static NJS_CNK_OBJECT turntable_object = {
#include "stg05/scenery_art/turntable/turntable_object.inc"
};

static NJS_MATERIAL turntable_collision_materials = {
#include "stg05/scenery_art/turntable/turntable_collision_materials.inc"
};

static Uint16 turntable_collision_strips[76] = {
#include "stg05/scenery_art/turntable/turntable_collision_strips.inc"
};

static NJS_MESHSET turntable_collision_faces = {
#include "stg05/scenery_art/turntable/turntable_collision_faces.inc"
};

static NJS_POINT3 turntable_collision_points[36] = {
#include "stg05/scenery_art/turntable/turntable_collision_points.inc"
};

static NJS_VECTOR turntable_collision_normals[36] = {
#include "stg05/scenery_art/turntable/turntable_collision_normals.inc"
};

static NJS_MODEL turntable_collision_model = {
#include "stg05/scenery_art/turntable/turntable_collision_model.inc"
};

static NJS_OBJECT turntable_collision = {
#include "stg05/scenery_art/turntable/turntable_collision.inc"
};

#endif
