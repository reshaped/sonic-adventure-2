/* Movent. */
#ifndef STG06_SCENERY_ART_MOVENT_H
#define STG06_SCENERY_ART_MOVENT_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME movent_texlist_names[8] = {
#include "stg06/scenery_art/movent/movent_texlist_names.inc"
};

static NJS_TEXLIST movent_texlist = {
#include "stg06/scenery_art/movent/movent_texlist.inc"
};

static Sint16 movent_object_child_polygons[231] = {
#include "stg06/scenery_art/movent/movent_object_child_polygons.inc"
};

static Sint32 movent_object_child_vertices[211] = {
#include "stg06/scenery_art/movent/movent_object_child_vertices.inc"
};

static NJS_CNK_MODEL movent_object_child_model = {
#include "stg06/scenery_art/movent/movent_object_child_model.inc"
};

static NJS_CNK_OBJECT movent_object_child = {
#include "stg06/scenery_art/movent/movent_object_child.inc"
};

static Sint16 movent_polygons[459] = {
#include "stg06/scenery_art/movent/movent_polygons.inc"
};

static Sint32 movent_vertices[363] = {
#include "stg06/scenery_art/movent/movent_vertices.inc"
};

static NJS_CNK_MODEL movent_model = {
#include "stg06/scenery_art/movent/movent_model.inc"
};

static NJS_CNK_OBJECT movent_object = {
#include "stg06/scenery_art/movent/movent_object.inc"
};

static NJS_MATERIAL movent_collision_materials = {
#include "stg06/scenery_art/movent/movent_collision_materials.inc"
};

static Uint16 movent_collision_strips[29] = {
#include "stg06/scenery_art/movent/movent_collision_strips.inc"
};

static NJS_MESHSET movent_collision_faces = {
#include "stg06/scenery_art/movent/movent_collision_faces.inc"
};

static NJS_POINT3 movent_collision_points[18] = {
#include "stg06/scenery_art/movent/movent_collision_points.inc"
};

static NJS_VECTOR movent_collision_normals[18] = {
#include "stg06/scenery_art/movent/movent_collision_normals.inc"
};

static NJS_MODEL movent_collision_model = {
#include "stg06/scenery_art/movent/movent_collision_model.inc"
};

static NJS_OBJECT movent_collision = {
#include "stg06/scenery_art/movent/movent_collision.inc"
};

#endif
