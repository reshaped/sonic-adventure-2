/* CHAO.PRS's chunk models and motions, from the disc. its D lies between
 * garden_camera.c and jack_box.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/toy_art.h"

static Sint16 garden_neutral_ball_model_polygons[127] = {
#include "chao/ball_models/garden_neutral_ball_model_polygons.inc"
};
static Sint32 garden_neutral_ball_model_vertices[255] = {
#include "chao/ball_models/garden_neutral_ball_model_vertices.inc"
};
static NJS_CNK_MODEL garden_neutral_ball_model_model = {
#include "chao/ball_models/garden_neutral_ball_model_model.inc"
};
NJS_CNK_OBJECT garden_neutral_ball_model = {
#include "chao/ball_models/garden_neutral_ball_model.inc"
};
static Sint16 garden_hero_ball_model_polygons[373] = {
#include "chao/ball_models/garden_hero_ball_model_polygons.inc"
};
static Sint32 garden_hero_ball_model_vertices[255] = {
#include "chao/ball_models/garden_hero_ball_model_vertices.inc"
};
static NJS_CNK_MODEL garden_hero_ball_model_model = {
#include "chao/ball_models/garden_hero_ball_model_model.inc"
};
NJS_CNK_OBJECT garden_hero_ball_model = {
#include "chao/ball_models/garden_hero_ball_model.inc"
};
static Sint16 garden_dark_ball_model_polygons[311] = {
#include "chao/ball_models/garden_dark_ball_model_polygons.inc"
};
static Sint32 garden_dark_ball_model_vertices[255] = {
#include "chao/ball_models/garden_dark_ball_model_vertices.inc"
};
static NJS_CNK_MODEL garden_dark_ball_model_model = {
#include "chao/ball_models/garden_dark_ball_model_model.inc"
};
NJS_CNK_OBJECT garden_dark_ball_model = {
#include "chao/ball_models/garden_dark_ball_model.inc"
};
static Sint16 garden_jack_box_node_5_polygons[25] = {
#include "chao/ball_models/garden_jack_box_node_5_polygons.inc"
};
static Sint32 garden_jack_box_node_5_vertices[27] = {
#include "chao/ball_models/garden_jack_box_node_5_vertices.inc"
};
static NJS_CNK_MODEL garden_jack_box_node_5_model = {
#include "chao/ball_models/garden_jack_box_node_5_model.inc"
};
static NJS_CNK_OBJECT garden_jack_box_node_5 = {
#include "chao/ball_models/garden_jack_box_node_5.inc"
};
static Sint16 garden_jack_box_node_4_polygons[37] = {
#include "chao/ball_models/garden_jack_box_node_4_polygons.inc"
};
static Sint32 garden_jack_box_node_4_vertices[39] = {
#include "chao/ball_models/garden_jack_box_node_4_vertices.inc"
};
static NJS_CNK_MODEL garden_jack_box_node_4_model = {
#include "chao/ball_models/garden_jack_box_node_4_model.inc"
};
static NJS_CNK_OBJECT garden_jack_box_node_4 = {
#include "chao/ball_models/garden_jack_box_node_4.inc"
};
static Sint16 garden_jack_box_node_3_polygons[101] = {
#include "chao/ball_models/garden_jack_box_node_3_polygons.inc"
};
static Sint32 garden_jack_box_node_3_vertices[75] = {
#include "chao/ball_models/garden_jack_box_node_3_vertices.inc"
};
static NJS_CNK_MODEL garden_jack_box_node_3_model = {
#include "chao/ball_models/garden_jack_box_node_3_model.inc"
};
static NJS_CNK_OBJECT garden_jack_box_node_3 = {
#include "chao/ball_models/garden_jack_box_node_3.inc"
};
static Sint16 garden_jack_polygons[131] = {
#include "chao/ball_models/garden_jack_polygons.inc"
};
static Sint32 garden_jack_vertices[261] = {
#include "chao/ball_models/garden_jack_vertices.inc"
};
NJS_CNK_MODEL garden_jack_model = {
#include "chao/ball_models/garden_jack_model.inc"
};
static NJS_CNK_MODEL jack_empty_bounds = {
#include "chao/ball_models/jack_empty_bounds.inc"
};
static NJS_CNK_OBJECT garden_jack_box_node_2 = {
#include "chao/ball_models/garden_jack_box_node_2.inc"
};
static Sint16 garden_jack_box_node_1_polygons[185] = {
#include "chao/ball_models/garden_jack_box_node_1_polygons.inc"
};
static Sint32 garden_jack_box_node_1_vertices[219] = {
#include "chao/ball_models/garden_jack_box_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_jack_box_node_1_model = {
#include "chao/ball_models/garden_jack_box_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_jack_box_node_1 = {
#include "chao/ball_models/garden_jack_box_node_1.inc"
};
NJS_CNK_OBJECT garden_jack_box[1] = {
#include "chao/ball_models/garden_jack_box.inc"
};
static NJS_MKEY_F garden_jack_lid_motion_joint_2_position[25] = {
#include "chao/ball_models/garden_jack_lid_motion_joint_2_position.inc"
};
static NJS_MKEY_A garden_jack_lid_motion_joint_2_angles[25] = {
#include "chao/ball_models/garden_jack_lid_motion_joint_2_angles.inc"
};
static NJS_MKEY_A garden_jack_lid_motion_joint_3_angles[10] = {
#include "chao/ball_models/garden_jack_lid_motion_joint_3_angles.inc"
};
static NJS_MKEY_A garden_jack_lid_motion_joint_4_angles[10] = {
#include "chao/ball_models/garden_jack_lid_motion_joint_4_angles.inc"
};
static NJS_MKEY_A garden_jack_lid_motion_joint_5_angles[12] = {
#include "chao/ball_models/garden_jack_lid_motion_joint_5_angles.inc"
};
static NJS_MDATA2 garden_jack_lid_motion_tracks[6] = {
#include "chao/ball_models/garden_jack_lid_motion_tracks.inc"
};
NJS_MOTION garden_jack_lid_motion[1] = {
#include "chao/ball_models/garden_jack_lid_motion.inc"
};
