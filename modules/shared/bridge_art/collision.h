/* Collision shared by the fixed bridge deck and the explosive span. */
#ifndef SHARED_BRIDGE_ART_COLLISION_H
#define SHARED_BRIDGE_ART_COLLISION_H
#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL dynamite_bridge_collision_ledge_materials = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_ledge_materials.inc"
};

static Uint16 dynamite_bridge_collision_ledge_strips[10] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_ledge_strips.inc"
};

static NJS_MESHSET dynamite_bridge_collision_ledge_faces = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_ledge_faces.inc"
};

static NJS_POINT3 dynamite_bridge_collision_ledge_points[8] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_ledge_points.inc"
};

static NJS_VECTOR dynamite_bridge_collision_ledge_normals[8] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_ledge_normals.inc"
};

static NJS_MODEL dynamite_bridge_collision_ledge_model = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_ledge_model.inc"
};

static NJS_OBJECT dynamite_bridge_collision_ledge = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_ledge.inc"
};

static NJS_MATERIAL dynamite_bridge_collision_side_materials = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_side_materials.inc"
};

static Uint16 dynamite_bridge_collision_side_strips[18] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_side_strips.inc"
};

static NJS_MESHSET dynamite_bridge_collision_side_faces = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_side_faces.inc"
};

static NJS_POINT3 dynamite_bridge_collision_side_points[16] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_side_points.inc"
};

static NJS_VECTOR dynamite_bridge_collision_side_normals[16] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_side_normals.inc"
};

static NJS_MODEL dynamite_bridge_collision_side_model = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_side_model.inc"
};

static NJS_OBJECT dynamite_bridge_collision_side = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_side.inc"
};

static NJS_MATERIAL dynamite_bridge_collision_materials = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_materials.inc"
};

static Uint16 dynamite_bridge_collision_strips[19] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_strips.inc"
};

static NJS_MESHSET dynamite_bridge_collision_faces = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_faces.inc"
};

static NJS_POINT3 dynamite_bridge_collision_points[8] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_points.inc"
};

static NJS_VECTOR dynamite_bridge_collision_normals[8] = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_normals.inc"
};

static NJS_MODEL dynamite_bridge_collision_model = {
#include "shared/bridge_art/collision/dynamite_bridge_collision_model.inc"
};

#endif
