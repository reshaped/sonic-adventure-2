#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgkinder/door_art.h"

/* clang-format off */
static Sint16 classroom_door_leaf_polygons[219] = {
#include "chaostgkinder/door_models/classroom_door_leaf_polygons.inc"
};

static Sint32 classroom_door_leaf_vertices[195] = {
#include "chaostgkinder/door_models/classroom_door_leaf_vertices.inc"
};

static NJS_CNK_MODEL classroom_door_leaf_mesh = {
#include "chaostgkinder/door_models/classroom_door_leaf_mesh.inc"
};

static NJS_CNK_OBJECT classroom_door_leaf = {
#include "chaostgkinder/door_models/classroom_door_leaf.inc"
};

static Sint16 classroom_door_frame_polygons[297] = {
#include "chaostgkinder/door_models/classroom_door_frame_polygons.inc"
};

static Sint32 classroom_door_frame_vertices[291] = {
#include "chaostgkinder/door_models/classroom_door_frame_vertices.inc"
};

static NJS_CNK_MODEL classroom_door_frame_mesh = {
#include "chaostgkinder/door_models/classroom_door_frame_mesh.inc"
};

NJS_CNK_OBJECT kinder_classroom_door = {
#include "chaostgkinder/door_models/kinder_classroom_door.inc"
};

static Sint16 corridor_door_leaf_b_polygons[229] = {
#include "chaostgkinder/door_models/corridor_door_leaf_b_polygons.inc"
};

static Sint32 corridor_door_leaf_b_vertices[159] = {
#include "chaostgkinder/door_models/corridor_door_leaf_b_vertices.inc"
};

static NJS_CNK_MODEL corridor_door_leaf_b_mesh = {
#include "chaostgkinder/door_models/corridor_door_leaf_b_mesh.inc"
};

static NJS_CNK_OBJECT corridor_door_leaf_b = {
#include "chaostgkinder/door_models/corridor_door_leaf_b.inc"
};

static Sint16 corridor_door_leaf_a_polygons[231] = {
#include "chaostgkinder/door_models/corridor_door_leaf_a_polygons.inc"
};

static Sint32 corridor_door_leaf_a_vertices[159] = {
#include "chaostgkinder/door_models/corridor_door_leaf_a_vertices.inc"
};

static NJS_CNK_MODEL corridor_door_leaf_a_mesh = {
#include "chaostgkinder/door_models/corridor_door_leaf_a_mesh.inc"
};

static NJS_CNK_OBJECT corridor_door_leaf_a = {
#include "chaostgkinder/door_models/corridor_door_leaf_a.inc"
};

static Sint16 corridor_door_frame_polygons[289] = {
#include "chaostgkinder/door_models/corridor_door_frame_polygons.inc"
};

static Sint32 corridor_door_frame_vertices[291] = {
#include "chaostgkinder/door_models/corridor_door_frame_vertices.inc"
};

static NJS_CNK_MODEL corridor_door_frame_mesh = {
#include "chaostgkinder/door_models/corridor_door_frame_mesh.inc"
};

NJS_CNK_OBJECT kinder_corridor_door = {
#include "chaostgkinder/door_models/kinder_corridor_door.inc"
};

static Sint16 doctor_door_leaf_polygons[219] = {
#include "chaostgkinder/door_models/doctor_door_leaf_polygons.inc"
};

static Sint32 doctor_door_leaf_vertices[195] = {
#include "chaostgkinder/door_models/doctor_door_leaf_vertices.inc"
};

static NJS_CNK_MODEL doctor_door_leaf_mesh = {
#include "chaostgkinder/door_models/doctor_door_leaf_mesh.inc"
};

static NJS_CNK_OBJECT doctor_door_leaf = {
#include "chaostgkinder/door_models/doctor_door_leaf.inc"
};

static Sint16 doctor_door_frame_polygons[297] = {
#include "chaostgkinder/door_models/doctor_door_frame_polygons.inc"
};

static Sint32 doctor_door_frame_vertices[291] = {
#include "chaostgkinder/door_models/doctor_door_frame_vertices.inc"
};

static NJS_CNK_MODEL doctor_door_frame_mesh = {
#include "chaostgkinder/door_models/doctor_door_frame_mesh.inc"
};

NJS_CNK_OBJECT kinder_doctor_door = {
#include "chaostgkinder/door_models/kinder_doctor_door.inc"
};

static Sint16 principal_door_leaf_polygons[227] = {
#include "chaostgkinder/door_models/principal_door_leaf_polygons.inc"
};

static Sint32 principal_door_leaf_vertices[195] = {
#include "chaostgkinder/door_models/principal_door_leaf_vertices.inc"
};

static NJS_CNK_MODEL principal_door_leaf_mesh = {
#include "chaostgkinder/door_models/principal_door_leaf_mesh.inc"
};

static NJS_CNK_OBJECT principal_door_leaf = {
#include "chaostgkinder/door_models/principal_door_leaf.inc"
};

static Sint16 principal_door_frame_polygons[297] = {
#include "chaostgkinder/door_models/principal_door_frame_polygons.inc"
};

static Sint32 principal_door_frame_vertices[291] = {
#include "chaostgkinder/door_models/principal_door_frame_vertices.inc"
};

static NJS_CNK_MODEL principal_door_frame_mesh = {
#include "chaostgkinder/door_models/principal_door_frame_mesh.inc"
};

NJS_CNK_OBJECT kinder_principal_door = {
#include "chaostgkinder/door_models/kinder_principal_door.inc"
};

static Sint16 house_door_leaf_b_polygons[307] = {
#include "chaostgkinder/door_models/house_door_leaf_b_polygons.inc"
};

static Sint32 house_door_leaf_b_vertices[127] = {
#include "chaostgkinder/door_models/house_door_leaf_b_vertices.inc"
};

static NJS_CNK_MODEL house_door_leaf_b_mesh = {
#include "chaostgkinder/door_models/house_door_leaf_b_mesh.inc"
};

static NJS_CNK_OBJECT house_door_leaf_b = {
#include "chaostgkinder/door_models/house_door_leaf_b.inc"
};

static Sint16 house_door_leaf_a_polygons[321] = {
#include "chaostgkinder/door_models/house_door_leaf_a_polygons.inc"
};

static Sint32 house_door_leaf_a_vertices[127] = {
#include "chaostgkinder/door_models/house_door_leaf_a_vertices.inc"
};

static NJS_CNK_MODEL house_door_leaf_a_mesh = {
#include "chaostgkinder/door_models/house_door_leaf_a_mesh.inc"
};

static NJS_CNK_OBJECT house_door_leaf_a = {
#include "chaostgkinder/door_models/house_door_leaf_a.inc"
};

static Sint16 house_door_frame_polygons[263] = {
#include "chaostgkinder/door_models/house_door_frame_polygons.inc"
};

static Sint32 house_door_frame_vertices[203] = {
#include "chaostgkinder/door_models/house_door_frame_vertices.inc"
};

static NJS_CNK_MODEL house_door_frame_mesh = {
#include "chaostgkinder/door_models/house_door_frame_mesh.inc"
};

NJS_CNK_OBJECT kinder_house_door = {
#include "chaostgkinder/door_models/kinder_house_door.inc"
};

/* clang-format on */
