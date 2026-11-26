#ifndef STG07_SCENERY_FLOATING_FOOTHOLD_H
#define STG07_SCENERY_FLOATING_FOOTHOLD_H

/* Floating foothold. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME floating_foothold_texlist_names[5] = {
#include "stg07/scenery/floating_foothold/floating_foothold_texlist_names.inc"
};

static NJS_TEXLIST floating_foothold_texlist = {
#include "stg07/scenery/floating_foothold/floating_foothold_texlist.inc"
};

static Sint16 floating_foothold_polygons[155] = {
#include "stg07/scenery/floating_foothold/floating_foothold_polygons.inc"
};

static Sint32 floating_foothold_vertices[119] = {
#include "stg07/scenery/floating_foothold/floating_foothold_vertices.inc"
};

static NJS_CNK_MODEL floating_foothold_model = {
#include "stg07/scenery/floating_foothold/floating_foothold_model.inc"
};

static NJS_CNK_OBJECT floating_foothold_object = {
#include "stg07/scenery/floating_foothold/floating_foothold_object.inc"
};

static NJS_MATERIAL floating_foothold_collision_materials = {
#include "stg07/scenery/floating_foothold/floating_foothold_collision_materials.inc"
};

static Uint16 floating_foothold_collision_strips[30] = {
#include "stg07/scenery/floating_foothold/floating_foothold_collision_strips.inc"
};

static NJS_MESHSET floating_foothold_collision_faces = {
#include "stg07/scenery/floating_foothold/floating_foothold_collision_faces.inc"
};

static NJS_POINT3 floating_foothold_collision_points[8] = {
#include "stg07/scenery/floating_foothold/floating_foothold_collision_points.inc"
};

static NJS_VECTOR floating_foothold_collision_normals[8] = {
#include "stg07/scenery/floating_foothold/floating_foothold_collision_normals.inc"
};

static NJS_MODEL floating_foothold_collision_model = {
#include "stg07/scenery/floating_foothold/floating_foothold_collision_model.inc"
};

static NJS_OBJECT floating_foothold_collision = {
#include "stg07/scenery/floating_foothold/floating_foothold_collision.inc"
};

#endif
