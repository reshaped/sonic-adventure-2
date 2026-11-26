#ifndef STG08_KILL_PLANE_H
#define STG08_KILL_PLANE_H

/* Security Hall's stage kill plane. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL kill_material = {
#include "stg08/kill_plane/kill_material.inc"
};

static Sint16 kill_strip[5] = {
#include "stg08/kill_plane/kill_strip.inc"
};

static NJS_MESHSET kill_faces = {
#include "stg08/kill_plane/kill_faces.inc"
};

static NJS_POINT3 kill_points[4] = {
#include "stg08/kill_plane/kill_points.inc"
};

static NJS_VECTOR kill_normals[4] = {
#include "stg08/kill_plane/kill_normals.inc"
};

static NJS_MODEL kill_model = {
#include "stg08/kill_plane/kill_model.inc"
};

static NJS_OBJECT kill_plane = {
#include "stg08/kill_plane/kill_plane.inc"
};

#endif
