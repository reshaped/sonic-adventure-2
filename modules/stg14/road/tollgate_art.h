/* Geometry and texture slots for tollgate. */
#ifndef STG14_ROAD_TOLLGATE_ART_H
#define STG14_ROAD_TOLLGATE_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME tollgate_names[11] = {
#include "stg14/road/tollgate_art/tollgate_names.inc"
};

static NJS_TEXLIST tollgate_texlist = {
#include "stg14/road/tollgate_art/tollgate_texlist.inc"
};

static Sint16 tollgate_polygons[1323] = {
#include "stg14/road/tollgate_art/tollgate_polygons.inc"
};

static Sint32 tollgate_vertices[1347] = {
#include "stg14/road/tollgate_art/tollgate_vertices.inc"
};

static NJS_CNK_MODEL tollgate_model = {
#include "stg14/road/tollgate_art/tollgate_model.inc"
};

static NJS_CNK_OBJECT tollgate_object = {
#include "stg14/road/tollgate_art/tollgate_object.inc"
};

static NJS_MATERIAL tollgate_solid_material = {
#include "stg14/road/tollgate_art/tollgate_solid_material.inc"
};

static Uint16 tollgate_solid_strips[153] = {
#include "stg14/road/tollgate_art/tollgate_solid_strips.inc"
};

static NJS_MESHSET tollgate_solid_faces = {
#include "stg14/road/tollgate_art/tollgate_solid_faces.inc"
};

static NJS_POINT3 tollgate_solid_points[80] = {
#include "stg14/road/tollgate_art/tollgate_solid_points.inc"
};

static NJS_VECTOR tollgate_solid_normals[80] = {
#include "stg14/road/tollgate_art/tollgate_solid_normals.inc"
};

static NJS_MODEL tollgate_solid_model = {
#include "stg14/road/tollgate_art/tollgate_solid_model.inc"
};

static NJS_OBJECT tollgate_collision = {
#include "stg14/road/tollgate_art/tollgate_collision.inc"
};

#endif
