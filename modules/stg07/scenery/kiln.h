#ifndef STG07_SCENERY_KILN_H
#define STG07_SCENERY_KILN_H

/* Kiln. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME kiln_texlist_names[4] = {
#include "stg07/scenery/kiln/kiln_texlist_names.inc"
};

static NJS_TEXLIST kiln_texlist = {
#include "stg07/scenery/kiln/kiln_texlist.inc"
};

static Sint16 kiln_object_child_polygons[25] = {
#include "stg07/scenery/kiln/kiln_object_child_polygons.inc"
};

static Sint32 kiln_object_child_vertices[19] = {
#include "stg07/scenery/kiln/kiln_object_child_vertices.inc"
};

static NJS_CNK_MODEL kiln_object_child_model = {
#include "stg07/scenery/kiln/kiln_object_child_model.inc"
};

static NJS_CNK_OBJECT kiln_object_child = {
#include "stg07/scenery/kiln/kiln_object_child.inc"
};

static Sint16 kiln_polygons[615] = {
#include "stg07/scenery/kiln/kiln_polygons.inc"
};

static Sint32 kiln_vertices[427] = {
#include "stg07/scenery/kiln/kiln_vertices.inc"
};

static NJS_CNK_MODEL kiln_model = {
#include "stg07/scenery/kiln/kiln_model.inc"
};

static NJS_CNK_OBJECT kiln_object = {
#include "stg07/scenery/kiln/kiln_object.inc"
};

static NJS_MATERIAL kiln_collision_materials = {
#include "stg07/scenery/kiln/kiln_collision_materials.inc"
};

static Uint16 kiln_collision_strips[51] = {
#include "stg07/scenery/kiln/kiln_collision_strips.inc"
};

static NJS_MESHSET kiln_collision_faces = {
#include "stg07/scenery/kiln/kiln_collision_faces.inc"
};

static NJS_POINT3 kiln_collision_points[18] = {
#include "stg07/scenery/kiln/kiln_collision_points.inc"
};

static NJS_VECTOR kiln_collision_normals[18] = {
#include "stg07/scenery/kiln/kiln_collision_normals.inc"
};

static NJS_MODEL kiln_collision_model = {
#include "stg07/scenery/kiln/kiln_collision_model.inc"
};

static NJS_OBJECT kiln_collision = {
#include "stg07/scenery/kiln/kiln_collision.inc"
};

#endif
