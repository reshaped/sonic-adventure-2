/* STG23.PRS's texture lists, chunk models, UV scrolls, motions and their
 * registry, from the disc. its D lies between explosion.c and
 * global_light_switch.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "player/player.h"
#include "shared/motions.h"
#include "stg23/sand_surfaces.h"

static NJS_TEXNAME base_sand_upper_material[1] = {
#include "stg23/hidden_base_sand_upper/base_sand_upper_material.inc"
};
static NJS_TEXLIST base_sand_upper_material_list = {
#include "stg23/hidden_base_sand_upper/base_sand_upper_material_list.inc"
};
static Sint16 base_sand_upper_polygons[345] = {
#include "stg23/hidden_base_sand_upper/base_sand_upper_polygons.inc"
};
static Sint32 base_sand_upper_vertices[267] = {
#include "stg23/hidden_base_sand_upper/base_sand_upper_vertices.inc"
};
static NJS_CNK_MODEL base_sand_upper_model = {
#include "stg23/hidden_base_sand_upper/base_sand_upper_model.inc"
};
NJS_CNK_OBJECT base_sand_upper = {
#include "stg23/hidden_base_sand_upper/base_sand_upper.inc"
};
static Sint16 hidden_base_sand_upper_uvs[248] = {
#include "stg23/hidden_base_sand_upper/hidden_base_sand_upper_uvs.inc"
};
struct texture_anim base_sand_upper_anim = {
#include "stg23/hidden_base_sand_upper/base_sand_upper_anim.inc"
};
static NJS_TEXNAME base_sand_end_material[1] = {
#include "stg23/hidden_base_sand_upper/base_sand_end_material.inc"
};
static NJS_TEXLIST base_sand_end_material_list = {
#include "stg23/hidden_base_sand_upper/base_sand_end_material_list.inc"
};
static Sint16 base_sand_end_polygons[229] = {
#include "stg23/hidden_base_sand_upper/base_sand_end_polygons.inc"
};
static Sint32 base_sand_end_vertices[211] = {
#include "stg23/hidden_base_sand_upper/base_sand_end_vertices.inc"
};
static NJS_CNK_MODEL base_sand_end_model = {
#include "stg23/hidden_base_sand_upper/base_sand_end_model.inc"
};
NJS_CNK_OBJECT base_sand_end = {
#include "stg23/hidden_base_sand_upper/base_sand_end.inc"
};
static Sint16 hidden_base_sand_end_uvs[162] = {
#include "stg23/hidden_base_sand_upper/hidden_base_sand_end_uvs.inc"
};
struct texture_anim base_sand_end_anim = {
#include "stg23/hidden_base_sand_upper/base_sand_end_anim.inc"
};
static NJS_TEXNAME base_sand_fall_material[1] = {
#include "stg23/hidden_base_sand_upper/base_sand_fall_material.inc"
};
static NJS_TEXLIST base_sand_fall_material_list = {
#include "stg23/hidden_base_sand_upper/base_sand_fall_material_list.inc"
};
static Sint16 base_sand_fall_polygons[69] = {
#include "stg23/hidden_base_sand_upper/base_sand_fall_polygons.inc"
};
static Sint32 base_sand_fall_vertices[55] = {
#include "stg23/hidden_base_sand_upper/base_sand_fall_vertices.inc"
};
static NJS_CNK_MODEL base_sand_fall_model = {
#include "stg23/hidden_base_sand_upper/base_sand_fall_model.inc"
};
NJS_CNK_OBJECT base_sand_fall = {
#include "stg23/hidden_base_sand_upper/base_sand_fall.inc"
};
static Sint16 hidden_base_sand_fall_uvs[44] = {
#include "stg23/hidden_base_sand_upper/hidden_base_sand_fall_uvs.inc"
};
struct texture_anim base_sand_fall_anim = {
#include "stg23/hidden_base_sand_upper/base_sand_fall_anim.inc"
};
static NJS_TEXNAME base_sand_start_material[1] = {
#include "stg23/hidden_base_sand_upper/base_sand_start_material.inc"
};
static NJS_TEXLIST base_sand_start_material_list = {
#include "stg23/hidden_base_sand_upper/base_sand_start_material_list.inc"
};
static Sint16 base_sand_start_polygons[1283] = {
#include "stg23/hidden_base_sand_upper/base_sand_start_polygons.inc"
};
static Sint32 base_sand_start_vertices[823] = {
#include "stg23/hidden_base_sand_upper/base_sand_start_vertices.inc"
};
static NJS_CNK_MODEL base_sand_start_model = {
#include "stg23/hidden_base_sand_upper/base_sand_start_model.inc"
};
NJS_CNK_OBJECT base_sand_start = {
#include "stg23/hidden_base_sand_upper/base_sand_start.inc"
};
static Sint16 hidden_base_sand_start_uvs[944] = {
#include "stg23/hidden_base_sand_upper/hidden_base_sand_start_uvs.inc"
};
struct texture_anim base_sand_start_anim = {
#include "stg23/hidden_base_sand_upper/base_sand_start_anim.inc"
};
static NJS_TEXNAME base_sand_low_material[1] = {
#include "stg23/hidden_base_sand_upper/base_sand_low_material.inc"
};
static NJS_TEXLIST base_sand_low_material_list = {
#include "stg23/hidden_base_sand_upper/base_sand_low_material_list.inc"
};
static Sint16 base_sand_low_polygons[741] = {
#include "stg23/hidden_base_sand_upper/base_sand_low_polygons.inc"
};
static Sint32 base_sand_low_vertices[555] = {
#include "stg23/hidden_base_sand_upper/base_sand_low_vertices.inc"
};
static NJS_CNK_MODEL base_sand_low_model = {
#include "stg23/hidden_base_sand_upper/base_sand_low_model.inc"
};
NJS_CNK_OBJECT base_sand_low = {
#include "stg23/hidden_base_sand_upper/base_sand_low.inc"
};
static Sint16 hidden_base_sand_low_uvs[544] = {
#include "stg23/hidden_base_sand_upper/hidden_base_sand_low_uvs.inc"
};
struct texture_anim base_sand_low_anim = {
#include "stg23/hidden_base_sand_upper/base_sand_low_anim.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_1_position[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_1_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_1_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_1_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_2_position[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_2_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_2_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_2_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_3_position[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_3_position.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_5_position[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_5_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_5_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_5_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_9_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_10_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_10_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_11_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_11_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_12_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_12_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_15_angles[12] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_15_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_16_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_16_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_17_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_18_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_20_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_20_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_22_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_22_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_23_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_27_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_28_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_28_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_29_position[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_29_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_29_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_30_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_30_angles.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_31_angles[16] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_31_angles.inc"
};
static NJS_MKEY_F upgrade_mission_street_motion_joint_32_position[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_32_position.inc"
};
static NJS_MKEY_A upgrade_mission_street_motion_joint_32_angles[2] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_joint_32_angles.inc"
};
static NJS_MDATA2 upgrade_mission_street_motion_tracks[33] = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion_tracks.inc"
};
static NJS_MOTION upgrade_mission_street_motion = {
#include "stg23/hidden_base_sand_upper/upgrade_mission_street_motion.inc"
};
static NJS_MKEY_F tails_walker_sink_motion_joint_1_position[2] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_1_position.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_1_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_1_angles.inc"
};
static NJS_MKEY_F tails_walker_sink_motion_joint_3_position[2] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_3_position.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_9_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_9_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_10_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_10_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_11_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_11_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_15_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_15_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_16_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_16_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_17_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_17_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_20_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_20_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_27_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_27_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_28_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_28_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_29_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_29_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_30_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_30_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_31_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_31_angles.inc"
};
static NJS_MKEY_A tails_walker_sink_motion_joint_32_angles[63] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_joint_32_angles.inc"
};
static NJS_MDATA2 tails_walker_sink_motion_tracks[33] = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion_tracks.inc"
};
static NJS_MOTION tails_walker_sink_motion = {
#include "stg23/hidden_base_sand_upper/tails_walker_sink_motion.inc"
};
struct motion_entry module_motions[3] = {
#include "stg23/hidden_base_sand_upper/module_motions.inc"
};
