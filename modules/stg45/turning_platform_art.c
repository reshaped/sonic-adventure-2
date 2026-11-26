#include <ninja.h>
#include <sg_xpt.h>

#include "shared/turning_platform.h"

/* Rendered platform and both collision meshes for the two-player stage. */

static Sint16 turning_platform_polygons[979] = {
#include "stg45/turning_platform_art/turning_platform_polygons.inc"
};

static Sint32 turning_platform_vertices[751] = {
#include "stg45/turning_platform_art/turning_platform_vertices.inc"
};

NJS_CNK_MODEL turning_platform_model = {
#include "stg45/turning_platform_art/turning_platform_model.inc"
};

static NJS_CNK_OBJECT turning_platform_display = {
#include "stg45/turning_platform_art/turning_platform_display.inc"
};

static NJS_MATERIAL turning_platform_object_materials[1] = {
#include "stg45/turning_platform_art/turning_platform_object_materials.inc"
};

static Sint16 turning_platform_object_faces[49] = {
#include "stg45/turning_platform_art/turning_platform_object_faces.inc"
};

static NJS_MESHSET turning_platform_object_meshsets[1] = {
#include "stg45/turning_platform_art/turning_platform_object_meshsets.inc"
};

static NJS_POINT3 turning_platform_object_points[24] = {
#include "stg45/turning_platform_art/turning_platform_object_points.inc"
};

static NJS_VECTOR turning_platform_object_normals[24] = {
#include "stg45/turning_platform_art/turning_platform_object_normals.inc"
};

static NJS_MODEL turning_platform_object_model = {
#include "stg45/turning_platform_art/turning_platform_object_model.inc"
};

NJS_OBJECT turning_platform_object = {
#include "stg45/turning_platform_art/turning_platform_object.inc"
};

#include "shared/ocean_turning_platform_child.h"

NJS_OBJECT turning_platform_child_object = {
#include "stg45/turning_platform_art/turning_platform_child_object.inc"
};
