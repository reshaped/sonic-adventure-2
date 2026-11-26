#include <ninja.h>
#include <sg_xpt.h>

#include "stg32/scenery_shapes.h"
#include "stg32/scenery/iris/text.inc"

static NJS_TEXNAME herd_iris_names[7] = {
#include "stg32/scenery/iris/herd_iris_names.inc"
};

NJS_TEXLIST herd_iris_texlist = {
#include "stg32/scenery/iris/herd_iris_texlist.inc"
};

static Sint16 herd_iris_object_8_polygons[181] = {
#include "stg32/scenery/iris/herd_iris_object_8_polygons.inc"
};

static Sint32 herd_iris_object_8_vertices[163] = {
#include "stg32/scenery/iris/herd_iris_object_8_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_8_model = {
#include "stg32/scenery/iris/herd_iris_object_8_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_8 = {
#include "stg32/scenery/iris/herd_iris_object_8.inc"
};

static Sint16 herd_iris_object_7_polygons[187] = {
#include "stg32/scenery/iris/herd_iris_object_7_polygons.inc"
};

static Sint32 herd_iris_object_7_vertices[147] = {
#include "stg32/scenery/iris/herd_iris_object_7_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_7_model = {
#include "stg32/scenery/iris/herd_iris_object_7_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_7 = {
#include "stg32/scenery/iris/herd_iris_object_7.inc"
};

static Sint16 herd_iris_object_6_polygons[181] = {
#include "stg32/scenery/iris/herd_iris_object_6_polygons.inc"
};

static Sint32 herd_iris_object_6_vertices[163] = {
#include "stg32/scenery/iris/herd_iris_object_6_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_6_model = {
#include "stg32/scenery/iris/herd_iris_object_6_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_6 = {
#include "stg32/scenery/iris/herd_iris_object_6.inc"
};

static Sint16 herd_iris_object_5_polygons[187] = {
#include "stg32/scenery/iris/herd_iris_object_5_polygons.inc"
};

static Sint32 herd_iris_object_5_vertices[147] = {
#include "stg32/scenery/iris/herd_iris_object_5_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_5_model = {
#include "stg32/scenery/iris/herd_iris_object_5_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_5 = {
#include "stg32/scenery/iris/herd_iris_object_5.inc"
};

static Sint16 herd_iris_object_4_polygons[181] = {
#include "stg32/scenery/iris/herd_iris_object_4_polygons.inc"
};

static Sint32 herd_iris_object_4_vertices[163] = {
#include "stg32/scenery/iris/herd_iris_object_4_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_4_model = {
#include "stg32/scenery/iris/herd_iris_object_4_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_4 = {
#include "stg32/scenery/iris/herd_iris_object_4.inc"
};

static Sint16 herd_iris_object_3_polygons[187] = {
#include "stg32/scenery/iris/herd_iris_object_3_polygons.inc"
};

static Sint32 herd_iris_object_3_vertices[147] = {
#include "stg32/scenery/iris/herd_iris_object_3_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_3_model = {
#include "stg32/scenery/iris/herd_iris_object_3_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_3 = {
#include "stg32/scenery/iris/herd_iris_object_3.inc"
};

static Sint16 herd_iris_object_2_polygons[181] = {
#include "stg32/scenery/iris/herd_iris_object_2_polygons.inc"
};

static Sint32 herd_iris_object_2_vertices[163] = {
#include "stg32/scenery/iris/herd_iris_object_2_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_2_model = {
#include "stg32/scenery/iris/herd_iris_object_2_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_2 = {
#include "stg32/scenery/iris/herd_iris_object_2.inc"
};

static Sint16 herd_iris_object_1_polygons[187] = {
#include "stg32/scenery/iris/herd_iris_object_1_polygons.inc"
};

static Sint32 herd_iris_object_1_vertices[151] = {
#include "stg32/scenery/iris/herd_iris_object_1_vertices.inc"
};

static NJS_CNK_MODEL herd_iris_object_1_model = {
#include "stg32/scenery/iris/herd_iris_object_1_model.inc"
};

static NJS_CNK_OBJECT herd_iris_object_1 = {
#include "stg32/scenery/iris/herd_iris_object_1.inc"
};

NJS_CNK_OBJECT herd_iris_object[1] = {
#include "stg32/scenery/iris/herd_iris_object.inc"
};

static NJS_MATERIAL herd_iris_collision_8_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_8_materials.inc"
};

static Sint16 herd_iris_collision_8_faces[15] = {
#include "stg32/scenery/iris/herd_iris_collision_8_faces.inc"
};

static NJS_MESHSET herd_iris_collision_8_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_8_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_8_points[6] = {
#include "stg32/scenery/iris/herd_iris_collision_8_points.inc"
};

static NJS_VECTOR herd_iris_collision_8_normals[6] = {
#include "stg32/scenery/iris/herd_iris_collision_8_normals.inc"
};

static NJS_MODEL herd_iris_collision_8_model = {
#include "stg32/scenery/iris/herd_iris_collision_8_model.inc"
};

static NJS_OBJECT herd_iris_collision_8 = {
#include "stg32/scenery/iris/herd_iris_collision_8.inc"
};

static NJS_MATERIAL herd_iris_collision_7_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_7_materials.inc"
};

static Sint16 herd_iris_collision_7_faces[27] = {
#include "stg32/scenery/iris/herd_iris_collision_7_faces.inc"
};

static NJS_MESHSET herd_iris_collision_7_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_7_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_7_points[10] = {
#include "stg32/scenery/iris/herd_iris_collision_7_points.inc"
};

static NJS_VECTOR herd_iris_collision_7_normals[10] = {
#include "stg32/scenery/iris/herd_iris_collision_7_normals.inc"
};

static NJS_MODEL herd_iris_collision_7_model = {
#include "stg32/scenery/iris/herd_iris_collision_7_model.inc"
};

static NJS_OBJECT herd_iris_collision_7 = {
#include "stg32/scenery/iris/herd_iris_collision_7.inc"
};

static NJS_MATERIAL herd_iris_collision_6_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_6_materials.inc"
};

static Sint16 herd_iris_collision_6_faces[15] = {
#include "stg32/scenery/iris/herd_iris_collision_6_faces.inc"
};

static NJS_MESHSET herd_iris_collision_6_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_6_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_6_points[6] = {
#include "stg32/scenery/iris/herd_iris_collision_6_points.inc"
};

static NJS_VECTOR herd_iris_collision_6_normals[6] = {
#include "stg32/scenery/iris/herd_iris_collision_6_normals.inc"
};

static NJS_MODEL herd_iris_collision_6_model = {
#include "stg32/scenery/iris/herd_iris_collision_6_model.inc"
};

static NJS_OBJECT herd_iris_collision_6 = {
#include "stg32/scenery/iris/herd_iris_collision_6.inc"
};

static NJS_MATERIAL herd_iris_collision_5_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_5_materials.inc"
};

static Sint16 herd_iris_collision_5_faces[27] = {
#include "stg32/scenery/iris/herd_iris_collision_5_faces.inc"
};

static NJS_MESHSET herd_iris_collision_5_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_5_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_5_points[10] = {
#include "stg32/scenery/iris/herd_iris_collision_5_points.inc"
};

static NJS_VECTOR herd_iris_collision_5_normals[10] = {
#include "stg32/scenery/iris/herd_iris_collision_5_normals.inc"
};

static NJS_MODEL herd_iris_collision_5_model = {
#include "stg32/scenery/iris/herd_iris_collision_5_model.inc"
};

static NJS_OBJECT herd_iris_collision_5 = {
#include "stg32/scenery/iris/herd_iris_collision_5.inc"
};

static NJS_MATERIAL herd_iris_collision_4_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_4_materials.inc"
};

static Sint16 herd_iris_collision_4_faces[15] = {
#include "stg32/scenery/iris/herd_iris_collision_4_faces.inc"
};

static NJS_MESHSET herd_iris_collision_4_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_4_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_4_points[6] = {
#include "stg32/scenery/iris/herd_iris_collision_4_points.inc"
};

static NJS_VECTOR herd_iris_collision_4_normals[6] = {
#include "stg32/scenery/iris/herd_iris_collision_4_normals.inc"
};

static NJS_MODEL herd_iris_collision_4_model = {
#include "stg32/scenery/iris/herd_iris_collision_4_model.inc"
};

static NJS_OBJECT herd_iris_collision_4 = {
#include "stg32/scenery/iris/herd_iris_collision_4.inc"
};

static NJS_MATERIAL herd_iris_collision_3_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_3_materials.inc"
};

static Sint16 herd_iris_collision_3_faces[27] = {
#include "stg32/scenery/iris/herd_iris_collision_3_faces.inc"
};

static NJS_MESHSET herd_iris_collision_3_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_3_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_3_points[10] = {
#include "stg32/scenery/iris/herd_iris_collision_3_points.inc"
};

static NJS_VECTOR herd_iris_collision_3_normals[10] = {
#include "stg32/scenery/iris/herd_iris_collision_3_normals.inc"
};

static NJS_MODEL herd_iris_collision_3_model = {
#include "stg32/scenery/iris/herd_iris_collision_3_model.inc"
};

static NJS_OBJECT herd_iris_collision_3 = {
#include "stg32/scenery/iris/herd_iris_collision_3.inc"
};

static NJS_MATERIAL herd_iris_collision_2_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_2_materials.inc"
};

static Sint16 herd_iris_collision_2_faces[15] = {
#include "stg32/scenery/iris/herd_iris_collision_2_faces.inc"
};

static NJS_MESHSET herd_iris_collision_2_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_2_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_2_points[6] = {
#include "stg32/scenery/iris/herd_iris_collision_2_points.inc"
};

static NJS_VECTOR herd_iris_collision_2_normals[6] = {
#include "stg32/scenery/iris/herd_iris_collision_2_normals.inc"
};

static NJS_MODEL herd_iris_collision_2_model = {
#include "stg32/scenery/iris/herd_iris_collision_2_model.inc"
};

static NJS_OBJECT herd_iris_collision_2 = {
#include "stg32/scenery/iris/herd_iris_collision_2.inc"
};

static NJS_MATERIAL herd_iris_collision_1_materials[1] = {
#include "stg32/scenery/iris/herd_iris_collision_1_materials.inc"
};

static Sint16 herd_iris_collision_1_faces[27] = {
#include "stg32/scenery/iris/herd_iris_collision_1_faces.inc"
};

static NJS_MESHSET herd_iris_collision_1_meshsets[1] = {
#include "stg32/scenery/iris/herd_iris_collision_1_meshsets.inc"
};

static NJS_POINT3 herd_iris_collision_1_points[10] = {
#include "stg32/scenery/iris/herd_iris_collision_1_points.inc"
};

static NJS_VECTOR herd_iris_collision_1_normals[10] = {
#include "stg32/scenery/iris/herd_iris_collision_1_normals.inc"
};

static NJS_MODEL herd_iris_collision_1_model = {
#include "stg32/scenery/iris/herd_iris_collision_1_model.inc"
};

static NJS_OBJECT herd_iris_collision_1 = {
#include "stg32/scenery/iris/herd_iris_collision_1.inc"
};

NJS_OBJECT herd_iris_collision[1] = {
#include "stg32/scenery/iris/herd_iris_collision.inc"
};
