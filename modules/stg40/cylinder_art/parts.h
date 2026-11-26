#ifndef STG40_CYLINDER_ART_PARTS_H
#define STG40_CYLINDER_ART_PARTS_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME textures_parts[3] = {
#include "stg40/cylinder_art/parts/textures_parts.inc"
};

static NJS_TEXLIST gravity_cylinder_parts_texlist = {
#include "stg40/cylinder_art/parts/gravity_cylinder_parts_texlist.inc"
};

static Sint16 gravity_cylinder_parts_object_polygons[145] = {
#include "stg40/cylinder_art/parts/gravity_cylinder_parts_object_polygons.inc"
};

static Sint32 gravity_cylinder_parts_object_vertices[107] = {
#include "stg40/cylinder_art/parts/gravity_cylinder_parts_object_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_parts_object_model = {
#include "stg40/cylinder_art/parts/gravity_cylinder_parts_object_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_parts_object[1] = {
#include "stg40/cylinder_art/parts/gravity_cylinder_parts_object.inc"
};

#endif
