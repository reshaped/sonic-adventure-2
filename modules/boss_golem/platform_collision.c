/* the collision models of platforms A, B and C, which ride GOLEM_SENSOR_LOW's
 * node */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_golem/platform_collision.h"

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_platform_a_collision_materials[1] = {
#include "boss_golem/platform_collision/golem_platform_a_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_platform_a_collision_faces[47] = {
#include "boss_golem/platform_collision/golem_platform_a_collision_faces.inc"
};

static NJS_MESHSET golem_platform_a_collision_meshes[1] = {
#include "boss_golem/platform_collision/golem_platform_a_collision_meshes.inc"
};

static NJS_POINT3 golem_platform_a_collision_points[18] = {
#include "boss_golem/platform_collision/golem_platform_a_collision_points.inc"
};

static NJS_VECTOR golem_platform_a_collision_normals[18] = {
#include "boss_golem/platform_collision/golem_platform_a_collision_normals.inc"
};

static NJS_MODEL golem_platform_a_collision_model = {
#include "boss_golem/platform_collision/golem_platform_a_collision_model.inc"
};

NJS_OBJECT golem_platform_a_collision = {
#include "boss_golem/platform_collision/golem_platform_a_collision.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_platform_b_collision_materials[1] = {
#include "boss_golem/platform_collision/golem_platform_b_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_platform_b_collision_faces[30] = {
#include "boss_golem/platform_collision/golem_platform_b_collision_faces.inc"
};

static NJS_MESHSET golem_platform_b_collision_meshes[1] = {
#include "boss_golem/platform_collision/golem_platform_b_collision_meshes.inc"
};

static NJS_POINT3 golem_platform_b_collision_points[8] = {
#include "boss_golem/platform_collision/golem_platform_b_collision_points.inc"
};

static NJS_VECTOR golem_platform_b_collision_normals[8] = {
#include "boss_golem/platform_collision/golem_platform_b_collision_normals.inc"
};

static NJS_MODEL golem_platform_b_collision_model = {
#include "boss_golem/platform_collision/golem_platform_b_collision_model.inc"
};

NJS_OBJECT golem_platform_b_collision = {
#include "boss_golem/platform_collision/golem_platform_b_collision.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_platform_c_collision_materials[1] = {
#include "boss_golem/platform_collision/golem_platform_c_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_platform_c_collision_faces[32] = {
#include "boss_golem/platform_collision/golem_platform_c_collision_faces.inc"
};

static NJS_MESHSET golem_platform_c_collision_meshes[1] = {
#include "boss_golem/platform_collision/golem_platform_c_collision_meshes.inc"
};

static NJS_POINT3 golem_platform_c_collision_points[12] = {
#include "boss_golem/platform_collision/golem_platform_c_collision_points.inc"
};

static NJS_VECTOR golem_platform_c_collision_normals[12] = {
#include "boss_golem/platform_collision/golem_platform_c_collision_normals.inc"
};

static NJS_MODEL golem_platform_c_collision_model = {
#include "boss_golem/platform_collision/golem_platform_c_collision_model.inc"
};

NJS_OBJECT golem_platform_c_collision = {
#include "boss_golem/platform_collision/golem_platform_c_collision.inc"
};
