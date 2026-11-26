#ifndef STG07_SCENERY_WATER_H
#define STG07_SCENERY_WATER_H

/* The animated lagoon surfaces and the deformable wake. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME water_texture_set_2_names[2] = {
#include "stg07/scenery/water/water_texture_set_2_names.inc"
};

static NJS_TEXLIST water_texture_set_2 = {
#include "stg07/scenery/water/water_texture_set_2.inc"
};

static Sint16 water_ring_polygons[87] = {
#include "stg07/scenery/water/water_ring_polygons.inc"
};

static Sint32 water_ring_vertices[47] = {
#include "stg07/scenery/water/water_ring_vertices.inc"
};

static NJS_CNK_MODEL water_ring_model = {
#include "stg07/scenery/water/water_ring_model.inc"
};

static NJS_CNK_OBJECT water_ring_object[1] = {
#include "stg07/scenery/water/water_ring_object.inc"
};

static NJS_TEXNAME water_texture_set_3_names[1] = {
#include "stg07/scenery/water/water_texture_set_3_names.inc"
};

static NJS_TEXLIST water_texture_set_3 = {
#include "stg07/scenery/water/water_texture_set_3.inc"
};

static Sint16 water_plain_polygons[19] = {
#include "stg07/scenery/water/water_plain_polygons.inc"
};

static Sint32 water_plain_vertices[19] = {
#include "stg07/scenery/water/water_plain_vertices.inc"
};

static NJS_CNK_MODEL water_plain_model = {
#include "stg07/scenery/water/water_plain_model.inc"
};

static NJS_CNK_OBJECT water_plain_object[1] = {
#include "stg07/scenery/water/water_plain_object.inc"
};

static NJS_TEXNAME water_texture_set_4_names[1] = {
#include "stg07/scenery/water/water_texture_set_4_names.inc"
};

static NJS_TEXLIST water_texture_set_4 = {
#include "stg07/scenery/water/water_texture_set_4.inc"
};

static Sint16 water_level_0_polygons[139] = {
#include "stg07/scenery/water/water_level_0_polygons.inc"
};

static Sint32 water_level_0_vertices[163] = {
#include "stg07/scenery/water/water_level_0_vertices.inc"
};

static NJS_CNK_MODEL water_level_0_model = {
#include "stg07/scenery/water/water_level_0_model.inc"
};

static NJS_CNK_OBJECT water_level_0_object[1] = {
#include "stg07/scenery/water/water_level_0_object.inc"
};

static NJS_TEXNAME water_texture_set_5_names[1] = {
#include "stg07/scenery/water/water_texture_set_5_names.inc"
};

static NJS_TEXLIST water_texture_set_5 = {
#include "stg07/scenery/water/water_texture_set_5.inc"
};

static Sint16 water_level_1_polygons[137] = {
#include "stg07/scenery/water/water_level_1_polygons.inc"
};

static Sint32 water_level_1_vertices[163] = {
#include "stg07/scenery/water/water_level_1_vertices.inc"
};

static NJS_CNK_MODEL water_level_1_model = {
#include "stg07/scenery/water/water_level_1_model.inc"
};

static NJS_CNK_OBJECT water_level_1_object[1] = {
#include "stg07/scenery/water/water_level_1_object.inc"
};

static NJS_MATERIAL water_piece_0_materials = {
#include "stg07/scenery/water/water_piece_0_materials.inc"
};

static Uint16 water_piece_0_strips[5] = {
#include "stg07/scenery/water/water_piece_0_strips.inc"
};

static NJS_MESHSET water_piece_0_faces = {
#include "stg07/scenery/water/water_piece_0_faces.inc"
};

static NJS_POINT3 water_piece_0_points[4] = {
#include "stg07/scenery/water/water_piece_0_points.inc"
};

static NJS_VECTOR water_piece_0_normals[4] = {
#include "stg07/scenery/water/water_piece_0_normals.inc"
};

static NJS_MODEL water_piece_0_model = {
#include "stg07/scenery/water/water_piece_0_model.inc"
};

static NJS_OBJECT water_piece_0 = {
#include "stg07/scenery/water/water_piece_0.inc"
};

static NJS_MATERIAL water_piece_1_materials = {
#include "stg07/scenery/water/water_piece_1_materials.inc"
};

static Uint16 water_piece_1_strips[44] = {
#include "stg07/scenery/water/water_piece_1_strips.inc"
};

static NJS_MESHSET water_piece_1_faces = {
#include "stg07/scenery/water/water_piece_1_faces.inc"
};

static NJS_POINT3 water_piece_1_points[25] = {
#include "stg07/scenery/water/water_piece_1_points.inc"
};

static NJS_VECTOR water_piece_1_normals[25] = {
#include "stg07/scenery/water/water_piece_1_normals.inc"
};

static NJS_MODEL water_piece_1_model = {
#include "stg07/scenery/water/water_piece_1_model.inc"
};

static NJS_OBJECT water_piece_1 = {
#include "stg07/scenery/water/water_piece_1.inc"
};

static NJS_MATERIAL water_piece_2_materials = {
#include "stg07/scenery/water/water_piece_2_materials.inc"
};

static Uint16 water_piece_2_strips[26] = {
#include "stg07/scenery/water/water_piece_2_strips.inc"
};

static NJS_MESHSET water_piece_2_faces = {
#include "stg07/scenery/water/water_piece_2_faces.inc"
};

static NJS_POINT3 water_piece_2_points[18] = {
#include "stg07/scenery/water/water_piece_2_points.inc"
};

static NJS_VECTOR water_piece_2_normals[18] = {
#include "stg07/scenery/water/water_piece_2_normals.inc"
};

static NJS_MODEL water_piece_2_model = {
#include "stg07/scenery/water/water_piece_2_model.inc"
};

static NJS_OBJECT water_piece_2 = {
#include "stg07/scenery/water/water_piece_2.inc"
};

static NJS_MATERIAL water_piece_3_materials = {
#include "stg07/scenery/water/water_piece_3_materials.inc"
};

static Uint16 water_piece_3_strips[9] = {
#include "stg07/scenery/water/water_piece_3_strips.inc"
};

static NJS_MESHSET water_piece_3_faces = {
#include "stg07/scenery/water/water_piece_3_faces.inc"
};

static NJS_POINT3 water_piece_3_points[8] = {
#include "stg07/scenery/water/water_piece_3_points.inc"
};

static NJS_VECTOR water_piece_3_normals[8] = {
#include "stg07/scenery/water/water_piece_3_normals.inc"
};

static NJS_MODEL water_piece_3_model = {
#include "stg07/scenery/water/water_piece_3_model.inc"
};

static NJS_OBJECT water_piece_3 = {
#include "stg07/scenery/water/water_piece_3.inc"
};

static NJS_MATERIAL water_piece_4_materials = {
#include "stg07/scenery/water/water_piece_4_materials.inc"
};

static Uint16 water_piece_4_strips[27] = {
#include "stg07/scenery/water/water_piece_4_strips.inc"
};

static NJS_MESHSET water_piece_4_faces = {
#include "stg07/scenery/water/water_piece_4_faces.inc"
};

static NJS_POINT3 water_piece_4_points[16] = {
#include "stg07/scenery/water/water_piece_4_points.inc"
};

static NJS_VECTOR water_piece_4_normals[16] = {
#include "stg07/scenery/water/water_piece_4_normals.inc"
};

static NJS_MODEL water_piece_4_model = {
#include "stg07/scenery/water/water_piece_4_model.inc"
};

static NJS_OBJECT water_piece_4 = {
#include "stg07/scenery/water/water_piece_4.inc"
};

static NJS_MATERIAL water_piece_5_materials = {
#include "stg07/scenery/water/water_piece_5_materials.inc"
};

static Uint16 water_piece_5_strips[12] = {
#include "stg07/scenery/water/water_piece_5_strips.inc"
};

static NJS_MESHSET water_piece_5_faces = {
#include "stg07/scenery/water/water_piece_5_faces.inc"
};

static NJS_POINT3 water_piece_5_points[8] = {
#include "stg07/scenery/water/water_piece_5_points.inc"
};

static NJS_VECTOR water_piece_5_normals[8] = {
#include "stg07/scenery/water/water_piece_5_normals.inc"
};

static NJS_MODEL water_piece_5_model = {
#include "stg07/scenery/water/water_piece_5_model.inc"
};

static NJS_OBJECT water_piece_5 = {
#include "stg07/scenery/water/water_piece_5.inc"
};

static NJS_MATERIAL water_piece_6_materials = {
#include "stg07/scenery/water/water_piece_6_materials.inc"
};

static Uint16 water_piece_6_strips[5] = {
#include "stg07/scenery/water/water_piece_6_strips.inc"
};

static NJS_MESHSET water_piece_6_faces = {
#include "stg07/scenery/water/water_piece_6_faces.inc"
};

static NJS_POINT3 water_piece_6_points[4] = {
#include "stg07/scenery/water/water_piece_6_points.inc"
};

static NJS_VECTOR water_piece_6_normals[4] = {
#include "stg07/scenery/water/water_piece_6_normals.inc"
};

static NJS_MODEL water_piece_6_model = {
#include "stg07/scenery/water/water_piece_6_model.inc"
};

static NJS_OBJECT water_piece_6 = {
#include "stg07/scenery/water/water_piece_6.inc"
};

static NJS_MATERIAL water_piece_7_materials = {
#include "stg07/scenery/water/water_piece_7_materials.inc"
};

static Uint16 water_piece_7_strips[14] = {
#include "stg07/scenery/water/water_piece_7_strips.inc"
};

static NJS_MESHSET water_piece_7_faces = {
#include "stg07/scenery/water/water_piece_7_faces.inc"
};

static NJS_POINT3 water_piece_7_points[10] = {
#include "stg07/scenery/water/water_piece_7_points.inc"
};

static NJS_VECTOR water_piece_7_normals[10] = {
#include "stg07/scenery/water/water_piece_7_normals.inc"
};

static NJS_MODEL water_piece_7_model = {
#include "stg07/scenery/water/water_piece_7_model.inc"
};

static NJS_OBJECT water_piece_7 = {
#include "stg07/scenery/water/water_piece_7.inc"
};

static NJS_MATERIAL water_piece_8_materials = {
#include "stg07/scenery/water/water_piece_8_materials.inc"
};

static Uint16 water_piece_8_strips[9] = {
#include "stg07/scenery/water/water_piece_8_strips.inc"
};

static NJS_MESHSET water_piece_8_faces = {
#include "stg07/scenery/water/water_piece_8_faces.inc"
};

static NJS_POINT3 water_piece_8_points[8] = {
#include "stg07/scenery/water/water_piece_8_points.inc"
};

static NJS_VECTOR water_piece_8_normals[8] = {
#include "stg07/scenery/water/water_piece_8_normals.inc"
};

static NJS_MODEL water_piece_8_model = {
#include "stg07/scenery/water/water_piece_8_model.inc"
};

static NJS_OBJECT water_piece_8 = {
#include "stg07/scenery/water/water_piece_8.inc"
};

static NJS_MATERIAL water_piece_9_materials = {
#include "stg07/scenery/water/water_piece_9_materials.inc"
};

static Uint16 water_piece_9_strips[27] = {
#include "stg07/scenery/water/water_piece_9_strips.inc"
};

static NJS_MESHSET water_piece_9_faces = {
#include "stg07/scenery/water/water_piece_9_faces.inc"
};

static NJS_POINT3 water_piece_9_points[16] = {
#include "stg07/scenery/water/water_piece_9_points.inc"
};

static NJS_VECTOR water_piece_9_normals[16] = {
#include "stg07/scenery/water/water_piece_9_normals.inc"
};

static NJS_MODEL water_piece_9_model = {
#include "stg07/scenery/water/water_piece_9_model.inc"
};

static NJS_OBJECT water_piece_9 = {
#include "stg07/scenery/water/water_piece_9.inc"
};

static NJS_MATERIAL water_piece_10_materials = {
#include "stg07/scenery/water/water_piece_10_materials.inc"
};

static Uint16 water_piece_10_strips[27] = {
#include "stg07/scenery/water/water_piece_10_strips.inc"
};

static NJS_MESHSET water_piece_10_faces = {
#include "stg07/scenery/water/water_piece_10_faces.inc"
};

static NJS_POINT3 water_piece_10_points[16] = {
#include "stg07/scenery/water/water_piece_10_points.inc"
};

static NJS_VECTOR water_piece_10_normals[16] = {
#include "stg07/scenery/water/water_piece_10_normals.inc"
};

static NJS_MODEL water_piece_10_model = {
#include "stg07/scenery/water/water_piece_10_model.inc"
};

static NJS_OBJECT water_piece_10 = {
#include "stg07/scenery/water/water_piece_10.inc"
};

static NJS_MATERIAL water_piece_11_materials = {
#include "stg07/scenery/water/water_piece_11_materials.inc"
};

static Uint16 water_piece_11_strips[27] = {
#include "stg07/scenery/water/water_piece_11_strips.inc"
};

static NJS_MESHSET water_piece_11_faces = {
#include "stg07/scenery/water/water_piece_11_faces.inc"
};

static NJS_POINT3 water_piece_11_points[16] = {
#include "stg07/scenery/water/water_piece_11_points.inc"
};

static NJS_VECTOR water_piece_11_normals[16] = {
#include "stg07/scenery/water/water_piece_11_normals.inc"
};

static NJS_MODEL water_piece_11_model = {
#include "stg07/scenery/water/water_piece_11_model.inc"
};

static NJS_OBJECT water_piece_11 = {
#include "stg07/scenery/water/water_piece_11.inc"
};

static NJS_MATERIAL water_piece_12_materials = {
#include "stg07/scenery/water/water_piece_12_materials.inc"
};

static Uint16 water_piece_12_strips[7] = {
#include "stg07/scenery/water/water_piece_12_strips.inc"
};

static NJS_MESHSET water_piece_12_faces = {
#include "stg07/scenery/water/water_piece_12_faces.inc"
};

static NJS_POINT3 water_piece_12_points[6] = {
#include "stg07/scenery/water/water_piece_12_points.inc"
};

static NJS_VECTOR water_piece_12_normals[6] = {
#include "stg07/scenery/water/water_piece_12_normals.inc"
};

static NJS_MODEL water_piece_12_model = {
#include "stg07/scenery/water/water_piece_12_model.inc"
};

static NJS_OBJECT water_piece_12 = {
#include "stg07/scenery/water/water_piece_12.inc"
};

static NJS_MATERIAL water_piece_13_materials = {
#include "stg07/scenery/water/water_piece_13_materials.inc"
};

static Uint16 water_piece_13_strips[7] = {
#include "stg07/scenery/water/water_piece_13_strips.inc"
};

static NJS_MESHSET water_piece_13_faces = {
#include "stg07/scenery/water/water_piece_13_faces.inc"
};

static NJS_POINT3 water_piece_13_points[6] = {
#include "stg07/scenery/water/water_piece_13_points.inc"
};

static NJS_VECTOR water_piece_13_normals[6] = {
#include "stg07/scenery/water/water_piece_13_normals.inc"
};

static NJS_MODEL water_piece_13_model = {
#include "stg07/scenery/water/water_piece_13_model.inc"
};

static NJS_OBJECT water_piece_13 = {
#include "stg07/scenery/water/water_piece_13.inc"
};

static NJS_MATERIAL water_piece_14_materials = {
#include "stg07/scenery/water/water_piece_14_materials.inc"
};

static Uint16 water_piece_14_strips[7] = {
#include "stg07/scenery/water/water_piece_14_strips.inc"
};

static NJS_MESHSET water_piece_14_faces = {
#include "stg07/scenery/water/water_piece_14_faces.inc"
};

static NJS_POINT3 water_piece_14_points[6] = {
#include "stg07/scenery/water/water_piece_14_points.inc"
};

static NJS_VECTOR water_piece_14_normals[6] = {
#include "stg07/scenery/water/water_piece_14_normals.inc"
};

static NJS_MODEL water_piece_14_model = {
#include "stg07/scenery/water/water_piece_14_model.inc"
};

static NJS_OBJECT water_piece_14 = {
#include "stg07/scenery/water/water_piece_14.inc"
};

static NJS_MATERIAL water_piece_15_materials = {
#include "stg07/scenery/water/water_piece_15_materials.inc"
};

static Uint16 water_piece_15_strips[5] = {
#include "stg07/scenery/water/water_piece_15_strips.inc"
};

static NJS_MESHSET water_piece_15_faces = {
#include "stg07/scenery/water/water_piece_15_faces.inc"
};

static NJS_POINT3 water_piece_15_points[4] = {
#include "stg07/scenery/water/water_piece_15_points.inc"
};

static NJS_VECTOR water_piece_15_normals[4] = {
#include "stg07/scenery/water/water_piece_15_normals.inc"
};

static NJS_MODEL water_piece_15_model = {
#include "stg07/scenery/water/water_piece_15_model.inc"
};

static NJS_OBJECT water_piece_15 = {
#include "stg07/scenery/water/water_piece_15.inc"
};

#endif
