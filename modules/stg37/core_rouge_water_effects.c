/* STG37.PRS's texture lists, chunk models and chunk poses, from the disc.
 * its D lies between shaft_middle_wall_models.c and water_cover.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg37/core_rouge_water_filenames.h"
#include "stg37/water_materials.h"
#include "stg37/water_poses.h"
#include "stg37/water_scenery.h"

struct bubble_poses_pose {
    Sint16 polygons[25];
    Sint32 vertices[19];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct second_splash_poses_pose {
    Sint16 polygons[31];
    Sint32 vertices[27];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct first_splash_poses_pose {
    Sint16 polygons[63];
    Sint32 vertices[67];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

static NJS_TEXNAME lower_wall_marker_textures_slots[1] = {
#include "stg37/core_rouge_water_effects/lower_wall_marker_textures_slots.inc"
};
static NJS_TEXLIST lower_wall_marker_textures = {
#include "stg37/core_rouge_water_effects/lower_wall_marker_textures.inc"
};
static Sint16 lower_wall_panel_2_polygons[165] = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_2_polygons.inc"
};
static Sint32 lower_wall_panel_2_vertices[139] = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL lower_wall_panel_2_mesh = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT lower_wall_panel_2 = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_2.inc"
};
static Sint16 lower_wall_panel_1_polygons[165] = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_1_polygons.inc"
};
static Sint32 lower_wall_panel_1_vertices[139] = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL lower_wall_panel_1_mesh = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT lower_wall_panel_1 = {
#include "stg37/core_rouge_water_effects/lower_wall_panel_1.inc"
};
NJS_CNK_OBJECT lower_walls = {
#include "stg37/core_rouge_water_effects/lower_walls.inc"
};
static NJS_TEXNAME lower_wall_textures_slots[1] = {
#include "stg37/core_rouge_water_effects/lower_wall_textures_slots.inc"
};
NJS_TEXLIST lower_wall_textures = {
#include "stg37/core_rouge_water_effects/lower_wall_textures.inc"
};
static Sint16 lower_wall_model_object_polygons[143] = {
#include "stg37/core_rouge_water_effects/lower_wall_model_object_polygons.inc"
};
static Sint32 lower_wall_model_object_vertices[107] = {
#include "stg37/core_rouge_water_effects/lower_wall_model_object_vertices.inc"
};
NJS_CNK_MODEL lower_wall_model = {
#include "stg37/core_rouge_water_effects/lower_wall_model.inc"
};
static NJS_CNK_OBJECT lower_wall_model_object = {
#include "stg37/core_rouge_water_effects/lower_wall_model_object.inc"
};
static NJS_TEXNAME bubble_textures_slots[1] = {
#include "stg37/core_rouge_water_effects/bubble_textures_slots.inc"
};
NJS_TEXLIST bubble_textures = {
#include "stg37/core_rouge_water_effects/bubble_textures.inc"
};
struct bubble_pose bubble_poses[60] = {
#include "stg37/core_rouge_water_effects/bubble_poses.inc"
};
static NJS_CNK_OBJECT bubbles_sequence = {
#include "stg37/core_rouge_water_effects/bubbles_sequence.inc"
};
static NJS_TEXNAME splash_textures_slots[2] = {
#include "stg37/core_rouge_water_effects/splash_textures_slots.inc"
};
NJS_TEXLIST splash_textures = {
#include "stg37/core_rouge_water_effects/splash_textures.inc"
};
struct second_splash_pose second_splash_poses[15] = {
#include "stg37/core_rouge_water_effects/second_splash_poses.inc"
};
struct first_splash_pose first_splash_poses[36] = {
#include "stg37/core_rouge_water_effects/first_splash_poses.inc"
};
static NJS_CNK_OBJECT splashes_sequence = {
#include "stg37/core_rouge_water_effects/splashes_sequence.inc"
};
static NJS_TEXNAME splash_water_textures_slots[1] = {
#include "stg37/core_rouge_water_effects/splash_water_textures_slots.inc"
};
NJS_TEXLIST splash_water_textures = {
#include "stg37/core_rouge_water_effects/splash_water_textures.inc"
};
static Sint16 water_splash_pose_15_polygons[339] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_15_polygons.inc"
};
static Sint32 water_splash_pose_15_vertices[263] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_15_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_15 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_15.inc"
};
static NJS_CNK_OBJECT water_splash_pose_15_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_15_node.inc"
};
static Sint16 water_splash_pose_14_polygons[339] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_14_polygons.inc"
};
static Sint32 water_splash_pose_14_vertices[263] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_14_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_14 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_14.inc"
};
static NJS_CNK_OBJECT water_splash_pose_14_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_14_node.inc"
};
static Sint16 water_splash_pose_13_polygons[321] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_13_polygons.inc"
};
static Sint32 water_splash_pose_13_vertices[247] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_13_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_13 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_13.inc"
};
static NJS_CNK_OBJECT water_splash_pose_13_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_13_node.inc"
};
static Sint16 water_splash_pose_12_polygons[323] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_12_polygons.inc"
};
static Sint32 water_splash_pose_12_vertices[231] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_12_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_12 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_12.inc"
};
static NJS_CNK_OBJECT water_splash_pose_12_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_12_node.inc"
};
static Sint16 water_splash_pose_11_polygons[285] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_11_polygons.inc"
};
static Sint32 water_splash_pose_11_vertices[215] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_11_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_11 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_11.inc"
};
static NJS_CNK_OBJECT water_splash_pose_11_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_11_node.inc"
};
static Sint16 water_splash_pose_10_polygons[259] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_10_polygons.inc"
};
static Sint32 water_splash_pose_10_vertices[199] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_10_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_10 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_10.inc"
};
static NJS_CNK_OBJECT water_splash_pose_10_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_10_node.inc"
};
static Sint16 water_splash_pose_9_polygons[241] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_9_polygons.inc"
};
static Sint32 water_splash_pose_9_vertices[183] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_9_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_9 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_9.inc"
};
static NJS_CNK_OBJECT water_splash_pose_9_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_9_node.inc"
};
static Sint16 water_splash_pose_8_polygons[217] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_8_polygons.inc"
};
static Sint32 water_splash_pose_8_vertices[167] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_8_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_8 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_8.inc"
};
static NJS_CNK_OBJECT water_splash_pose_8_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_8_node.inc"
};
static Sint16 water_splash_pose_7_polygons[185] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_7_polygons.inc"
};
static Sint32 water_splash_pose_7_vertices[151] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_7_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_7 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_7.inc"
};
static NJS_CNK_OBJECT water_splash_pose_7_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_7_node.inc"
};
static Sint16 water_splash_pose_6_polygons[167] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_6_polygons.inc"
};
static Sint32 water_splash_pose_6_vertices[135] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_6_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_6 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_6.inc"
};
static NJS_CNK_OBJECT water_splash_pose_6_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_6_node.inc"
};
static Sint16 water_splash_pose_5_polygons[149] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_5_polygons.inc"
};
static Sint32 water_splash_pose_5_vertices[119] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_5_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_5 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_5.inc"
};
static NJS_CNK_OBJECT water_splash_pose_5_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_5_node.inc"
};
static Sint16 water_splash_pose_4_polygons[131] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_4_polygons.inc"
};
static Sint32 water_splash_pose_4_vertices[103] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_4_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_4 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_4.inc"
};
static NJS_CNK_OBJECT water_splash_pose_4_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_4_node.inc"
};
static Sint16 water_splash_pose_3_polygons[113] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_3_polygons.inc"
};
static Sint32 water_splash_pose_3_vertices[87] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_3_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_3 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_3.inc"
};
static NJS_CNK_OBJECT water_splash_pose_3_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_3_node.inc"
};
static Sint16 water_splash_pose_2_polygons[87] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_2_polygons.inc"
};
static Sint32 water_splash_pose_2_vertices[67] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_2_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_2 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_2.inc"
};
static NJS_CNK_OBJECT water_splash_pose_2_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_2_node.inc"
};
static Sint16 water_splash_pose_1_polygons[63] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_1_polygons.inc"
};
static Sint32 water_splash_pose_1_vertices[51] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_1_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_1 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_1.inc"
};
static NJS_CNK_OBJECT water_splash_pose_1_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_1_node.inc"
};
static Sint16 water_splash_pose_0_polygons[37] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_0_polygons.inc"
};
static Sint32 water_splash_pose_0_vertices[35] = {
#include "stg37/core_rouge_water_effects/water_splash_pose_0_vertices.inc"
};
NJS_CNK_MODEL water_splash_pose_0 = {
#include "stg37/core_rouge_water_effects/water_splash_pose_0.inc"
};
static NJS_CNK_OBJECT water_splash_pose_0_node = {
#include "stg37/core_rouge_water_effects/water_splash_pose_0_node.inc"
};
static NJS_CNK_OBJECT water_splash_sequence = {
#include "stg37/core_rouge_water_effects/water_splash_sequence.inc"
};
static NJS_TEXNAME rising_water_textures_slots[1] = {
#include "stg37/core_rouge_water_effects/rising_water_textures_slots.inc"
};
static NJS_TEXLIST rising_water_textures = {
#include "stg37/core_rouge_water_effects/rising_water_textures.inc"
};
static Sint16 rising_water_polygons[125] = {
#include "stg37/core_rouge_water_effects/rising_water_polygons.inc"
};
static Sint32 rising_water_vertices[87] = {
#include "stg37/core_rouge_water_effects/rising_water_vertices.inc"
};
static NJS_CNK_MODEL rising_water_mesh = {
#include "stg37/core_rouge_water_effects/rising_water_mesh.inc"
};
NJS_CNK_OBJECT rising_water = {
#include "stg37/core_rouge_water_effects/rising_water.inc"
};
