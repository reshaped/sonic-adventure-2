/* CHAO.PRS's chunk models and motions, from the disc. its D lies between
 * tree_data.c and machine_collision.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/machine_models.h"

static Sint16 machine_neutral_base_polygons[153] = {
#include "chao/machine_models/machine_neutral_base_polygons.inc"
};
static Sint32 machine_neutral_base_vertices[123] = {
#include "chao/machine_models/machine_neutral_base_vertices.inc"
};
static NJS_CNK_MODEL machine_neutral_base_model = {
#include "chao/machine_models/machine_neutral_base_model.inc"
};
NJS_CNK_OBJECT machine_neutral_base = {
#include "chao/machine_models/machine_neutral_base.inc"
};
static Sint16 machine_neutral_button_polygons[127] = {
#include "chao/machine_models/machine_neutral_button_polygons.inc"
};
static Sint32 machine_neutral_button_vertices[105] = {
#include "chao/machine_models/machine_neutral_button_vertices.inc"
};
static NJS_CNK_MODEL machine_neutral_button_model = {
#include "chao/machine_models/machine_neutral_button_model.inc"
};
NJS_CNK_OBJECT machine_neutral_button = {
#include "chao/machine_models/machine_neutral_button.inc"
};
static Sint16 machine_neutral_door_polygons[21] = {
#include "chao/machine_models/machine_neutral_door_polygons.inc"
};
static Sint32 machine_neutral_door_vertices[51] = {
#include "chao/machine_models/machine_neutral_door_vertices.inc"
};
static NJS_CNK_MODEL machine_neutral_door_model = {
#include "chao/machine_models/machine_neutral_door_model.inc"
};
NJS_CNK_OBJECT machine_neutral_door = {
#include "chao/machine_models/machine_neutral_door.inc"
};
static Sint16 machine_neutral_body_polygons[393] = {
#include "chao/machine_models/machine_neutral_body_polygons.inc"
};
static Sint32 machine_neutral_body_vertices[687] = {
#include "chao/machine_models/machine_neutral_body_vertices.inc"
};
static NJS_CNK_MODEL machine_neutral_body_model = {
#include "chao/machine_models/machine_neutral_body_model.inc"
};
NJS_CNK_OBJECT machine_neutral_body = {
#include "chao/machine_models/machine_neutral_body.inc"
};
static Sint16 machine_hero_base_polygons[153] = {
#include "chao/machine_models/machine_hero_base_polygons.inc"
};
static Sint32 machine_hero_base_vertices[83] = {
#include "chao/machine_models/machine_hero_base_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_base_model = {
#include "chao/machine_models/machine_hero_base_model.inc"
};
NJS_CNK_OBJECT machine_hero_base = {
#include "chao/machine_models/machine_hero_base.inc"
};
static Sint16 machine_hero_button_polygons[127] = {
#include "chao/machine_models/machine_hero_button_polygons.inc"
};
static Sint32 machine_hero_button_vertices[71] = {
#include "chao/machine_models/machine_hero_button_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_button_model = {
#include "chao/machine_models/machine_hero_button_model.inc"
};
NJS_CNK_OBJECT machine_hero_button = {
#include "chao/machine_models/machine_hero_button.inc"
};
static Sint16 machine_hero_trim_node_2_polygons[173] = {
#include "chao/machine_models/machine_hero_trim_node_2_polygons.inc"
};
static Sint32 machine_hero_trim_node_2_vertices[75] = {
#include "chao/machine_models/machine_hero_trim_node_2_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_trim_node_2_model = {
#include "chao/machine_models/machine_hero_trim_node_2_model.inc"
};
static NJS_CNK_OBJECT machine_hero_trim_node_2 = {
#include "chao/machine_models/machine_hero_trim_node_2.inc"
};
static Sint16 machine_hero_trim_node_1_polygons[181] = {
#include "chao/machine_models/machine_hero_trim_node_1_polygons.inc"
};
static Sint32 machine_hero_trim_node_1_vertices[75] = {
#include "chao/machine_models/machine_hero_trim_node_1_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_trim_node_1_model = {
#include "chao/machine_models/machine_hero_trim_node_1_model.inc"
};
static NJS_CNK_OBJECT machine_hero_trim_node_1 = {
#include "chao/machine_models/machine_hero_trim_node_1.inc"
};
static Sint16 machine_hero_trim_polygons[161] = {
#include "chao/machine_models/machine_hero_trim_polygons.inc"
};
static Sint32 machine_hero_trim_vertices[163] = {
#include "chao/machine_models/machine_hero_trim_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_trim_model = {
#include "chao/machine_models/machine_hero_trim_model.inc"
};
NJS_CNK_OBJECT machine_hero_trim_object = {
#include "chao/machine_models/machine_hero_trim_object.inc"
};
static Sint16 machine_hero_body_node_2_polygons[101] = {
#include "chao/machine_models/machine_hero_body_node_2_polygons.inc"
};
static Sint32 machine_hero_body_node_2_vertices[83] = {
#include "chao/machine_models/machine_hero_body_node_2_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_body_node_2_model = {
#include "chao/machine_models/machine_hero_body_node_2_model.inc"
};
static NJS_CNK_OBJECT machine_hero_body_node_2 = {
#include "chao/machine_models/machine_hero_body_node_2.inc"
};
static Sint16 machine_hero_body_node_1_polygons[101] = {
#include "chao/machine_models/machine_hero_body_node_1_polygons.inc"
};
static Sint32 machine_hero_body_node_1_vertices[83] = {
#include "chao/machine_models/machine_hero_body_node_1_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_body_node_1_model = {
#include "chao/machine_models/machine_hero_body_node_1_model.inc"
};
static NJS_CNK_OBJECT machine_hero_body_node_1 = {
#include "chao/machine_models/machine_hero_body_node_1.inc"
};
static Sint16 machine_hero_body_polygons[483] = {
#include "chao/machine_models/machine_hero_body_polygons.inc"
};
static Sint32 machine_hero_body_vertices[403] = {
#include "chao/machine_models/machine_hero_body_vertices.inc"
};
static NJS_CNK_MODEL machine_hero_body_model = {
#include "chao/machine_models/machine_hero_body_model.inc"
};
NJS_CNK_OBJECT machine_hero_body = {
#include "chao/machine_models/machine_hero_body.inc"
};
static NJS_MKEY_A machine_hero_door_motion_joint_1_angles[8] = {
#include "chao/machine_models/machine_hero_door_motion_joint_1_angles.inc"
};
static NJS_MKEY_A machine_hero_door_motion_joint_2_angles[8] = {
#include "chao/machine_models/machine_hero_door_motion_joint_2_angles.inc"
};
static NJS_MDATA2 machine_hero_door_motion_tracks[3] = {
#include "chao/machine_models/machine_hero_door_motion_tracks.inc"
};
NJS_MOTION machine_hero_door_motion = {
#include "chao/machine_models/machine_hero_door_motion.inc"
};
static NJS_MKEY_A machine_hero_trim_motion_joint_1_angles[8] = {
#include "chao/machine_models/machine_hero_trim_motion_joint_1_angles.inc"
};
static NJS_MKEY_A machine_hero_trim_motion_joint_2_angles[8] = {
#include "chao/machine_models/machine_hero_trim_motion_joint_2_angles.inc"
};
static NJS_MDATA2 machine_hero_trim_motion_tracks[3] = {
#include "chao/machine_models/machine_hero_trim_motion_tracks.inc"
};
NJS_MOTION machine_hero_trim_motion = {
#include "chao/machine_models/machine_hero_trim_motion.inc"
};
static Sint16 machine_dark_base_polygons[153] = {
#include "chao/machine_models/machine_dark_base_polygons.inc"
};
static Sint32 machine_dark_base_vertices[83] = {
#include "chao/machine_models/machine_dark_base_vertices.inc"
};
static NJS_CNK_MODEL machine_dark_base_model = {
#include "chao/machine_models/machine_dark_base_model.inc"
};
NJS_CNK_OBJECT machine_dark_base = {
#include "chao/machine_models/machine_dark_base.inc"
};
static Sint16 machine_dark_button_polygons[127] = {
#include "chao/machine_models/machine_dark_button_polygons.inc"
};
static Sint32 machine_dark_button_vertices[71] = {
#include "chao/machine_models/machine_dark_button_vertices.inc"
};
static NJS_CNK_MODEL machine_dark_button_model = {
#include "chao/machine_models/machine_dark_button_model.inc"
};
NJS_CNK_OBJECT machine_dark_button = {
#include "chao/machine_models/machine_dark_button.inc"
};
static Sint16 machine_dark_body_node_4_polygons[79] = {
#include "chao/machine_models/machine_dark_body_node_4_polygons.inc"
};
static Sint32 machine_dark_body_node_4_vertices[47] = {
#include "chao/machine_models/machine_dark_body_node_4_vertices.inc"
};
static NJS_CNK_MODEL machine_dark_body_node_4_model = {
#include "chao/machine_models/machine_dark_body_node_4_model.inc"
};
static NJS_CNK_OBJECT machine_dark_body_node_4 = {
#include "chao/machine_models/machine_dark_body_node_4.inc"
};
static Sint16 machine_dark_body_node_3_polygons[79] = {
#include "chao/machine_models/machine_dark_body_node_3_polygons.inc"
};
static Sint32 machine_dark_body_node_3_vertices[47] = {
#include "chao/machine_models/machine_dark_body_node_3_vertices.inc"
};
static NJS_CNK_MODEL machine_dark_body_node_3_model = {
#include "chao/machine_models/machine_dark_body_node_3_model.inc"
};
static NJS_CNK_OBJECT machine_dark_body_node_3 = {
#include "chao/machine_models/machine_dark_body_node_3.inc"
};
static Sint16 machine_dark_body_node_2_polygons[101] = {
#include "chao/machine_models/machine_dark_body_node_2_polygons.inc"
};
static Sint32 machine_dark_body_node_2_vertices[99] = {
#include "chao/machine_models/machine_dark_body_node_2_vertices.inc"
};
static NJS_CNK_MODEL machine_dark_body_node_2_model = {
#include "chao/machine_models/machine_dark_body_node_2_model.inc"
};
static NJS_CNK_OBJECT machine_dark_body_node_2 = {
#include "chao/machine_models/machine_dark_body_node_2.inc"
};
static Sint16 machine_dark_body_node_1_polygons[101] = {
#include "chao/machine_models/machine_dark_body_node_1_polygons.inc"
};
static Sint32 machine_dark_body_node_1_vertices[99] = {
#include "chao/machine_models/machine_dark_body_node_1_vertices.inc"
};
static NJS_CNK_MODEL machine_dark_body_node_1_model = {
#include "chao/machine_models/machine_dark_body_node_1_model.inc"
};
static NJS_CNK_OBJECT machine_dark_body_node_1 = {
#include "chao/machine_models/machine_dark_body_node_1.inc"
};
static Sint16 machine_dark_body_polygons[1285] = {
#include "chao/machine_models/machine_dark_body_polygons.inc"
};
static Sint32 machine_dark_body_vertices[791] = {
#include "chao/machine_models/machine_dark_body_vertices.inc"
};
static NJS_CNK_MODEL machine_dark_body_model = {
#include "chao/machine_models/machine_dark_body_model.inc"
};
NJS_CNK_OBJECT machine_dark_body = {
#include "chao/machine_models/machine_dark_body.inc"
};
static NJS_MKEY_A machine_dark_door_motion_joint_1_angles[8] = {
#include "chao/machine_models/machine_dark_door_motion_joint_1_angles.inc"
};
static NJS_MKEY_A machine_dark_door_motion_joint_2_angles[8] = {
#include "chao/machine_models/machine_dark_door_motion_joint_2_angles.inc"
};
static NJS_MKEY_A machine_dark_door_motion_joint_3_angles[8] = {
#include "chao/machine_models/machine_dark_door_motion_joint_3_angles.inc"
};
static NJS_MKEY_A machine_dark_door_motion_joint_4_angles[8] = {
#include "chao/machine_models/machine_dark_door_motion_joint_4_angles.inc"
};
static NJS_MDATA2 machine_dark_door_motion_tracks[5] = {
#include "chao/machine_models/machine_dark_door_motion_tracks.inc"
};
NJS_MOTION machine_dark_door_motion = {
#include "chao/machine_models/machine_dark_door_motion.inc"
};
