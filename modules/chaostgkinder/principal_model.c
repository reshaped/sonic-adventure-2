#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgkinder/character_art.h"

/* clang-format off */
static Sint16 principal_wing_b_polygons[133] = {
#include "chaostgkinder/principal_model/principal_wing_b_polygons.inc"
};

static Sint32 principal_wing_b_vertices[147] = {
#include "chaostgkinder/principal_model/principal_wing_b_vertices.inc"
};

static NJS_CNK_MODEL principal_wing_b_mesh = {
#include "chaostgkinder/principal_model/principal_wing_b_mesh.inc"
};

static NJS_CNK_OBJECT principal_wing_b = {
#include "chaostgkinder/principal_model/principal_wing_b.inc"
};

static Sint16 principal_wing_a_polygons[133] = {
#include "chaostgkinder/principal_model/principal_wing_a_polygons.inc"
};

static Sint32 principal_wing_a_vertices[147] = {
#include "chaostgkinder/principal_model/principal_wing_a_vertices.inc"
};

static NJS_CNK_MODEL principal_wing_a_mesh = {
#include "chaostgkinder/principal_model/principal_wing_a_mesh.inc"
};

static NJS_CNK_OBJECT principal_wing_a = {
#include "chaostgkinder/principal_model/principal_wing_a.inc"
};

static Sint16 principal_orb_polygons[249] = {
#include "chaostgkinder/principal_model/principal_orb_polygons.inc"
};

static Sint32 principal_orb_vertices[195] = {
#include "chaostgkinder/principal_model/principal_orb_vertices.inc"
};

static NJS_CNK_MODEL principal_orb_mesh = {
#include "chaostgkinder/principal_model/principal_orb_mesh.inc"
};

static NJS_CNK_OBJECT principal_orb = {
#include "chaostgkinder/principal_model/principal_orb.inc"
};

static Sint16 principal_glasses_polygons[541] = {
#include "chaostgkinder/principal_model/principal_glasses_polygons.inc"
};

static Sint32 principal_glasses_vertices[759] = {
#include "chaostgkinder/principal_model/principal_glasses_vertices.inc"
};

static NJS_CNK_MODEL principal_glasses_mesh = {
#include "chaostgkinder/principal_model/principal_glasses_mesh.inc"
};

static NJS_CNK_OBJECT principal_glasses = {
#include "chaostgkinder/principal_model/principal_glasses.inc"
};

static Sint16 principal_face_center_polygons[125] = {
#include "chaostgkinder/principal_model/principal_face_center_polygons.inc"
};

static Sint32 principal_face_center_vertices[129] = {
#include "chaostgkinder/principal_model/principal_face_center_vertices.inc"
};

static NJS_CNK_MODEL principal_face_center_mesh = {
#include "chaostgkinder/principal_model/principal_face_center_mesh.inc"
};

static NJS_CNK_OBJECT principal_face_center = {
#include "chaostgkinder/principal_model/principal_face_center.inc"
};

static Sint16 principal_face_side_b_polygons[137] = {
#include "chaostgkinder/principal_model/principal_face_side_b_polygons.inc"
};

static Sint32 principal_face_side_b_vertices[159] = {
#include "chaostgkinder/principal_model/principal_face_side_b_vertices.inc"
};

static NJS_CNK_MODEL principal_face_side_b_mesh = {
#include "chaostgkinder/principal_model/principal_face_side_b_mesh.inc"
};

static NJS_CNK_OBJECT principal_face_side_b = {
#include "chaostgkinder/principal_model/principal_face_side_b.inc"
};

static Sint16 principal_face_side_a_polygons[137] = {
#include "chaostgkinder/principal_model/principal_face_side_a_polygons.inc"
};

static Sint32 principal_face_side_a_vertices[159] = {
#include "chaostgkinder/principal_model/principal_face_side_a_vertices.inc"
};

static NJS_CNK_MODEL principal_face_side_a_mesh = {
#include "chaostgkinder/principal_model/principal_face_side_a_mesh.inc"
};

static NJS_CNK_OBJECT principal_face_side_a = {
#include "chaostgkinder/principal_model/principal_face_side_a.inc"
};

static Sint16 principal_head_polygons[623] = {
#include "chaostgkinder/principal_model/principal_head_polygons.inc"
};

static Sint32 principal_head_vertices[483] = {
#include "chaostgkinder/principal_model/principal_head_vertices.inc"
};

static NJS_CNK_MODEL principal_head_mesh = {
#include "chaostgkinder/principal_model/principal_head_mesh.inc"
};

static NJS_CNK_OBJECT principal_head = {
#include "chaostgkinder/principal_model/principal_head.inc"
};

static Sint16 principal_arm_b_polygons[201] = {
#include "chaostgkinder/principal_model/principal_arm_b_polygons.inc"
};

static Sint32 principal_arm_b_vertices[147] = {
#include "chaostgkinder/principal_model/principal_arm_b_vertices.inc"
};

static NJS_CNK_MODEL principal_arm_b_mesh = {
#include "chaostgkinder/principal_model/principal_arm_b_mesh.inc"
};

static NJS_CNK_OBJECT principal_arm_b = {
#include "chaostgkinder/principal_model/principal_arm_b.inc"
};

static Sint16 principal_arm_a_polygons[201] = {
#include "chaostgkinder/principal_model/principal_arm_a_polygons.inc"
};

static Sint32 principal_arm_a_vertices[147] = {
#include "chaostgkinder/principal_model/principal_arm_a_vertices.inc"
};

static NJS_CNK_MODEL principal_arm_a_mesh = {
#include "chaostgkinder/principal_model/principal_arm_a_mesh.inc"
};

static NJS_CNK_OBJECT principal_arm_a = {
#include "chaostgkinder/principal_model/principal_arm_a.inc"
};

static Sint16 principal_body_polygons[267] = {
#include "chaostgkinder/principal_model/principal_body_polygons.inc"
};

static Sint32 principal_body_vertices[195] = {
#include "chaostgkinder/principal_model/principal_body_vertices.inc"
};

static NJS_CNK_MODEL principal_body_mesh = {
#include "chaostgkinder/principal_model/principal_body_mesh.inc"
};

static NJS_CNK_OBJECT principal_body = {
#include "chaostgkinder/principal_model/principal_body.inc"
};

static Sint16 principal_station_polygons[211] = {
#include "chaostgkinder/principal_model/principal_station_polygons.inc"
};

static Sint32 principal_station_vertices[171] = {
#include "chaostgkinder/principal_model/principal_station_vertices.inc"
};

static NJS_CNK_MODEL principal_station_mesh = {
#include "chaostgkinder/principal_model/principal_station_mesh.inc"
};

NJS_CNK_OBJECT kinder_principal_model = {
#include "chaostgkinder/principal_model/kinder_principal_model.inc"
};

/* clang-format on */
