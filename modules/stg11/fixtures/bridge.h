#ifndef STG11_FIXTURES_BRIDGE_H
#define STG11_FIXTURES_BRIDGE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME bridge_names[7] = {
#include "stg11/fixtures/bridge/bridge_names.inc"
};

static NJS_TEXLIST bridge_texlist = {
#include "stg11/fixtures/bridge/bridge_texlist.inc"
};

static Sint16 bridge_polygons[253] = {
#include "stg11/fixtures/bridge/bridge_polygons.inc"
};

static Sint32 bridge_vertices[243] = {
#include "stg11/fixtures/bridge/bridge_vertices.inc"
};

static NJS_CNK_MODEL bridge_model = {
#include "stg11/fixtures/bridge/bridge_model.inc"
};

static NJS_CNK_OBJECT bridge_object = {
#include "stg11/fixtures/bridge/bridge_object.inc"
};

static NJS_MATERIAL bridge_solid_materials = {
#include "stg11/fixtures/bridge/bridge_solid_materials.inc"
};

static Sint16 bridge_solid_strips[16] = {
#include "stg11/fixtures/bridge/bridge_solid_strips.inc"
};

static NJS_MESHSET bridge_solid_faces = {
#include "stg11/fixtures/bridge/bridge_solid_faces.inc"
};

static NJS_POINT3 bridge_solid_points[8] = {
#include "stg11/fixtures/bridge/bridge_solid_points.inc"
};

static NJS_VECTOR bridge_solid_normals[8] = {
#include "stg11/fixtures/bridge/bridge_solid_normals.inc"
};

static NJS_MODEL bridge_solid_model = {
#include "stg11/fixtures/bridge/bridge_solid_model.inc"
};

static NJS_OBJECT bridge_collision = {
#include "stg11/fixtures/bridge/bridge_collision.inc"
};

#endif
