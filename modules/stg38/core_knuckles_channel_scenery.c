/* STG38.PRS's texture lists, chunk models and motions, from the disc. its D
 * lies between shaft_middle_wall_models.c and background_textures.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/water_shaft/textures.h"
#include "stg38/water_scenery.h"

static NJS_TEXNAME channel_textures_slot[1] = {
#include "stg38/core_knuckles_channel_scenery/channel_textures_slot.inc"
};
NJS_TEXLIST channel_textures = {
#include "stg38/core_knuckles_channel_scenery/channel_textures.inc"
};
static Sint16 lower_channel_panel_2_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_2_polygons.inc"
};
static Sint32 lower_channel_panel_2_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_2_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_panel_2_mesh = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT lower_channel_panel_2 = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_2.inc"
};
static Sint16 lower_channel_panel_1_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_1_polygons.inc"
};
static Sint32 lower_channel_panel_1_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_1_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_panel_1_mesh = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT lower_channel_panel_1 = {
#include "stg38/core_knuckles_channel_scenery/lower_channel_panel_1.inc"
};
NJS_CNK_OBJECT lower_channel = {
#include "stg38/core_knuckles_channel_scenery/lower_channel.inc"
};
static NJS_TEXNAME channel_turn_textures_slot[1] = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_textures_slot.inc"
};
static NJS_TEXLIST channel_turn_textures = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_textures.inc"
};
static Sint16 channel_turn_panel_2_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_2_polygons.inc"
};
static Sint32 channel_turn_panel_2_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_2_vertices.inc"
};
static NJS_CNK_MODEL channel_turn_panel_2_mesh = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT channel_turn_panel_2 = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_2.inc"
};
static Sint16 channel_turn_panel_1_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_1_polygons.inc"
};
static Sint32 channel_turn_panel_1_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_1_vertices.inc"
};
static NJS_CNK_MODEL channel_turn_panel_1_mesh = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT channel_turn_panel_1 = {
#include "stg38/core_knuckles_channel_scenery/channel_turn_panel_1.inc"
};
NJS_CNK_OBJECT channel_turn = {
#include "stg38/core_knuckles_channel_scenery/channel_turn.inc"
};
static NJS_TEXNAME upper_channel_textures_slot[1] = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_textures_slot.inc"
};
static NJS_TEXLIST upper_channel_textures = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_textures.inc"
};
static Sint16 upper_channel_panel_2_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_2_polygons.inc"
};
static Sint32 upper_channel_panel_2_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_2_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_panel_2_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT upper_channel_panel_2 = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_2.inc"
};
static Sint16 upper_channel_panel_1_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_1_polygons.inc"
};
static Sint32 upper_channel_panel_1_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_1_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_panel_1_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT upper_channel_panel_1 = {
#include "stg38/core_knuckles_channel_scenery/upper_channel_panel_1.inc"
};
NJS_CNK_OBJECT upper_channel = {
#include "stg38/core_knuckles_channel_scenery/upper_channel.inc"
};
static NJS_TEXNAME upper_shaft_textures_slot[1] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_textures_slot.inc"
};
NJS_TEXLIST upper_shaft_textures = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_textures.inc"
};
static Sint16 upper_shaft_panel_6_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_6_polygons.inc"
};
static Sint32 upper_shaft_panel_6_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_6_vertices.inc"
};
static NJS_CNK_MODEL upper_shaft_panel_6_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_6_mesh.inc"
};
static NJS_CNK_OBJECT upper_shaft_panel_6 = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_6.inc"
};
static Sint16 upper_shaft_panel_5_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_5_polygons.inc"
};
static Sint32 upper_shaft_panel_5_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_5_vertices.inc"
};
static NJS_CNK_MODEL upper_shaft_panel_5_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_5_mesh.inc"
};
static NJS_CNK_OBJECT upper_shaft_panel_5 = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_5.inc"
};
static Sint16 upper_shaft_panel_4_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_4_polygons.inc"
};
static Sint32 upper_shaft_panel_4_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_4_vertices.inc"
};
static NJS_CNK_MODEL upper_shaft_panel_4_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT upper_shaft_panel_4 = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_4.inc"
};
static Sint16 upper_shaft_panel_3_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_3_polygons.inc"
};
static Sint32 upper_shaft_panel_3_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_3_vertices.inc"
};
static NJS_CNK_MODEL upper_shaft_panel_3_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT upper_shaft_panel_3 = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_3.inc"
};
static Sint16 upper_shaft_panel_2_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_2_polygons.inc"
};
static Sint32 upper_shaft_panel_2_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_2_vertices.inc"
};
static NJS_CNK_MODEL upper_shaft_panel_2_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT upper_shaft_panel_2 = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_2.inc"
};
static Sint16 upper_shaft_panel_1_polygons[143] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_1_polygons.inc"
};
static Sint32 upper_shaft_panel_1_vertices[107] = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_1_vertices.inc"
};
static NJS_CNK_MODEL upper_shaft_panel_1_mesh = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT upper_shaft_panel_1 = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft_panel_1.inc"
};
NJS_CNK_OBJECT upper_shaft = {
#include "stg38/core_knuckles_channel_scenery/upper_shaft.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_0_position[2] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_0_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_0_angles[2] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_0_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_1_position[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_1_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_1_angles[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_1_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_2_position[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_2_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_2_angles[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_2_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_3_position[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_3_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_3_angles[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_3_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_4_position[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_4_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_4_angles[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_4_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_5_position[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_5_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_5_angles[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_5_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_6_position[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_6_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_6_angles[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_6_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_7_position[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_7_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_7_angles[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_7_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_8_position[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_8_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_8_angles[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_8_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_9_position[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_9_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_9_angles[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_9_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_10_position[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_10_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_10_angles[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_10_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_11_position[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_11_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_11_angles[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_11_angles.inc"
};
static NJS_MKEY_F upper_rotor_motion_joint_12_position[137] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_12_position.inc"
};
static NJS_MKEY_A upper_rotor_motion_joint_12_angles[136] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_joint_12_angles.inc"
};
static NJS_MDATA3 upper_rotor_motion_tracks[13] = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion_tracks.inc"
};
NJS_MOTION upper_rotor_motion = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_motion.inc"
};
static NJS_TEXNAME panel_texture[1] = {
#include "stg38/core_knuckles_channel_scenery/panel_texture.inc"
};
NJS_TEXLIST upper_rotor_textures = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor_textures.inc"
};
static Sint16 rotor_panel_1_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_1_polygons.inc"
};
static Sint32 rotor_panel_1_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_1_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_1_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_1_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_1_node.inc"
};
static Sint16 rotor_panel_2_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_2_polygons.inc"
};
static Sint32 rotor_panel_2_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_2_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_2_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_2_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_2_node.inc"
};
static Sint16 rotor_panel_3_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_3_polygons.inc"
};
static Sint32 rotor_panel_3_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_3_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_3_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_3_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_3_node.inc"
};
static Sint16 rotor_panel_4_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_4_polygons.inc"
};
static Sint32 rotor_panel_4_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_4_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_4_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_4_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_4_node.inc"
};
static Sint16 rotor_panel_5_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_5_polygons.inc"
};
static Sint32 rotor_panel_5_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_5_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_5_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_5_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_5_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_5_node.inc"
};
static Sint16 rotor_panel_6_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_6_polygons.inc"
};
static Sint32 rotor_panel_6_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_6_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_6_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_6_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_6_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_6_node.inc"
};
static Sint16 rotor_panel_7_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_7_polygons.inc"
};
static Sint32 rotor_panel_7_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_7_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_7_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_7_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_7_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_7_node.inc"
};
static Sint16 rotor_panel_8_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_8_polygons.inc"
};
static Sint32 rotor_panel_8_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_8_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_8_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_8_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_8_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_8_node.inc"
};
static Sint16 rotor_panel_9_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_9_polygons.inc"
};
static Sint32 rotor_panel_9_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_9_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_9_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_9_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_9_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_9_node.inc"
};
static Sint16 rotor_panel_10_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_10_polygons.inc"
};
static Sint32 rotor_panel_10_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_10_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_10_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_10_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_10_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_10_node.inc"
};
static Sint16 rotor_panel_11_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_11_polygons.inc"
};
static Sint32 rotor_panel_11_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_11_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_11_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_11_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_11_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_11_node.inc"
};
static Sint16 rotor_panel_12_polygons[263] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_12_polygons.inc"
};
static Sint32 rotor_panel_12_vertices[243] = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_12_vertices.inc"
};
static NJS_CNK_MODEL rotor_panel_12_mesh = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_12_mesh.inc"
};
static NJS_CNK_OBJECT rotor_panel_12_node = {
#include "stg38/core_knuckles_channel_scenery/rotor_panel_12_node.inc"
};
NJS_CNK_OBJECT upper_rotor = {
#include "stg38/core_knuckles_channel_scenery/upper_rotor.inc"
};
