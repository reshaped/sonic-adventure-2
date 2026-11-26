#ifndef STG08_SAFE_SCENERY_0_H
#define STG08_SAFE_SCENERY_0_H

/* Scenery 0 geometry. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME scenery_0_names[8] = {
#include "stg08/safe/scenery_0/scenery_0_names.inc"
};

static NJS_TEXLIST security_scenery_0_texlist = {
#include "stg08/safe/scenery_0/security_scenery_0_texlist.inc"
};

static Sint16 scenery_0_polygons[1495] = {
#include "stg08/safe/scenery_0/scenery_0_polygons.inc"
};

static Sint32 scenery_0_vertices[1071] = {
#include "stg08/safe/scenery_0/scenery_0_vertices.inc"
};

static NJS_CNK_MODEL scenery_0_model = {
#include "stg08/safe/scenery_0/scenery_0_model.inc"
};

static NJS_CNK_OBJECT security_scenery_0_object = {
#include "stg08/safe/scenery_0/security_scenery_0_object.inc"
};

static NJS_MATERIAL scenery_0_solid_materials = {
#include "stg08/safe/scenery_0/scenery_0_solid_materials.inc"
};

static Uint16 scenery_0_solid_strips[72] = {
#include "stg08/safe/scenery_0/scenery_0_solid_strips.inc"
};

static NJS_MESHSET scenery_0_solid_faces = {
#include "stg08/safe/scenery_0/scenery_0_solid_faces.inc"
};

static NJS_POINT3 scenery_0_solid_points[31] = {
#include "stg08/safe/scenery_0/scenery_0_solid_points.inc"
};

static NJS_VECTOR scenery_0_solid_normals[31] = {
#include "stg08/safe/scenery_0/scenery_0_solid_normals.inc"
};

static NJS_MODEL scenery_0_solid_model = {
#include "stg08/safe/scenery_0/scenery_0_solid_model.inc"
};

static NJS_OBJECT security_scenery_0_solid = {
#include "stg08/safe/scenery_0/security_scenery_0_solid.inc"
};

#endif
