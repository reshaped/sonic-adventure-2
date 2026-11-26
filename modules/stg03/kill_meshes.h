/* The first five surfaces mark water deaths. The final three catch falls. */
#ifndef STG03_KILL_MESHES_H
#define STG03_KILL_MESHES_H

#include <ninja.h>
#include <sg_xpt.h>

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL water_floor_1_material = {
#include "stg03/kill_meshes/water_floor_1_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 water_floor_1_indices[5] = {
#include "stg03/kill_meshes/water_floor_1_indices.inc"
};

static NJS_MESHSET water_floor_1_meshset = {
#include "stg03/kill_meshes/water_floor_1_meshset.inc"
};

static NJS_POINT3 water_floor_1_points[4] = {
#include "stg03/kill_meshes/water_floor_1_points.inc"
};

static NJS_VECTOR water_floor_1_normals[4] = {
#include "stg03/kill_meshes/water_floor_1_normals.inc"
};

static NJS_MODEL water_floor_1_model = {
#include "stg03/kill_meshes/water_floor_1_model.inc"
};

static NJS_OBJECT green_forest_kill_area_0 = {
#include "stg03/kill_meshes/green_forest_kill_area_0.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL water_floor_2_material = {
#include "stg03/kill_meshes/water_floor_2_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 water_floor_2_indices[5] = {
#include "stg03/kill_meshes/water_floor_2_indices.inc"
};

static NJS_MESHSET water_floor_2_meshset = {
#include "stg03/kill_meshes/water_floor_2_meshset.inc"
};

static NJS_POINT3 water_floor_2_points[4] = {
#include "stg03/kill_meshes/water_floor_2_points.inc"
};

static NJS_VECTOR water_floor_2_normals[4] = {
#include "stg03/kill_meshes/water_floor_2_normals.inc"
};

static NJS_MODEL water_floor_2_model = {
#include "stg03/kill_meshes/water_floor_2_model.inc"
};

static NJS_OBJECT green_forest_kill_area_1 = {
#include "stg03/kill_meshes/green_forest_kill_area_1.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL water_floor_3_material = {
#include "stg03/kill_meshes/water_floor_3_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 water_floor_3_indices[5] = {
#include "stg03/kill_meshes/water_floor_3_indices.inc"
};

static NJS_MESHSET water_floor_3_meshset = {
#include "stg03/kill_meshes/water_floor_3_meshset.inc"
};

static NJS_POINT3 water_floor_3_points[4] = {
#include "stg03/kill_meshes/water_floor_3_points.inc"
};

static NJS_VECTOR water_floor_3_normals[4] = {
#include "stg03/kill_meshes/water_floor_3_normals.inc"
};

static NJS_MODEL water_floor_3_model = {
#include "stg03/kill_meshes/water_floor_3_model.inc"
};

static NJS_OBJECT green_forest_kill_area_2 = {
#include "stg03/kill_meshes/green_forest_kill_area_2.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL water_floor_4_material = {
#include "stg03/kill_meshes/water_floor_4_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 water_floor_4_indices[7] = {
#include "stg03/kill_meshes/water_floor_4_indices.inc"
};

static NJS_MESHSET water_floor_4_meshset = {
#include "stg03/kill_meshes/water_floor_4_meshset.inc"
};

static NJS_POINT3 water_floor_4_points[6] = {
#include "stg03/kill_meshes/water_floor_4_points.inc"
};

static NJS_VECTOR water_floor_4_normals[6] = {
#include "stg03/kill_meshes/water_floor_4_normals.inc"
};

static NJS_MODEL water_floor_4_model = {
#include "stg03/kill_meshes/water_floor_4_model.inc"
};

static NJS_OBJECT green_forest_kill_area_3 = {
#include "stg03/kill_meshes/green_forest_kill_area_3.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL water_floor_5_material = {
#include "stg03/kill_meshes/water_floor_5_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 water_floor_5_indices[5] = {
#include "stg03/kill_meshes/water_floor_5_indices.inc"
};

static NJS_MESHSET water_floor_5_meshset = {
#include "stg03/kill_meshes/water_floor_5_meshset.inc"
};

static NJS_POINT3 water_floor_5_points[4] = {
#include "stg03/kill_meshes/water_floor_5_points.inc"
};

static NJS_VECTOR water_floor_5_normals[4] = {
#include "stg03/kill_meshes/water_floor_5_normals.inc"
};

static NJS_MODEL water_floor_5_model = {
#include "stg03/kill_meshes/water_floor_5_model.inc"
};

static NJS_OBJECT green_forest_kill_area_4 = {
#include "stg03/kill_meshes/green_forest_kill_area_4.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL fall_floor_1_material = {
#include "stg03/kill_meshes/fall_floor_1_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 fall_floor_1_indices[5] = {
#include "stg03/kill_meshes/fall_floor_1_indices.inc"
};

static NJS_MESHSET fall_floor_1_meshset = {
#include "stg03/kill_meshes/fall_floor_1_meshset.inc"
};

static NJS_POINT3 fall_floor_1_points[4] = {
#include "stg03/kill_meshes/fall_floor_1_points.inc"
};

static NJS_VECTOR fall_floor_1_normals[4] = {
#include "stg03/kill_meshes/fall_floor_1_normals.inc"
};

static NJS_MODEL fall_floor_1_model = {
#include "stg03/kill_meshes/fall_floor_1_model.inc"
};

static NJS_OBJECT green_forest_kill_area_5 = {
#include "stg03/kill_meshes/green_forest_kill_area_5.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL fall_floor_2_material = {
#include "stg03/kill_meshes/fall_floor_2_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 fall_floor_2_indices[5] = {
#include "stg03/kill_meshes/fall_floor_2_indices.inc"
};

static NJS_MESHSET fall_floor_2_meshset = {
#include "stg03/kill_meshes/fall_floor_2_meshset.inc"
};

static NJS_POINT3 fall_floor_2_points[4] = {
#include "stg03/kill_meshes/fall_floor_2_points.inc"
};

static NJS_VECTOR fall_floor_2_normals[4] = {
#include "stg03/kill_meshes/fall_floor_2_normals.inc"
};

static NJS_MODEL fall_floor_2_model = {
#include "stg03/kill_meshes/fall_floor_2_model.inc"
};

static NJS_OBJECT green_forest_kill_area_6 = {
#include "stg03/kill_meshes/green_forest_kill_area_6.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL fall_floor_3_material = {
#include "stg03/kill_meshes/fall_floor_3_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 fall_floor_3_indices[5] = {
#include "stg03/kill_meshes/fall_floor_3_indices.inc"
};

static NJS_MESHSET fall_floor_3_meshset = {
#include "stg03/kill_meshes/fall_floor_3_meshset.inc"
};

static NJS_POINT3 fall_floor_3_points[4] = {
#include "stg03/kill_meshes/fall_floor_3_points.inc"
};

static NJS_VECTOR fall_floor_3_normals[4] = {
#include "stg03/kill_meshes/fall_floor_3_normals.inc"
};

static NJS_MODEL fall_floor_3_model = {
#include "stg03/kill_meshes/fall_floor_3_model.inc"
};

static NJS_OBJECT green_forest_kill_area_7 = {
#include "stg03/kill_meshes/green_forest_kill_area_7.inc"
};

#endif
