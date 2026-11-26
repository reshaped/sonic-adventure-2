#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgkinder/character_art.h"

/* clang-format off */
static Sint16 doctor_orb_polygons[249] = {
#include "chaostgkinder/doctor_model/doctor_orb_polygons.inc"
};

static Sint32 doctor_orb_vertices[195] = {
#include "chaostgkinder/doctor_model/doctor_orb_vertices.inc"
};

static NJS_CNK_MODEL doctor_orb_mesh = {
#include "chaostgkinder/doctor_model/doctor_orb_mesh.inc"
};

static NJS_CNK_OBJECT doctor_orb = {
#include "chaostgkinder/doctor_model/doctor_orb.inc"
};

static Sint16 doctor_eye_b_polygons[269] = {
#include "chaostgkinder/doctor_model/doctor_eye_b_polygons.inc"
};

static Sint32 doctor_eye_b_vertices[159] = {
#include "chaostgkinder/doctor_model/doctor_eye_b_vertices.inc"
};

static NJS_CNK_MODEL doctor_eye_b_mesh = {
#include "chaostgkinder/doctor_model/doctor_eye_b_mesh.inc"
};

static NJS_CNK_OBJECT doctor_eye_b = {
#include "chaostgkinder/doctor_model/doctor_eye_b.inc"
};

static Sint16 doctor_lid_b_polygons[189] = {
#include "chaostgkinder/doctor_model/doctor_lid_b_polygons.inc"
};

static Sint32 doctor_lid_b_vertices[165] = {
#include "chaostgkinder/doctor_model/doctor_lid_b_vertices.inc"
};

static NJS_CNK_MODEL doctor_lid_b_mesh = {
#include "chaostgkinder/doctor_model/doctor_lid_b_mesh.inc"
};

static NJS_CNK_OBJECT doctor_lid_b = {
#include "chaostgkinder/doctor_model/doctor_lid_b.inc"
};

static NJS_CNK_OBJECT doctor_eye_pivot_b = {
#include "chaostgkinder/doctor_model/doctor_eye_pivot_b.inc"
};

static Sint16 doctor_eye_a_polygons[261] = {
#include "chaostgkinder/doctor_model/doctor_eye_a_polygons.inc"
};

static Sint32 doctor_eye_a_vertices[159] = {
#include "chaostgkinder/doctor_model/doctor_eye_a_vertices.inc"
};

static NJS_CNK_MODEL doctor_eye_a_mesh = {
#include "chaostgkinder/doctor_model/doctor_eye_a_mesh.inc"
};

static NJS_CNK_OBJECT doctor_eye_a = {
#include "chaostgkinder/doctor_model/doctor_eye_a.inc"
};

static Sint16 doctor_lid_a_polygons[181] = {
#include "chaostgkinder/doctor_model/doctor_lid_a_polygons.inc"
};

static Sint32 doctor_lid_a_vertices[165] = {
#include "chaostgkinder/doctor_model/doctor_lid_a_vertices.inc"
};

static NJS_CNK_MODEL doctor_lid_a_mesh = {
#include "chaostgkinder/doctor_model/doctor_lid_a_mesh.inc"
};

static NJS_CNK_OBJECT doctor_lid_a = {
#include "chaostgkinder/doctor_model/doctor_lid_a.inc"
};

static NJS_CNK_OBJECT doctor_eye_pivot_a = {
#include "chaostgkinder/doctor_model/doctor_eye_pivot_a.inc"
};

static Sint16 doctor_muzzle_polygons[133] = {
#include "chaostgkinder/doctor_model/doctor_muzzle_polygons.inc"
};

static Sint32 doctor_muzzle_vertices[111] = {
#include "chaostgkinder/doctor_model/doctor_muzzle_vertices.inc"
};

static NJS_CNK_MODEL doctor_muzzle_mesh = {
#include "chaostgkinder/doctor_model/doctor_muzzle_mesh.inc"
};

static NJS_CNK_OBJECT doctor_muzzle = {
#include "chaostgkinder/doctor_model/doctor_muzzle.inc"
};

static Sint16 doctor_ear_b_polygons[441] = {
#include "chaostgkinder/doctor_model/doctor_ear_b_polygons.inc"
};

static Sint32 doctor_ear_b_vertices[267] = {
#include "chaostgkinder/doctor_model/doctor_ear_b_vertices.inc"
};

static NJS_CNK_MODEL doctor_ear_b_mesh = {
#include "chaostgkinder/doctor_model/doctor_ear_b_mesh.inc"
};

static NJS_CNK_OBJECT doctor_ear_b = {
#include "chaostgkinder/doctor_model/doctor_ear_b.inc"
};

static Sint16 doctor_ear_a_polygons[441] = {
#include "chaostgkinder/doctor_model/doctor_ear_a_polygons.inc"
};

static Sint32 doctor_ear_a_vertices[267] = {
#include "chaostgkinder/doctor_model/doctor_ear_a_vertices.inc"
};

static NJS_CNK_MODEL doctor_ear_a_mesh = {
#include "chaostgkinder/doctor_model/doctor_ear_a_mesh.inc"
};

static NJS_CNK_OBJECT doctor_ear_a = {
#include "chaostgkinder/doctor_model/doctor_ear_a.inc"
};

static Sint16 doctor_headwear_polygons[575] = {
#include "chaostgkinder/doctor_model/doctor_headwear_polygons.inc"
};

static Sint32 doctor_headwear_vertices[741] = {
#include "chaostgkinder/doctor_model/doctor_headwear_vertices.inc"
};

static NJS_CNK_MODEL doctor_headwear_mesh = {
#include "chaostgkinder/doctor_model/doctor_headwear_mesh.inc"
};

static NJS_CNK_OBJECT doctor_headwear = {
#include "chaostgkinder/doctor_model/doctor_headwear.inc"
};

static Sint16 doctor_head_polygons[603] = {
#include "chaostgkinder/doctor_model/doctor_head_polygons.inc"
};

static Sint32 doctor_head_vertices[465] = {
#include "chaostgkinder/doctor_model/doctor_head_vertices.inc"
};

static NJS_CNK_MODEL doctor_head_mesh = {
#include "chaostgkinder/doctor_model/doctor_head_mesh.inc"
};

static NJS_CNK_OBJECT doctor_head = {
#include "chaostgkinder/doctor_model/doctor_head.inc"
};

static Sint16 doctor_foot_b_polygons[175] = {
#include "chaostgkinder/doctor_model/doctor_foot_b_polygons.inc"
};

static Sint32 doctor_foot_b_vertices[135] = {
#include "chaostgkinder/doctor_model/doctor_foot_b_vertices.inc"
};

static NJS_CNK_MODEL doctor_foot_b_mesh = {
#include "chaostgkinder/doctor_model/doctor_foot_b_mesh.inc"
};

static NJS_CNK_OBJECT doctor_foot_b = {
#include "chaostgkinder/doctor_model/doctor_foot_b.inc"
};

static Sint16 doctor_foot_a_polygons[175] = {
#include "chaostgkinder/doctor_model/doctor_foot_a_polygons.inc"
};

static Sint32 doctor_foot_a_vertices[135] = {
#include "chaostgkinder/doctor_model/doctor_foot_a_vertices.inc"
};

static NJS_CNK_MODEL doctor_foot_a_mesh = {
#include "chaostgkinder/doctor_model/doctor_foot_a_mesh.inc"
};

static NJS_CNK_OBJECT doctor_foot_a = {
#include "chaostgkinder/doctor_model/doctor_foot_a.inc"
};

static Sint16 doctor_wing_b_polygons[125] = {
#include "chaostgkinder/doctor_model/doctor_wing_b_polygons.inc"
};

static Sint32 doctor_wing_b_vertices[93] = {
#include "chaostgkinder/doctor_model/doctor_wing_b_vertices.inc"
};

static NJS_CNK_MODEL doctor_wing_b_mesh = {
#include "chaostgkinder/doctor_model/doctor_wing_b_mesh.inc"
};

static NJS_CNK_OBJECT doctor_wing_b = {
#include "chaostgkinder/doctor_model/doctor_wing_b.inc"
};

static Sint16 doctor_wing_a_polygons[125] = {
#include "chaostgkinder/doctor_model/doctor_wing_a_polygons.inc"
};

static Sint32 doctor_wing_a_vertices[93] = {
#include "chaostgkinder/doctor_model/doctor_wing_a_vertices.inc"
};

static NJS_CNK_MODEL doctor_wing_a_mesh = {
#include "chaostgkinder/doctor_model/doctor_wing_a_mesh.inc"
};

static NJS_CNK_OBJECT doctor_wing_a = {
#include "chaostgkinder/doctor_model/doctor_wing_a.inc"
};

static Sint16 doctor_arm_b_polygons[215] = {
#include "chaostgkinder/doctor_model/doctor_arm_b_polygons.inc"
};

static Sint32 doctor_arm_b_vertices[147] = {
#include "chaostgkinder/doctor_model/doctor_arm_b_vertices.inc"
};

static NJS_CNK_MODEL doctor_arm_b_mesh = {
#include "chaostgkinder/doctor_model/doctor_arm_b_mesh.inc"
};

static NJS_CNK_OBJECT doctor_arm_b = {
#include "chaostgkinder/doctor_model/doctor_arm_b.inc"
};

static Sint16 doctor_arm_a_polygons[345] = {
#include "chaostgkinder/doctor_model/doctor_arm_a_polygons.inc"
};

static Sint32 doctor_arm_a_vertices[273] = {
#include "chaostgkinder/doctor_model/doctor_arm_a_vertices.inc"
};

static NJS_CNK_MODEL doctor_arm_a_mesh = {
#include "chaostgkinder/doctor_model/doctor_arm_a_mesh.inc"
};

static NJS_CNK_OBJECT doctor_arm_a = {
#include "chaostgkinder/doctor_model/doctor_arm_a.inc"
};

static Sint16 doctor_body_polygons[267] = {
#include "chaostgkinder/doctor_model/doctor_body_polygons.inc"
};

static Sint32 doctor_body_vertices[195] = {
#include "chaostgkinder/doctor_model/doctor_body_vertices.inc"
};

static NJS_CNK_MODEL doctor_body_mesh = {
#include "chaostgkinder/doctor_model/doctor_body_mesh.inc"
};

static NJS_CNK_OBJECT doctor_body = {
#include "chaostgkinder/doctor_model/doctor_body.inc"
};

static Sint16 doctor_station_polygons[1115] = {
#include "chaostgkinder/doctor_model/doctor_station_polygons.inc"
};

static Sint32 doctor_station_vertices[1029] = {
#include "chaostgkinder/doctor_model/doctor_station_vertices.inc"
};

static NJS_CNK_MODEL doctor_station_mesh = {
#include "chaostgkinder/doctor_model/doctor_station_mesh.inc"
};

NJS_CNK_OBJECT kinder_doctor_model = {
#include "chaostgkinder/doctor_model/kinder_doctor_model.inc"
};

/* clang-format on */
