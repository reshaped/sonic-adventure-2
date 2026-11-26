#ifndef STG35_KILL_PLANES_H
#define STG35_KILL_PLANES_H

/* Collision surfaces registered when the walker enters its route. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL kill_west_material = {
#include "stg35/kill_planes/kill_west_material.inc"
};
static Sint16 kill_west_strips[14] = {
#include "stg35/kill_planes/kill_west_strips.inc"
};
static NJS_MESHSET kill_west_face = {
#include "stg35/kill_planes/kill_west_face.inc"
};
static NJS_POINT3 kill_west_points[9] = {
#include "stg35/kill_planes/kill_west_points.inc"
};
static NJS_VECTOR kill_west_normals[9] = {
#include "stg35/kill_planes/kill_west_normals.inc"
};
static NJS_MODEL kill_west_model = {
#include "stg35/kill_planes/kill_west_model.inc"
};
static NJS_OBJECT kill_west = {
#include "stg35/kill_planes/kill_west.inc"
};

static NJS_MATERIAL kill_entry_material = {
#include "stg35/kill_planes/kill_entry_material.inc"
};
static Sint16 kill_entry_strips[14] = {
#include "stg35/kill_planes/kill_entry_strips.inc"
};
static NJS_MESHSET kill_entry_face = {
#include "stg35/kill_planes/kill_entry_face.inc"
};
static NJS_POINT3 kill_entry_points[9] = {
#include "stg35/kill_planes/kill_entry_points.inc"
};
static NJS_VECTOR kill_entry_normals[9] = {
#include "stg35/kill_planes/kill_entry_normals.inc"
};
static NJS_MODEL kill_entry_model = {
#include "stg35/kill_planes/kill_entry_model.inc"
};
static NJS_OBJECT kill_entry = {
#include "stg35/kill_planes/kill_entry.inc"
};

static NJS_MATERIAL kill_depths_material = {
#include "stg35/kill_planes/kill_depths_material.inc"
};
static Sint16 kill_depths_strips[5] = {
#include "stg35/kill_planes/kill_depths_strips.inc"
};
static NJS_MESHSET kill_depths_face = {
#include "stg35/kill_planes/kill_depths_face.inc"
};
static NJS_POINT3 kill_depths_points[4] = {
#include "stg35/kill_planes/kill_depths_points.inc"
};
static NJS_VECTOR kill_depths_normals[4] = {
#include "stg35/kill_planes/kill_depths_normals.inc"
};
static NJS_MODEL kill_depths_model = {
#include "stg35/kill_planes/kill_depths_model.inc"
};
static NJS_OBJECT kill_depths = {
#include "stg35/kill_planes/kill_depths.inc"
};

#endif
