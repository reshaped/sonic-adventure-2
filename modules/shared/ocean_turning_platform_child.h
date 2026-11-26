#ifndef SHARED_OCEAN_TURNING_PLATFORM_CHILD_H
#define SHARED_OCEAN_TURNING_PLATFORM_CHILD_H

#include <ninja.h>
#include <sg_xpt.h>

/* The child task adds this collision mesh to the world in both stages. */
static NJS_MATERIAL turning_platform_child_object_materials[1] = {
#include "shared/ocean_turning_platform_child/turning_platform_child_object_materials.inc"
};

static Sint16 turning_platform_child_object_faces[19] = {
#include "shared/ocean_turning_platform_child/turning_platform_child_object_faces.inc"
};

static NJS_MESHSET turning_platform_child_object_meshsets[1] = {
#include "shared/ocean_turning_platform_child/turning_platform_child_object_meshsets.inc"
};

static NJS_POINT3 turning_platform_child_object_points[16] = {
#include "shared/ocean_turning_platform_child/turning_platform_child_object_points.inc"
};

static NJS_VECTOR turning_platform_child_object_normals[16] = {
#include "shared/ocean_turning_platform_child/turning_platform_child_object_normals.inc"
};

static NJS_MODEL turning_platform_child_object_model = {
#include "shared/ocean_turning_platform_child/turning_platform_child_object_model.inc"
};

#endif
