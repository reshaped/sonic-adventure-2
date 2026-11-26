#ifndef SHARED_CORE_SWITCH_COLLISION_H
#define SHARED_CORE_SWITCH_COLLISION_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_MATERIAL base_collision_material = {
#include "shared/core_switch/collision/base_collision_material.inc"
};

static Sint16 base_collision_strips[40] = {
#include "shared/core_switch/collision/base_collision_strips.inc"
};

static NJS_MESHSET base_collision_meshset = {
#include "shared/core_switch/collision/base_collision_meshset.inc"
};

static NJS_POINT3 base_collision_points[16] = {
#include "shared/core_switch/collision/base_collision_points.inc"
};

static NJS_VECTOR base_collision_normals[16] = {
#include "shared/core_switch/collision/base_collision_normals.inc"
};

static NJS_MODEL base_collision_model = {
#include "shared/core_switch/collision/base_collision_model.inc"
};

static NJS_OBJECT base_collision_object = {
#include "shared/core_switch/collision/base_collision_object.inc"
};

static NJS_MATERIAL plate_collision_material = {
#include "shared/core_switch/collision/plate_collision_material.inc"
};

static Sint16 plate_collision_strips[60] = {
#include "shared/core_switch/collision/plate_collision_strips.inc"
};

static NJS_MESHSET plate_collision_meshset = {
#include "shared/core_switch/collision/plate_collision_meshset.inc"
};

static NJS_POINT3 plate_collision_points[17] = {
#include "shared/core_switch/collision/plate_collision_points.inc"
};

static NJS_VECTOR plate_collision_normals[17] = {
#include "shared/core_switch/collision/plate_collision_normals.inc"
};

static NJS_MODEL plate_collision_model = {
#include "shared/core_switch/collision/plate_collision_model.inc"
};

#endif
