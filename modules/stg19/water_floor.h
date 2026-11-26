/* Water below the rival arena subtracts a life on contact. */
#ifndef STG19_WATER_FLOOR_H
#define STG19_WATER_FLOOR_H

#include <ninja.h>
#include <sg_xpt.h>

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL water_floor_material = {
#include "stg19/water_floor/water_floor_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 water_floor_indices[5] = {
#include "stg19/water_floor/water_floor_indices.inc"
};

static NJS_MESHSET water_floor_meshset = {
#include "stg19/water_floor/water_floor_meshset.inc"
};

static NJS_POINT3 water_floor_points[4] = {
#include "stg19/water_floor/water_floor_points.inc"
};

static NJS_VECTOR water_floor_normals[4] = {
#include "stg19/water_floor/water_floor_normals.inc"
};

static NJS_MODEL water_floor_model = {
#include "stg19/water_floor/water_floor_model.inc"
};

static NJS_OBJECT water_floor = {
#include "stg19/water_floor/water_floor.inc"
};

#endif
