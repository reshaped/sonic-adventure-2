/* the collision models of the three platforms that ride GOLEM_SENSOR_HEAD's
 * node */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_golem/platform_collision.h"

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_head_platform_a_collision_materials[1] = {
#include "boss_golem/head_platform_collision/golem_head_platform_a_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_head_platform_a_collision_faces[41] = {
#include "boss_golem/head_platform_collision/golem_head_platform_a_collision_faces.inc"
};

static NJS_MESHSET golem_head_platform_a_collision_meshes[1] = {
#include "boss_golem/head_platform_collision/golem_head_platform_a_collision_meshes.inc"
};

static NJS_POINT3 golem_head_platform_a_collision_points[20] = {
#include "boss_golem/head_platform_collision/golem_head_platform_a_collision_points.inc"
};

static NJS_VECTOR golem_head_platform_a_collision_normals[20] = {
#include "boss_golem/head_platform_collision/golem_head_platform_a_collision_normals.inc"
};

static NJS_MODEL golem_head_platform_a_collision_model = {
#include "boss_golem/head_platform_collision/golem_head_platform_a_collision_model.inc"
};

NJS_OBJECT golem_head_platform_a_collision = {
#include "boss_golem/head_platform_collision/golem_head_platform_a_collision.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_head_platform_b_collision_materials[1] = {
#include "boss_golem/head_platform_collision/golem_head_platform_b_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_head_platform_b_collision_faces[41] = {
#include "boss_golem/head_platform_collision/golem_head_platform_b_collision_faces.inc"
};

static NJS_MESHSET golem_head_platform_b_collision_meshes[1] = {
#include "boss_golem/head_platform_collision/golem_head_platform_b_collision_meshes.inc"
};

static NJS_POINT3 golem_head_platform_b_collision_points[20] = {
#include "boss_golem/head_platform_collision/golem_head_platform_b_collision_points.inc"
};

static NJS_VECTOR golem_head_platform_b_collision_normals[20] = {
#include "boss_golem/head_platform_collision/golem_head_platform_b_collision_normals.inc"
};

static NJS_MODEL golem_head_platform_b_collision_model = {
#include "boss_golem/head_platform_collision/golem_head_platform_b_collision_model.inc"
};

NJS_OBJECT golem_head_platform_b_collision = {
#include "boss_golem/head_platform_collision/golem_head_platform_b_collision.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_head_platform_c_collision_materials[1] = {
#include "boss_golem/head_platform_collision/golem_head_platform_c_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_head_platform_c_collision_faces[44] = {
#include "boss_golem/head_platform_collision/golem_head_platform_c_collision_faces.inc"
};

static NJS_MESHSET golem_head_platform_c_collision_meshes[1] = {
#include "boss_golem/head_platform_collision/golem_head_platform_c_collision_meshes.inc"
};

static NJS_POINT3 golem_head_platform_c_collision_points[20] = {
#include "boss_golem/head_platform_collision/golem_head_platform_c_collision_points.inc"
};

static NJS_VECTOR golem_head_platform_c_collision_normals[20] = {
#include "boss_golem/head_platform_collision/golem_head_platform_c_collision_normals.inc"
};

static NJS_MODEL golem_head_platform_c_collision_model = {
#include "boss_golem/head_platform_collision/golem_head_platform_c_collision_model.inc"
};

NJS_OBJECT golem_head_platform_c_collision = {
#include "boss_golem/head_platform_collision/golem_head_platform_c_collision.inc"
};
