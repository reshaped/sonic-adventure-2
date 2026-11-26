/* the collision object of the floor that core_tails.c places. the floor is
 * solid only in land block 0x200, while time runs */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg36/route.h"

static NJS_MATERIAL floor_material = {
#include "stg36/floor_collision/floor_material.inc"
};

static Sint16 floor_faces[14] = {
#include "stg36/floor_collision/floor_faces.inc"
};

static NJS_MESHSET floor_meshset = {
#include "stg36/floor_collision/floor_meshset.inc"
};

static NJS_POINT3 floor_points[12] = {
#include "stg36/floor_collision/floor_points.inc"
};

static NJS_VECTOR floor_normals[12] = {
#include "stg36/floor_collision/floor_normals.inc"
};

static NJS_MODEL floor_model = {
#include "stg36/floor_collision/floor_model.inc"
};

NJS_OBJECT tails_floor_template = {
#include "stg36/floor_collision/tails_floor_template.inc"
};
