#ifndef STG08_SAFE_SCENERY_1_H
#define STG08_SAFE_SCENERY_1_H

/* Scenery 1 geometry. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME scenery_1_names[4] = {
#include "stg08/safe/scenery_1/scenery_1_names.inc"
};

static NJS_TEXLIST security_scenery_1_texlist = {
#include "stg08/safe/scenery_1/security_scenery_1_texlist.inc"
};

static Sint16 scenery_1_polygons[215] = {
#include "stg08/safe/scenery_1/scenery_1_polygons.inc"
};

static Sint32 scenery_1_vertices[179] = {
#include "stg08/safe/scenery_1/scenery_1_vertices.inc"
};

static NJS_CNK_MODEL scenery_1_model = {
#include "stg08/safe/scenery_1/scenery_1_model.inc"
};

static NJS_CNK_OBJECT security_scenery_1_object = {
#include "stg08/safe/scenery_1/security_scenery_1_object.inc"
};

static NJS_MATERIAL scenery_1_solid_materials = {
#include "stg08/safe/scenery_1/scenery_1_solid_materials.inc"
};

static Uint16 scenery_1_solid_strips[19] = {
#include "stg08/safe/scenery_1/scenery_1_solid_strips.inc"
};

static NJS_MESHSET scenery_1_solid_faces = {
#include "stg08/safe/scenery_1/scenery_1_solid_faces.inc"
};

static NJS_POINT3 scenery_1_solid_points[8] = {
#include "stg08/safe/scenery_1/scenery_1_solid_points.inc"
};

static NJS_VECTOR scenery_1_solid_normals[8] = {
#include "stg08/safe/scenery_1/scenery_1_solid_normals.inc"
};

static NJS_MODEL scenery_1_solid_model = {
#include "stg08/safe/scenery_1/scenery_1_solid_model.inc"
};

static NJS_OBJECT security_scenery_1_solid = {
#include "stg08/safe/scenery_1/security_scenery_1_solid.inc"
};

#endif
