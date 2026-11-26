#include <ninja.h>
#include <sg_xpt.h>

#include "stg44/scenery_shapes.h"

#include "stg44/scenery/marker/text.inc"

static NJS_TEXNAME mad_space_marker_a_names[6] = {
#include "stg44/scenery/marker/mad_space_marker_a_names.inc"
};

NJS_TEXLIST mad_space_marker_a_texlist = {
#include "stg44/scenery/marker/mad_space_marker_a_texlist.inc"
};

static Sint16 mad_space_marker_a_object_1_polygons[71] = {
#include "stg44/scenery/marker/mad_space_marker_a_object_1_polygons.inc"
};

static Sint32 mad_space_marker_a_object_1_vertices[51] = {
#include "stg44/scenery/marker/mad_space_marker_a_object_1_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_a_object_1_model = {
#include "stg44/scenery/marker/mad_space_marker_a_object_1_model.inc"
};

static NJS_CNK_OBJECT mad_space_marker_a_object_1 = {
#include "stg44/scenery/marker/mad_space_marker_a_object_1.inc"
};

static Sint16 mad_space_marker_a_object_polygons[379] = {
#include "stg44/scenery/marker/mad_space_marker_a_object_polygons.inc"
};

static Sint32 mad_space_marker_a_object_vertices[279] = {
#include "stg44/scenery/marker/mad_space_marker_a_object_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_a_object_model = {
#include "stg44/scenery/marker/mad_space_marker_a_object_model.inc"
};

NJS_CNK_OBJECT mad_space_marker_a_object = {
#include "stg44/scenery/marker/mad_space_marker_a_object.inc"
};

static NJS_TEXNAME mad_space_marker_b_names[6] = {
#include "stg44/scenery/marker/mad_space_marker_b_names.inc"
};

NJS_TEXLIST mad_space_marker_b_texlist = {
#include "stg44/scenery/marker/mad_space_marker_b_texlist.inc"
};

static Sint16 mad_space_marker_b_object_1_polygons[71] = {
#include "stg44/scenery/marker/mad_space_marker_b_object_1_polygons.inc"
};

static Sint32 mad_space_marker_b_object_1_vertices[51] = {
#include "stg44/scenery/marker/mad_space_marker_b_object_1_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_b_object_1_model = {
#include "stg44/scenery/marker/mad_space_marker_b_object_1_model.inc"
};

static NJS_CNK_OBJECT mad_space_marker_b_object_1 = {
#include "stg44/scenery/marker/mad_space_marker_b_object_1.inc"
};

static Sint16 mad_space_marker_b_object_polygons[387] = {
#include "stg44/scenery/marker/mad_space_marker_b_object_polygons.inc"
};

static Sint32 mad_space_marker_b_object_vertices[287] = {
#include "stg44/scenery/marker/mad_space_marker_b_object_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_b_object_model = {
#include "stg44/scenery/marker/mad_space_marker_b_object_model.inc"
};

NJS_CNK_OBJECT mad_space_marker_b_object = {
#include "stg44/scenery/marker/mad_space_marker_b_object.inc"
};

static NJS_TEXNAME mad_space_marker_c_names[6] = {
#include "stg44/scenery/marker/mad_space_marker_c_names.inc"
};

NJS_TEXLIST mad_space_marker_c_texlist = {
#include "stg44/scenery/marker/mad_space_marker_c_texlist.inc"
};

static Sint16 mad_space_marker_c_object_1_polygons[71] = {
#include "stg44/scenery/marker/mad_space_marker_c_object_1_polygons.inc"
};

static Sint32 mad_space_marker_c_object_1_vertices[51] = {
#include "stg44/scenery/marker/mad_space_marker_c_object_1_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_c_object_1_model = {
#include "stg44/scenery/marker/mad_space_marker_c_object_1_model.inc"
};

static NJS_CNK_OBJECT mad_space_marker_c_object_1 = {
#include "stg44/scenery/marker/mad_space_marker_c_object_1.inc"
};

static Sint16 mad_space_marker_c_object_polygons[359] = {
#include "stg44/scenery/marker/mad_space_marker_c_object_polygons.inc"
};

static Sint32 mad_space_marker_c_object_vertices[267] = {
#include "stg44/scenery/marker/mad_space_marker_c_object_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_c_object_model = {
#include "stg44/scenery/marker/mad_space_marker_c_object_model.inc"
};

NJS_CNK_OBJECT mad_space_marker_c_object = {
#include "stg44/scenery/marker/mad_space_marker_c_object.inc"
};

static NJS_TEXNAME mad_space_marker_d_names[6] = {
#include "stg44/scenery/marker/mad_space_marker_d_names.inc"
};

NJS_TEXLIST mad_space_marker_d_texlist = {
#include "stg44/scenery/marker/mad_space_marker_d_texlist.inc"
};

static Sint16 mad_space_marker_d_object_1_polygons[71] = {
#include "stg44/scenery/marker/mad_space_marker_d_object_1_polygons.inc"
};

static Sint32 mad_space_marker_d_object_1_vertices[51] = {
#include "stg44/scenery/marker/mad_space_marker_d_object_1_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_d_object_1_model = {
#include "stg44/scenery/marker/mad_space_marker_d_object_1_model.inc"
};

static NJS_CNK_OBJECT mad_space_marker_d_object_1 = {
#include "stg44/scenery/marker/mad_space_marker_d_object_1.inc"
};

static Sint16 mad_space_marker_d_object_polygons[359] = {
#include "stg44/scenery/marker/mad_space_marker_d_object_polygons.inc"
};

static Sint32 mad_space_marker_d_object_vertices[267] = {
#include "stg44/scenery/marker/mad_space_marker_d_object_vertices.inc"
};

static NJS_CNK_MODEL mad_space_marker_d_object_model = {
#include "stg44/scenery/marker/mad_space_marker_d_object_model.inc"
};

NJS_CNK_OBJECT mad_space_marker_d_object = {
#include "stg44/scenery/marker/mad_space_marker_d_object.inc"
};

static NJS_MATERIAL mad_space_marker_collision_materials[1] = {
#include "stg44/scenery/marker/mad_space_marker_collision_materials.inc"
};

static Sint16 mad_space_marker_collision_faces[36] = {
#include "stg44/scenery/marker/mad_space_marker_collision_faces.inc"
};

static NJS_MESHSET mad_space_marker_collision_meshsets[1] = {
#include "stg44/scenery/marker/mad_space_marker_collision_meshsets.inc"
};

static NJS_POINT3 mad_space_marker_collision_points[22] = {
#include "stg44/scenery/marker/mad_space_marker_collision_points.inc"
};

static NJS_VECTOR mad_space_marker_collision_normals[22] = {
#include "stg44/scenery/marker/mad_space_marker_collision_normals.inc"
};

static NJS_MODEL mad_space_marker_collision_model = {
#include "stg44/scenery/marker/mad_space_marker_collision_model.inc"
};

NJS_OBJECT mad_space_marker_collision = {
#include "stg44/scenery/marker/mad_space_marker_collision.inc"
};
