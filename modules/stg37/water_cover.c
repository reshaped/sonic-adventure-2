/* STG37.PRS's texture lists, chunk models and motions, from the disc. its D
 * lies between core_rouge_water_effects.c and background_textures.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg37/water_cover.h"
#include "stg37/water_materials.h"

static NJS_TEXNAME cover_motion_names[2] = {
#include "stg37/water_cover/cover_motion_names.inc"
};
NJS_TEXLIST cover_motion_textures = {
#include "stg37/water_cover/cover_motion_textures.inc"
};
static Sint16 cover_motion_object_part_8_polygons[173] = {
#include "stg37/water_cover/cover_motion_object_part_8_polygons.inc"
};
static Sint32 cover_motion_object_part_8_vertices[179] = {
#include "stg37/water_cover/cover_motion_object_part_8_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_8_model = {
#include "stg37/water_cover/cover_motion_object_part_8_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_8 = {
#include "stg37/water_cover/cover_motion_object_part_8.inc"
};
static Sint16 cover_motion_object_part_7_polygons[527] = {
#include "stg37/water_cover/cover_motion_object_part_7_polygons.inc"
};
static Sint32 cover_motion_object_part_7_vertices[503] = {
#include "stg37/water_cover/cover_motion_object_part_7_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_7_model = {
#include "stg37/water_cover/cover_motion_object_part_7_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_7 = {
#include "stg37/water_cover/cover_motion_object_part_7.inc"
};
static Sint16 cover_motion_object_part_6_polygons[173] = {
#include "stg37/water_cover/cover_motion_object_part_6_polygons.inc"
};
static Sint32 cover_motion_object_part_6_vertices[179] = {
#include "stg37/water_cover/cover_motion_object_part_6_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_6_model = {
#include "stg37/water_cover/cover_motion_object_part_6_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_6 = {
#include "stg37/water_cover/cover_motion_object_part_6.inc"
};
static Sint16 cover_motion_object_part_5_polygons[527] = {
#include "stg37/water_cover/cover_motion_object_part_5_polygons.inc"
};
static Sint32 cover_motion_object_part_5_vertices[503] = {
#include "stg37/water_cover/cover_motion_object_part_5_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_5_model = {
#include "stg37/water_cover/cover_motion_object_part_5_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_5 = {
#include "stg37/water_cover/cover_motion_object_part_5.inc"
};
static Sint16 cover_motion_object_part_4_polygons[173] = {
#include "stg37/water_cover/cover_motion_object_part_4_polygons.inc"
};
static Sint32 cover_motion_object_part_4_vertices[179] = {
#include "stg37/water_cover/cover_motion_object_part_4_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_4_model = {
#include "stg37/water_cover/cover_motion_object_part_4_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_4 = {
#include "stg37/water_cover/cover_motion_object_part_4.inc"
};
static Sint16 cover_motion_object_part_3_polygons[527] = {
#include "stg37/water_cover/cover_motion_object_part_3_polygons.inc"
};
static Sint32 cover_motion_object_part_3_vertices[503] = {
#include "stg37/water_cover/cover_motion_object_part_3_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_3_model = {
#include "stg37/water_cover/cover_motion_object_part_3_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_3 = {
#include "stg37/water_cover/cover_motion_object_part_3.inc"
};
static Sint16 cover_motion_object_part_2_polygons[173] = {
#include "stg37/water_cover/cover_motion_object_part_2_polygons.inc"
};
static Sint32 cover_motion_object_part_2_vertices[179] = {
#include "stg37/water_cover/cover_motion_object_part_2_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_2_model = {
#include "stg37/water_cover/cover_motion_object_part_2_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_2 = {
#include "stg37/water_cover/cover_motion_object_part_2.inc"
};
static Sint16 cover_motion_object_part_1_polygons[527] = {
#include "stg37/water_cover/cover_motion_object_part_1_polygons.inc"
};
static Sint32 cover_motion_object_part_1_vertices[503] = {
#include "stg37/water_cover/cover_motion_object_part_1_vertices.inc"
};
static NJS_CNK_MODEL cover_motion_object_part_1_model = {
#include "stg37/water_cover/cover_motion_object_part_1_model.inc"
};
static NJS_CNK_OBJECT cover_motion_object_part_1 = {
#include "stg37/water_cover/cover_motion_object_part_1.inc"
};
NJS_CNK_OBJECT cover_motion_object[1] = {
#include "stg37/water_cover/cover_motion_object.inc"
};
static NJS_MKEY_A cover_motion_joint_0_angles[2] = {
#include "stg37/water_cover/cover_motion_joint_0_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_1_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_1_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_2_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_2_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_3_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_3_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_4_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_4_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_5_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_5_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_6_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_6_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_7_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_7_angles.inc"
};
static NJS_MKEY_A cover_motion_joint_8_angles[30] = {
#include "stg37/water_cover/cover_motion_joint_8_angles.inc"
};
static NJS_MDATA3 cover_motion_tracks[9] = {
#include "stg37/water_cover/cover_motion_tracks.inc"
};
NJS_MOTION cover_motion[1] = {
#include "stg37/water_cover/cover_motion.inc"
};
