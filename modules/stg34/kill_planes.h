#ifndef STG34_KILL_PLANES_H
#define STG34_KILL_PLANES_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL kill_upper_material = {
#include "stg34/kill_planes/kill_upper_material.inc"
};
static Sint16 kill_upper_quad[4] = {
#include "stg34/kill_planes/kill_upper_quad.inc"
};
static NJS_MESHSET kill_upper_face = {
#include "stg34/kill_planes/kill_upper_face.inc"
};
static NJS_POINT3 kill_upper_points[4] = {
#include "stg34/kill_planes/kill_upper_points.inc"
};
static NJS_VECTOR kill_upper_normals[4] = {
#include "stg34/kill_planes/kill_upper_normals.inc"
};
static NJS_MODEL kill_upper_model = {
#include "stg34/kill_planes/kill_upper_model.inc"
};
static NJS_OBJECT kill_upper = {
#include "stg34/kill_planes/kill_upper.inc"
};

static NJS_MATERIAL kill_middle_material = {
#include "stg34/kill_planes/kill_middle_material.inc"
};
static Sint16 kill_middle_quad[4] = {
#include "stg34/kill_planes/kill_middle_quad.inc"
};
static NJS_MESHSET kill_middle_face = {
#include "stg34/kill_planes/kill_middle_face.inc"
};
static NJS_POINT3 kill_middle_points[4] = {
#include "stg34/kill_planes/kill_middle_points.inc"
};
static NJS_VECTOR kill_middle_normals[4] = {
#include "stg34/kill_planes/kill_middle_normals.inc"
};
static NJS_MODEL kill_middle_model = {
#include "stg34/kill_planes/kill_middle_model.inc"
};
static NJS_OBJECT kill_middle = {
#include "stg34/kill_planes/kill_middle.inc"
};

static NJS_MATERIAL kill_lower_material = {
#include "stg34/kill_planes/kill_lower_material.inc"
};
static Sint16 kill_lower_quad[4] = {
#include "stg34/kill_planes/kill_lower_quad.inc"
};
static NJS_MESHSET kill_lower_face = {
#include "stg34/kill_planes/kill_lower_face.inc"
};
static NJS_POINT3 kill_lower_points[4] = {
#include "stg34/kill_planes/kill_lower_points.inc"
};
static NJS_VECTOR kill_lower_normals[4] = {
#include "stg34/kill_planes/kill_lower_normals.inc"
};
static NJS_MODEL kill_lower_model = {
#include "stg34/kill_planes/kill_lower_model.inc"
};
static NJS_OBJECT kill_lower = {
#include "stg34/kill_planes/kill_lower.inc"
};

#endif
