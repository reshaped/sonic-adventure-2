/* The 4,000-unit square at y = -50 catches players falling below the canyon. */
#ifndef SHARED_WILD_CANYON_KILL_PLANE_H
#define SHARED_WILD_CANYON_KILL_PLANE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL kill_area_material = {
#include "shared/wild_canyon_kill_plane/kill_area_material.inc"
};
static Sint16 kill_area_meshes[5] = {
#include "shared/wild_canyon_kill_plane/kill_area_meshes.inc"
};
static NJS_MESHSET kill_area_meshset = {
#include "shared/wild_canyon_kill_plane/kill_area_meshset.inc"
};
static NJS_POINT3 kill_area_points[4] = {
#include "shared/wild_canyon_kill_plane/kill_area_points.inc"
};
static NJS_VECTOR kill_area_normals[4] = {
#include "shared/wild_canyon_kill_plane/kill_area_normals.inc"
};
static NJS_MODEL kill_area_model = {
#include "shared/wild_canyon_kill_plane/kill_area_model.inc"
};
static NJS_OBJECT kill_area_object = {
#include "shared/wild_canyon_kill_plane/kill_area_object.inc"
};

#endif
