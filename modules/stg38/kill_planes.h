#ifndef STG38_KILL_PLANES_H
#define STG38_KILL_PLANES_H

/* Collision surfaces used by the route's player task. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL kill_upper_material = {
#include "stg38/kill_planes/kill_upper_material.inc"
};
static Sint16 kill_upper_strips[5] = {
#include "stg38/kill_planes/kill_upper_strips.inc"
};
static NJS_MESHSET kill_upper_face = {
#include "stg38/kill_planes/kill_upper_face.inc"
};
static NJS_POINT3 kill_upper_points[4] = {
#include "stg38/kill_planes/kill_upper_points.inc"
};
static NJS_VECTOR kill_upper_normals[4] = {
#include "stg38/kill_planes/kill_upper_normals.inc"
};
static NJS_MODEL kill_upper_model = {
#include "stg38/kill_planes/kill_upper_model.inc"
};
static NJS_OBJECT kill_upper = {
#include "stg38/kill_planes/kill_upper.inc"
};

static NJS_MATERIAL kill_lower_material = {
#include "stg38/kill_planes/kill_lower_material.inc"
};
static Sint16 kill_lower_strips[5] = {
#include "stg38/kill_planes/kill_lower_strips.inc"
};
static NJS_MESHSET kill_lower_face = {
#include "stg38/kill_planes/kill_lower_face.inc"
};
static NJS_POINT3 kill_lower_points[4] = {
#include "stg38/kill_planes/kill_lower_points.inc"
};
static NJS_VECTOR kill_lower_normals[4] = {
#include "stg38/kill_planes/kill_lower_normals.inc"
};
static NJS_MODEL kill_lower_model = {
#include "stg38/kill_planes/kill_lower_model.inc"
};
static NJS_OBJECT kill_lower = {
#include "stg38/kill_planes/kill_lower.inc"
};

#endif
