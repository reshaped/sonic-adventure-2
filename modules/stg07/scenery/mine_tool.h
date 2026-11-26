#ifndef STG07_SCENERY_MINE_TOOL_H
#define STG07_SCENERY_MINE_TOOL_H

/* Mine tool. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME mine_tool_texlist_names[7] = {
#include "stg07/scenery/mine_tool/mine_tool_texlist_names.inc"
};

static NJS_TEXLIST mine_tool_texlist = {
#include "stg07/scenery/mine_tool/mine_tool_texlist.inc"
};

static Sint16 mine_tool_object_child_polygons[401] = {
#include "stg07/scenery/mine_tool/mine_tool_object_child_polygons.inc"
};

static Sint32 mine_tool_object_child_vertices[363] = {
#include "stg07/scenery/mine_tool/mine_tool_object_child_vertices.inc"
};

static NJS_CNK_MODEL mine_tool_object_child_model = {
#include "stg07/scenery/mine_tool/mine_tool_object_child_model.inc"
};

static NJS_CNK_OBJECT mine_tool_object_child = {
#include "stg07/scenery/mine_tool/mine_tool_object_child.inc"
};

static Sint16 mine_tool_polygons[311] = {
#include "stg07/scenery/mine_tool/mine_tool_polygons.inc"
};

static Sint32 mine_tool_vertices[331] = {
#include "stg07/scenery/mine_tool/mine_tool_vertices.inc"
};

static NJS_CNK_MODEL mine_tool_model = {
#include "stg07/scenery/mine_tool/mine_tool_model.inc"
};

static NJS_CNK_OBJECT mine_tool_object = {
#include "stg07/scenery/mine_tool/mine_tool_object.inc"
};

static NJS_MATERIAL mine_tool_collision_materials = {
#include "stg07/scenery/mine_tool/mine_tool_collision_materials.inc"
};

static Uint16 mine_tool_collision_strips[46] = {
#include "stg07/scenery/mine_tool/mine_tool_collision_strips.inc"
};

static NJS_MESHSET mine_tool_collision_faces = {
#include "stg07/scenery/mine_tool/mine_tool_collision_faces.inc"
};

static NJS_POINT3 mine_tool_collision_points[28] = {
#include "stg07/scenery/mine_tool/mine_tool_collision_points.inc"
};

static NJS_VECTOR mine_tool_collision_normals[28] = {
#include "stg07/scenery/mine_tool/mine_tool_collision_normals.inc"
};

static NJS_MODEL mine_tool_collision_model = {
#include "stg07/scenery/mine_tool/mine_tool_collision_model.inc"
};

static NJS_OBJECT mine_tool_collision = {
#include "stg07/scenery/mine_tool/mine_tool_collision.inc"
};

static NJS_MATERIAL mine_tool_surface_collision_materials = {
#include "stg07/scenery/mine_tool/mine_tool_surface_collision_materials.inc"
};

static Uint16 mine_tool_surface_collision_strips[97] = {
#include "stg07/scenery/mine_tool/mine_tool_surface_collision_strips.inc"
};

static NJS_MESHSET mine_tool_surface_collision_faces = {
#include "stg07/scenery/mine_tool/mine_tool_surface_collision_faces.inc"
};

static NJS_POINT3 mine_tool_surface_collision_points[39] = {
#include "stg07/scenery/mine_tool/mine_tool_surface_collision_points.inc"
};

static NJS_VECTOR mine_tool_surface_collision_normals[39] = {
#include "stg07/scenery/mine_tool/mine_tool_surface_collision_normals.inc"
};

static NJS_MODEL mine_tool_surface_collision_model = {
#include "stg07/scenery/mine_tool/mine_tool_surface_collision_model.inc"
};

static NJS_OBJECT mine_tool_surface_collision = {
#include "stg07/scenery/mine_tool/mine_tool_surface_collision.inc"
};

#endif
