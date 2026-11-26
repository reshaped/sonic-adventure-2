/* the collision models of golem_collision.h. the twelve unregistered parts are
 * models that no code reads */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/golem_collision.h"

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL golem_sensor_material[1] = {
#include "shared/golem_sensors/golem_sensor_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 golem_head_model_collision_faces[19] = {
#include "shared/golem_sensors/golem_head_model_collision_faces.inc"
};

static NJS_MESHSET golem_head_model_collision_meshes[1] = {
#include "shared/golem_sensors/golem_head_model_collision_meshes.inc"
};

static NJS_POINT3 golem_head_model_collision_points[9] = {
#include "shared/golem_sensors/golem_head_model_collision_points.inc"
};

static NJS_VECTOR golem_head_model_collision_normals[9] = {
#include "shared/golem_sensors/golem_head_model_collision_normals.inc"
};

static NJS_MODEL golem_head_model_collision_model = {
#include "shared/golem_sensors/golem_head_model_collision_model.inc"
};

NJS_OBJECT golem_head_model_collision = {
#include "shared/golem_sensors/golem_head_model_collision.inc"
};

static Sint16 golem_sensor_head_faces[215] = {
#include "shared/golem_sensors/golem_sensor_head_faces.inc"
};

static NJS_MESHSET golem_sensor_head_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_head_meshes.inc"
};

static NJS_POINT3 golem_sensor_head_points[56] = {
#include "shared/golem_sensors/golem_sensor_head_points.inc"
};

static NJS_VECTOR golem_sensor_head_normals[56] = {
#include "shared/golem_sensors/golem_sensor_head_normals.inc"
};

static NJS_MODEL golem_sensor_head_model = {
#include "shared/golem_sensors/golem_sensor_head_model.inc"
};

NJS_OBJECT golem_sensor_head = {
#include "shared/golem_sensors/golem_sensor_head.inc"
};

static Sint16 unregistered_part_1_faces[23] = {
#include "shared/golem_sensors/unregistered_part_1_faces.inc"
};

static NJS_MESHSET unregistered_part_1_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_1_meshes.inc"
};

static NJS_POINT3 unregistered_part_1_points[9] = {
#include "shared/golem_sensors/unregistered_part_1_points.inc"
};

static NJS_VECTOR unregistered_part_1_normals[9] = {
#include "shared/golem_sensors/unregistered_part_1_normals.inc"
};

static NJS_MODEL unregistered_part_1_model = {
#include "shared/golem_sensors/unregistered_part_1_model.inc"
};

static NJS_OBJECT unregistered_part_1 = {
#include "shared/golem_sensors/unregistered_part_1.inc"
};

static Sint16 unregistered_part_2_faces[25] = {
#include "shared/golem_sensors/unregistered_part_2_faces.inc"
};

static NJS_MESHSET unregistered_part_2_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_2_meshes.inc"
};

static NJS_POINT3 unregistered_part_2_points[10] = {
#include "shared/golem_sensors/unregistered_part_2_points.inc"
};

static NJS_VECTOR unregistered_part_2_normals[10] = {
#include "shared/golem_sensors/unregistered_part_2_normals.inc"
};

static NJS_MODEL unregistered_part_2_model = {
#include "shared/golem_sensors/unregistered_part_2_model.inc"
};

static NJS_OBJECT unregistered_part_2 = {
#include "shared/golem_sensors/unregistered_part_2.inc"
};

static Sint16 unregistered_part_3_faces[21] = {
#include "shared/golem_sensors/unregistered_part_3_faces.inc"
};

static NJS_MESHSET unregistered_part_3_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_3_meshes.inc"
};

static NJS_POINT3 unregistered_part_3_points[8] = {
#include "shared/golem_sensors/unregistered_part_3_points.inc"
};

static NJS_VECTOR unregistered_part_3_normals[8] = {
#include "shared/golem_sensors/unregistered_part_3_normals.inc"
};

static NJS_MODEL unregistered_part_3_model = {
#include "shared/golem_sensors/unregistered_part_3_model.inc"
};

static NJS_OBJECT unregistered_part_3 = {
#include "shared/golem_sensors/unregistered_part_3.inc"
};

static Sint16 unregistered_part_4_faces[32] = {
#include "shared/golem_sensors/unregistered_part_4_faces.inc"
};

static NJS_MESHSET unregistered_part_4_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_4_meshes.inc"
};

static NJS_POINT3 unregistered_part_4_points[12] = {
#include "shared/golem_sensors/unregistered_part_4_points.inc"
};

static NJS_VECTOR unregistered_part_4_normals[12] = {
#include "shared/golem_sensors/unregistered_part_4_normals.inc"
};

static NJS_MODEL unregistered_part_4_model = {
#include "shared/golem_sensors/unregistered_part_4_model.inc"
};

static NJS_OBJECT unregistered_part_4 = {
#include "shared/golem_sensors/unregistered_part_4.inc"
};

static Sint16 golem_sensor_hand_b_faces[53] = {
#include "shared/golem_sensors/golem_sensor_hand_b_faces.inc"
};

static NJS_MESHSET golem_sensor_hand_b_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_hand_b_meshes.inc"
};

static NJS_POINT3 golem_sensor_hand_b_points[15] = {
#include "shared/golem_sensors/golem_sensor_hand_b_points.inc"
};

static NJS_VECTOR golem_sensor_hand_b_normals[15] = {
#include "shared/golem_sensors/golem_sensor_hand_b_normals.inc"
};

static NJS_MODEL golem_sensor_hand_b_model = {
#include "shared/golem_sensors/golem_sensor_hand_b_model.inc"
};

NJS_OBJECT golem_sensor_hand_b = {
#include "shared/golem_sensors/golem_sensor_hand_b.inc"
};

static Sint16 golem_sensor_forearm_b_faces[89] = {
#include "shared/golem_sensors/golem_sensor_forearm_b_faces.inc"
};

static NJS_MESHSET golem_sensor_forearm_b_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_forearm_b_meshes.inc"
};

static NJS_POINT3 golem_sensor_forearm_b_points[33] = {
#include "shared/golem_sensors/golem_sensor_forearm_b_points.inc"
};

static NJS_VECTOR golem_sensor_forearm_b_normals[33] = {
#include "shared/golem_sensors/golem_sensor_forearm_b_normals.inc"
};

static NJS_MODEL golem_sensor_forearm_b_model = {
#include "shared/golem_sensors/golem_sensor_forearm_b_model.inc"
};

NJS_OBJECT golem_sensor_forearm_b = {
#include "shared/golem_sensors/golem_sensor_forearm_b.inc"
};

static Sint16 golem_sensor_upper_arm_b_faces[78] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_b_faces.inc"
};

static NJS_MESHSET golem_sensor_upper_arm_b_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_b_meshes.inc"
};

static NJS_POINT3 golem_sensor_upper_arm_b_points[32] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_b_points.inc"
};

static NJS_VECTOR golem_sensor_upper_arm_b_normals[32] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_b_normals.inc"
};

static NJS_MODEL golem_sensor_upper_arm_b_model = {
#include "shared/golem_sensors/golem_sensor_upper_arm_b_model.inc"
};

NJS_OBJECT golem_sensor_upper_arm_b = {
#include "shared/golem_sensors/golem_sensor_upper_arm_b.inc"
};

static Sint16 unregistered_part_5_faces[25] = {
#include "shared/golem_sensors/unregistered_part_5_faces.inc"
};

static NJS_MESHSET unregistered_part_5_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_5_meshes.inc"
};

static NJS_POINT3 unregistered_part_5_points[10] = {
#include "shared/golem_sensors/unregistered_part_5_points.inc"
};

static NJS_VECTOR unregistered_part_5_normals[10] = {
#include "shared/golem_sensors/unregistered_part_5_normals.inc"
};

static NJS_MODEL unregistered_part_5_model = {
#include "shared/golem_sensors/unregistered_part_5_model.inc"
};

static NJS_OBJECT unregistered_part_5 = {
#include "shared/golem_sensors/unregistered_part_5.inc"
};

static Sint16 unregistered_part_6_faces[21] = {
#include "shared/golem_sensors/unregistered_part_6_faces.inc"
};

static NJS_MESHSET unregistered_part_6_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_6_meshes.inc"
};

static NJS_POINT3 unregistered_part_6_points[8] = {
#include "shared/golem_sensors/unregistered_part_6_points.inc"
};

static NJS_VECTOR unregistered_part_6_normals[8] = {
#include "shared/golem_sensors/unregistered_part_6_normals.inc"
};

static NJS_MODEL unregistered_part_6_model = {
#include "shared/golem_sensors/unregistered_part_6_model.inc"
};

static NJS_OBJECT unregistered_part_6 = {
#include "shared/golem_sensors/unregistered_part_6.inc"
};

static Sint16 unregistered_part_7_faces[21] = {
#include "shared/golem_sensors/unregistered_part_7_faces.inc"
};

static NJS_MESHSET unregistered_part_7_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_7_meshes.inc"
};

static NJS_POINT3 unregistered_part_7_points[8] = {
#include "shared/golem_sensors/unregistered_part_7_points.inc"
};

static NJS_VECTOR unregistered_part_7_normals[8] = {
#include "shared/golem_sensors/unregistered_part_7_normals.inc"
};

static NJS_MODEL unregistered_part_7_model = {
#include "shared/golem_sensors/unregistered_part_7_model.inc"
};

static NJS_OBJECT unregistered_part_7 = {
#include "shared/golem_sensors/unregistered_part_7.inc"
};

static Sint16 unregistered_part_8_faces[35] = {
#include "shared/golem_sensors/unregistered_part_8_faces.inc"
};

static NJS_MESHSET unregistered_part_8_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_8_meshes.inc"
};

static NJS_POINT3 unregistered_part_8_points[12] = {
#include "shared/golem_sensors/unregistered_part_8_points.inc"
};

static NJS_VECTOR unregistered_part_8_normals[12] = {
#include "shared/golem_sensors/unregistered_part_8_normals.inc"
};

static NJS_MODEL unregistered_part_8_model = {
#include "shared/golem_sensors/unregistered_part_8_model.inc"
};

static NJS_OBJECT unregistered_part_8 = {
#include "shared/golem_sensors/unregistered_part_8.inc"
};

static Sint16 golem_sensor_hand_a_faces[53] = {
#include "shared/golem_sensors/golem_sensor_hand_a_faces.inc"
};

static NJS_MESHSET golem_sensor_hand_a_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_hand_a_meshes.inc"
};

static NJS_POINT3 golem_sensor_hand_a_points[15] = {
#include "shared/golem_sensors/golem_sensor_hand_a_points.inc"
};

static NJS_VECTOR golem_sensor_hand_a_normals[15] = {
#include "shared/golem_sensors/golem_sensor_hand_a_normals.inc"
};

static NJS_MODEL golem_sensor_hand_a_model = {
#include "shared/golem_sensors/golem_sensor_hand_a_model.inc"
};

NJS_OBJECT golem_sensor_hand_a = {
#include "shared/golem_sensors/golem_sensor_hand_a.inc"
};

static Sint16 golem_sensor_forearm_a_faces[89] = {
#include "shared/golem_sensors/golem_sensor_forearm_a_faces.inc"
};

static NJS_MESHSET golem_sensor_forearm_a_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_forearm_a_meshes.inc"
};

static NJS_POINT3 golem_sensor_forearm_a_points[33] = {
#include "shared/golem_sensors/golem_sensor_forearm_a_points.inc"
};

static NJS_VECTOR golem_sensor_forearm_a_normals[33] = {
#include "shared/golem_sensors/golem_sensor_forearm_a_normals.inc"
};

static NJS_MODEL golem_sensor_forearm_a_model = {
#include "shared/golem_sensors/golem_sensor_forearm_a_model.inc"
};

NJS_OBJECT golem_sensor_forearm_a = {
#include "shared/golem_sensors/golem_sensor_forearm_a.inc"
};

static Sint16 golem_sensor_upper_arm_a_faces[75] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_a_faces.inc"
};

static NJS_MESHSET golem_sensor_upper_arm_a_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_a_meshes.inc"
};

static NJS_POINT3 golem_sensor_upper_arm_a_points[32] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_a_points.inc"
};

static NJS_VECTOR golem_sensor_upper_arm_a_normals[32] = {
#include "shared/golem_sensors/golem_sensor_upper_arm_a_normals.inc"
};

static NJS_MODEL golem_sensor_upper_arm_a_model = {
#include "shared/golem_sensors/golem_sensor_upper_arm_a_model.inc"
};

NJS_OBJECT golem_sensor_upper_arm_a = {
#include "shared/golem_sensors/golem_sensor_upper_arm_a.inc"
};

static Sint16 golem_sensor_low_faces[226] = {
#include "shared/golem_sensors/golem_sensor_low_faces.inc"
};

static NJS_MESHSET golem_sensor_low_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_low_meshes.inc"
};

static NJS_POINT3 golem_sensor_low_points[82] = {
#include "shared/golem_sensors/golem_sensor_low_points.inc"
};

static NJS_VECTOR golem_sensor_low_normals[82] = {
#include "shared/golem_sensors/golem_sensor_low_normals.inc"
};

static NJS_MODEL golem_sensor_low_model = {
#include "shared/golem_sensors/golem_sensor_low_model.inc"
};

NJS_OBJECT golem_sensor_low = {
#include "shared/golem_sensors/golem_sensor_low.inc"
};

static NJS_OBJECT unregistered_part_9 = {
#include "shared/golem_sensors/unregistered_part_9.inc"
};

static Sint16 unregistered_part_10_faces[19] = {
#include "shared/golem_sensors/unregistered_part_10_faces.inc"
};

static NJS_MESHSET unregistered_part_10_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_10_meshes.inc"
};

static NJS_POINT3 unregistered_part_10_points[16] = {
#include "shared/golem_sensors/unregistered_part_10_points.inc"
};

static NJS_VECTOR unregistered_part_10_normals[16] = {
#include "shared/golem_sensors/unregistered_part_10_normals.inc"
};

static NJS_MODEL unregistered_part_10_model = {
#include "shared/golem_sensors/unregistered_part_10_model.inc"
};

static NJS_OBJECT unregistered_part_10 = {
#include "shared/golem_sensors/unregistered_part_10.inc"
};

static Sint16 unregistered_part_11_faces[40] = {
#include "shared/golem_sensors/unregistered_part_11_faces.inc"
};

static NJS_MESHSET unregistered_part_11_meshes[1] = {
#include "shared/golem_sensors/unregistered_part_11_meshes.inc"
};

static NJS_POINT3 unregistered_part_11_points[16] = {
#include "shared/golem_sensors/unregistered_part_11_points.inc"
};

static NJS_VECTOR unregistered_part_11_normals[16] = {
#include "shared/golem_sensors/unregistered_part_11_normals.inc"
};

static NJS_MODEL unregistered_part_11_model = {
#include "shared/golem_sensors/unregistered_part_11_model.inc"
};

static NJS_OBJECT unregistered_part_11 = {
#include "shared/golem_sensors/unregistered_part_11.inc"
};

static Sint16 golem_sensor_full_faces[107] = {
#include "shared/golem_sensors/golem_sensor_full_faces.inc"
};

static NJS_MESHSET golem_sensor_full_meshes[1] = {
#include "shared/golem_sensors/golem_sensor_full_meshes.inc"
};

static NJS_POINT3 golem_sensor_full_points[37] = {
#include "shared/golem_sensors/golem_sensor_full_points.inc"
};

static NJS_VECTOR golem_sensor_full_normals[37] = {
#include "shared/golem_sensors/golem_sensor_full_normals.inc"
};

static NJS_MODEL golem_sensor_full_model = {
#include "shared/golem_sensors/golem_sensor_full_model.inc"
};

NJS_OBJECT golem_sensor_full = {
#include "shared/golem_sensors/golem_sensor_full.inc"
};

static NJS_OBJECT unregistered_part_12 = {
#include "shared/golem_sensors/unregistered_part_12.inc"
};
