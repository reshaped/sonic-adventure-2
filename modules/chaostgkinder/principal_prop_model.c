#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgkinder/character_art.h"

/* clang-format off */
static Sint16 principal_prop_orb_polygons[257] = {
#include "chaostgkinder/principal_prop_model/principal_prop_orb_polygons.inc"
};

static Sint32 principal_prop_orb_vertices[195] = {
#include "chaostgkinder/principal_prop_model/principal_prop_orb_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_orb_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_orb_mesh.inc"
};

static NJS_CNK_OBJECT principal_prop_orb = {
#include "chaostgkinder/principal_prop_model/principal_prop_orb.inc"
};

static Sint16 principal_prop_face_polygons[223] = {
#include "chaostgkinder/principal_prop_model/principal_prop_face_polygons.inc"
};

static Sint32 principal_prop_face_vertices[231] = {
#include "chaostgkinder/principal_prop_model/principal_prop_face_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_face_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_face_mesh.inc"
};

static NJS_CNK_OBJECT principal_prop_face = {
#include "chaostgkinder/principal_prop_model/principal_prop_face.inc"
};

static Sint16 principal_prop_eye_a_polygons[275] = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_a_polygons.inc"
};

static Sint32 principal_prop_eye_a_vertices[159] = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_a_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_eye_a_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_a_mesh.inc"
};

static NJS_CNK_OBJECT principal_prop_eye_a = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_a.inc"
};

static Sint16 principal_prop_lid_a_polygons[175] = {
#include "chaostgkinder/principal_prop_model/principal_prop_lid_a_polygons.inc"
};

static Sint32 principal_prop_lid_a_vertices[165] = {
#include "chaostgkinder/principal_prop_model/principal_prop_lid_a_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_lid_a_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_lid_a_mesh.inc"
};

NJS_CNK_OBJECT kinder_prop_pivot_a = {
#include "chaostgkinder/principal_prop_model/kinder_prop_pivot_a.inc"
};

static NJS_CNK_OBJECT principal_prop_eye_pivot_a = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_pivot_a.inc"
};

static Sint16 principal_prop_eye_b_polygons[269] = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_b_polygons.inc"
};

static Sint32 principal_prop_eye_b_vertices[159] = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_b_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_eye_b_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_b_mesh.inc"
};

static NJS_CNK_OBJECT principal_prop_eye_b = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_b.inc"
};

static Sint16 principal_prop_lid_b_polygons[161] = {
#include "chaostgkinder/principal_prop_model/principal_prop_lid_b_polygons.inc"
};

static Sint32 principal_prop_lid_b_vertices[165] = {
#include "chaostgkinder/principal_prop_model/principal_prop_lid_b_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_lid_b_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_lid_b_mesh.inc"
};

NJS_CNK_OBJECT kinder_prop_pivot_b = {
#include "chaostgkinder/principal_prop_model/kinder_prop_pivot_b.inc"
};

static NJS_CNK_OBJECT principal_prop_eye_pivot_b = {
#include "chaostgkinder/principal_prop_model/principal_prop_eye_pivot_b.inc"
};

static Sint16 principal_prop_head_polygons[595] = {
#include "chaostgkinder/principal_prop_model/principal_prop_head_polygons.inc"
};

static Sint32 principal_prop_head_vertices[465] = {
#include "chaostgkinder/principal_prop_model/principal_prop_head_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_head_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_head_mesh.inc"
};

static NJS_CNK_OBJECT principal_prop_head = {
#include "chaostgkinder/principal_prop_model/principal_prop_head.inc"
};

static Sint16 principal_prop_base_polygons[169] = {
#include "chaostgkinder/principal_prop_model/principal_prop_base_polygons.inc"
};

static Sint32 principal_prop_base_vertices[141] = {
#include "chaostgkinder/principal_prop_model/principal_prop_base_vertices.inc"
};

static NJS_CNK_MODEL principal_prop_base_mesh = {
#include "chaostgkinder/principal_prop_model/principal_prop_base_mesh.inc"
};

NJS_CNK_OBJECT kinder_principal_prop = {
#include "chaostgkinder/principal_prop_model/kinder_principal_prop.inc"
};

/* clang-format on */
