/* CHAOSTGRACE.PRS's chunk models and motions, from the disc. its D lies
 * between track_events.c and texture_slots.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgrace/course_models.h"
#include "chaostgrace/crab_art.h"
#include "chaostgrace/props.h"

static Sint16 crab_model_right_pincer_polygons[63] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_pincer_polygons.inc"
};
static Sint32 crab_model_right_pincer_vertices[39] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_pincer_vertices.inc"
};
static NJS_CNK_MODEL crab_model_right_pincer_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_pincer_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_right_pincer = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_pincer.inc"
};
static Sint16 crab_model_right_claw_polygons[87] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_claw_polygons.inc"
};
static Sint32 crab_model_right_claw_vertices[57] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_claw_vertices.inc"
};
static NJS_CNK_MODEL crab_model_right_claw_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_claw_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_right_claw = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_claw.inc"
};
static Sint16 crab_model_right_inner_leg_polygons[37] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_inner_leg_polygons.inc"
};
static Sint32 crab_model_right_inner_leg_vertices[33] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_inner_leg_vertices.inc"
};
static NJS_CNK_MODEL crab_model_right_inner_leg_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_inner_leg_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_right_inner_leg = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_inner_leg.inc"
};
static Sint16 crab_model_right_outer_leg_polygons[37] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_outer_leg_polygons.inc"
};
static Sint32 crab_model_right_outer_leg_vertices[33] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_outer_leg_vertices.inc"
};
static NJS_CNK_MODEL crab_model_right_outer_leg_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_outer_leg_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_right_outer_leg = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_outer_leg.inc"
};
static Sint16 crab_model_right_eye_polygons[171] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_eye_polygons.inc"
};
static Sint32 crab_model_right_eye_vertices[111] = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_eye_vertices.inc"
};
static NJS_CNK_MODEL crab_model_right_eye_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_eye_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_right_eye = {
#include "chaostgrace/chao_race_crab_model/crab_model_right_eye.inc"
};
static Sint16 crab_model_left_eye_polygons[169] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_eye_polygons.inc"
};
static Sint32 crab_model_left_eye_vertices[111] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_eye_vertices.inc"
};
static NJS_CNK_MODEL crab_model_left_eye_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_eye_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_left_eye = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_eye.inc"
};
static NJS_CNK_OBJECT crab_model_eye_mount = {
#include "chaostgrace/chao_race_crab_model/crab_model_eye_mount.inc"
};
static Sint16 crab_model_upper_shell_polygons[2] = {
#include "chaostgrace/chao_race_crab_model/crab_model_upper_shell_polygons.inc"
};
static Sint32 crab_model_upper_shell_vertices[240] = {
#include "chaostgrace/chao_race_crab_model/crab_model_upper_shell_vertices.inc"
};
static NJS_CNK_MODEL crab_model_upper_shell_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_upper_shell_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_upper_shell = {
#include "chaostgrace/chao_race_crab_model/crab_model_upper_shell.inc"
};
static NJS_CNK_OBJECT crab_model_upper_shell_pivot = {
#include "chaostgrace/chao_race_crab_model/crab_model_upper_shell_pivot.inc"
};
static NJS_CNK_OBJECT crab_model_lower_shell_tip = {
#include "chaostgrace/chao_race_crab_model/crab_model_lower_shell_tip.inc"
};
static Sint32 crab_model_lower_shell_vertices[248] = {
#include "chaostgrace/chao_race_crab_model/crab_model_lower_shell_vertices.inc"
};
static NJS_CNK_MODEL crab_model_lower_shell_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_lower_shell_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_lower_shell = {
#include "chaostgrace/chao_race_crab_model/crab_model_lower_shell.inc"
};
static NJS_CNK_OBJECT crab_model_lower_shell_pivot = {
#include "chaostgrace/chao_race_crab_model/crab_model_lower_shell_pivot.inc"
};
static Sint16 crab_model_left_pincer_polygons[63] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_pincer_polygons.inc"
};
static Sint32 crab_model_left_pincer_vertices[39] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_pincer_vertices.inc"
};
static NJS_CNK_MODEL crab_model_left_pincer_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_pincer_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_left_pincer = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_pincer.inc"
};
static Sint16 crab_model_left_claw_polygons[87] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_claw_polygons.inc"
};
static Sint32 crab_model_left_claw_vertices[57] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_claw_vertices.inc"
};
static NJS_CNK_MODEL crab_model_left_claw_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_claw_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_left_claw = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_claw.inc"
};
static Sint16 crab_model_left_inner_leg_polygons[37] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_inner_leg_polygons.inc"
};
static Sint32 crab_model_left_inner_leg_vertices[33] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_inner_leg_vertices.inc"
};
static NJS_CNK_MODEL crab_model_left_inner_leg_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_inner_leg_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_left_inner_leg = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_inner_leg.inc"
};
static Sint16 crab_model_left_outer_leg_polygons[37] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_outer_leg_polygons.inc"
};
static Sint32 crab_model_left_outer_leg_vertices[33] = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_outer_leg_vertices.inc"
};
static NJS_CNK_MODEL crab_model_left_outer_leg_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_outer_leg_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_left_outer_leg = {
#include "chaostgrace/chao_race_crab_model/crab_model_left_outer_leg.inc"
};
static Sint32 crab_model_body_vertices[59] = {
#include "chaostgrace/chao_race_crab_model/crab_model_body_vertices.inc"
};
static NJS_CNK_MODEL crab_model_body_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_body_mesh.inc"
};
static NJS_CNK_OBJECT crab_model_body = {
#include "chaostgrace/chao_race_crab_model/crab_model_body.inc"
};
static Sint16 crab_model_shell_polygons[409] = {
#include "chaostgrace/chao_race_crab_model/crab_model_shell_polygons.inc"
};
static NJS_CNK_MODEL crab_model_shell_mesh = {
#include "chaostgrace/chao_race_crab_model/crab_model_shell_mesh.inc"
};
NJS_CNK_OBJECT race_crab_model = {
#include "chaostgrace/chao_race_crab_model/race_crab_model.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_2_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_2_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_3_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_3_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_4_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_4_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_5_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_5_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_7_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_7_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_10_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_10_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_12_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_12_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_13_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_13_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_14_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_14_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_15_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_15_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_16_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_16_angles.inc"
};
static NJS_MKEY_A crab_walk_motion_joint_17_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_joint_17_angles.inc"
};
static NJS_MDATA2 crab_walk_motion_tracks[18] = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion_tracks.inc"
};
NJS_MOTION crab_walk_motion = {
#include "chaostgrace/chao_race_crab_model/crab_walk_motion.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_0_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_0_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_2_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_2_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_4_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_4_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_5_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_5_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_7_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_7_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_10_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_10_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_12_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_12_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_13_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_13_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_14_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_14_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_16_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_16_angles.inc"
};
static NJS_MKEY_A crab_hold_motion_joint_17_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_joint_17_angles.inc"
};
static NJS_MDATA2 crab_hold_motion_tracks[18] = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion_tracks.inc"
};
NJS_MOTION crab_hold_motion = {
#include "chaostgrace/chao_race_crab_model/crab_hold_motion.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_0_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_0_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_4_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_4_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_5_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_5_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_7_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_7_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_10_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_10_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_12_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_12_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_13_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_13_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_16_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_16_angles.inc"
};
static NJS_MKEY_A crab_idle_motion_joint_17_angles[8] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_joint_17_angles.inc"
};
static NJS_MDATA2 crab_idle_motion_tracks[18] = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion_tracks.inc"
};
NJS_MOTION crab_idle_motion = {
#include "chaostgrace/chao_race_crab_model/crab_idle_motion.inc"
};
static NJS_CNK_OBJECT orchard_models_fruit_right = {
#include "chaostgrace/chao_race_crab_model/orchard_models_fruit_right.inc"
};
static NJS_CNK_OBJECT orchard_models_fruit_left = {
#include "chaostgrace/chao_race_crab_model/orchard_models_fruit_left.inc"
};
static NJS_CNK_OBJECT orchard_models_fruit_front = {
#include "chaostgrace/chao_race_crab_model/orchard_models_fruit_front.inc"
};
static Sint16 orchard_models_branch_left_polygons[57] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_left_polygons.inc"
};
static Sint32 orchard_models_branch_left_vertices[63] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_left_vertices.inc"
};
static NJS_CNK_MODEL orchard_models_branch_left_model = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_left_model.inc"
};
static NJS_CNK_OBJECT orchard_models_branch_left = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_left.inc"
};
static Sint16 orchard_models_branch_back_polygons[57] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_back_polygons.inc"
};
static Sint32 orchard_models_branch_back_vertices[63] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_back_vertices.inc"
};
static NJS_CNK_MODEL orchard_models_branch_back_model = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_back_model.inc"
};
static NJS_CNK_OBJECT orchard_models_branch_back = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_back.inc"
};
static Sint16 orchard_models_branch_right_polygons[57] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_right_polygons.inc"
};
static Sint32 orchard_models_branch_right_vertices[63] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_right_vertices.inc"
};
static NJS_CNK_MODEL orchard_models_branch_right_model = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_right_model.inc"
};
static NJS_CNK_OBJECT orchard_models_branch_right = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_right.inc"
};
static Sint16 orchard_models_branch_front_polygons[57] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_front_polygons.inc"
};
static Sint32 orchard_models_branch_front_vertices[63] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_front_vertices.inc"
};
static NJS_CNK_MODEL orchard_models_branch_front_model = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_front_model.inc"
};
static NJS_CNK_OBJECT orchard_models_branch_front = {
#include "chaostgrace/chao_race_crab_model/orchard_models_branch_front.inc"
};
static Sint16 orchard_models_tree_polygons[157] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_tree_polygons.inc"
};
static Sint32 orchard_models_tree_vertices[105] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_tree_vertices.inc"
};
static NJS_CNK_MODEL orchard_models_tree_model = {
#include "chaostgrace/chao_race_crab_model/orchard_models_tree_model.inc"
};
NJS_CNK_OBJECT race_tree_model = {
#include "chaostgrace/chao_race_crab_model/race_tree_model.inc"
};
static Sint16 orchard_models_fruit_polygons[163] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_fruit_polygons.inc"
};
static Sint32 orchard_models_fruit_vertices[123] = {
#include "chaostgrace/chao_race_crab_model/orchard_models_fruit_vertices.inc"
};
static NJS_CNK_MODEL orchard_models_fruit_model = {
#include "chaostgrace/chao_race_crab_model/orchard_models_fruit_model.inc"
};
NJS_CNK_OBJECT race_fruit_model[1] = {
#include "chaostgrace/chao_race_crab_model/race_fruit_model.inc"
};
static Sint16 neutral_logs_offset_base_polygons[119] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_offset_base_polygons.inc"
};
static Sint32 neutral_logs_offset_base_vertices[87] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_offset_base_vertices.inc"
};
static NJS_CNK_MODEL neutral_logs_offset_base_model = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_offset_base_model.inc"
};
static NJS_CNK_OBJECT neutral_logs_offset_base = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_offset_base.inc"
};
static Sint16 neutral_logs_neutral_log_a_polygons[583] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_a_polygons.inc"
};
static Sint32 neutral_logs_neutral_log_a_vertices[407] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_a_vertices.inc"
};
static NJS_CNK_MODEL neutral_logs_neutral_log_a_model = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_a_model.inc"
};
NJS_CNK_OBJECT race_neutral_log_a = {
#include "chaostgrace/chao_race_crab_model/race_neutral_log_a.inc"
};
static Sint16 neutral_logs_neutral_log_b_polygons[573] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_b_polygons.inc"
};
static Sint32 neutral_logs_neutral_log_b_vertices[395] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_b_vertices.inc"
};
static NJS_CNK_MODEL neutral_logs_neutral_log_b_model = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_b_model.inc"
};
NJS_CNK_OBJECT race_neutral_log_b = {
#include "chaostgrace/chao_race_crab_model/race_neutral_log_b.inc"
};
static Sint16 neutral_logs_neutral_log_c_polygons[509] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_c_polygons.inc"
};
static Sint32 neutral_logs_neutral_log_c_vertices[359] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_c_vertices.inc"
};
static NJS_CNK_MODEL neutral_logs_neutral_log_c_model = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_neutral_log_c_model.inc"
};
NJS_CNK_OBJECT race_neutral_log_c = {
#include "chaostgrace/chao_race_crab_model/race_neutral_log_c.inc"
};
static Sint16 neutral_logs_upright_short_polygons[135] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_short_polygons.inc"
};
static Sint32 neutral_logs_upright_short_vertices[99] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_short_vertices.inc"
};
static NJS_CNK_MODEL neutral_logs_upright_short_model = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_short_model.inc"
};
static NJS_CNK_OBJECT neutral_logs_upright_short = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_short.inc"
};
static Sint16 neutral_logs_upright_medium_polygons[135] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_medium_polygons.inc"
};
static Sint32 neutral_logs_upright_medium_vertices[99] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_medium_vertices.inc"
};
static NJS_CNK_MODEL neutral_logs_upright_medium_model = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_medium_model.inc"
};
static NJS_CNK_OBJECT neutral_logs_upright_medium = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_medium.inc"
};
static Sint16 neutral_logs_upright_tall_polygons[157] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_tall_polygons.inc"
};
static Sint32 neutral_logs_upright_tall_vertices[111] = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_tall_vertices.inc"
};
static NJS_CNK_MODEL neutral_logs_upright_tall_model = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_tall_model.inc"
};
static NJS_CNK_OBJECT neutral_logs_upright_tall = {
#include "chaostgrace/chao_race_crab_model/neutral_logs_upright_tall.inc"
};
static Sint16 hero_logs_base_polygons[119] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_base_polygons.inc"
};
static Sint32 hero_logs_base_vertices[87] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_base_vertices.inc"
};
static NJS_CNK_MODEL hero_logs_base_model = {
#include "chaostgrace/chao_race_crab_model/hero_logs_base_model.inc"
};
static NJS_CNK_OBJECT hero_logs_base = {
#include "chaostgrace/chao_race_crab_model/hero_logs_base.inc"
};
static Sint16 hero_logs_hero_log_a_polygons[583] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_a_polygons.inc"
};
static Sint32 hero_logs_hero_log_a_vertices[407] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_a_vertices.inc"
};
static NJS_CNK_MODEL hero_logs_hero_log_a_model = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_a_model.inc"
};
NJS_CNK_OBJECT race_hero_log_a = {
#include "chaostgrace/chao_race_crab_model/race_hero_log_a.inc"
};
static Sint16 hero_logs_hero_log_b_polygons[573] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_b_polygons.inc"
};
static Sint32 hero_logs_hero_log_b_vertices[395] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_b_vertices.inc"
};
static NJS_CNK_MODEL hero_logs_hero_log_b_model = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_b_model.inc"
};
NJS_CNK_OBJECT race_hero_log_b = {
#include "chaostgrace/chao_race_crab_model/race_hero_log_b.inc"
};
static Sint16 hero_logs_hero_log_c_polygons[509] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_c_polygons.inc"
};
static Sint32 hero_logs_hero_log_c_vertices[359] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_c_vertices.inc"
};
static NJS_CNK_MODEL hero_logs_hero_log_c_model = {
#include "chaostgrace/chao_race_crab_model/hero_logs_hero_log_c_model.inc"
};
NJS_CNK_OBJECT race_hero_log_c = {
#include "chaostgrace/chao_race_crab_model/race_hero_log_c.inc"
};
static Sint16 hero_logs_upright_short_polygons[135] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_short_polygons.inc"
};
static Sint32 hero_logs_upright_short_vertices[99] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_short_vertices.inc"
};
static NJS_CNK_MODEL hero_logs_upright_short_model = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_short_model.inc"
};
static NJS_CNK_OBJECT hero_logs_upright_short = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_short.inc"
};
static Sint16 hero_logs_upright_medium_polygons[135] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_medium_polygons.inc"
};
static Sint32 hero_logs_upright_medium_vertices[99] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_medium_vertices.inc"
};
static NJS_CNK_MODEL hero_logs_upright_medium_model = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_medium_model.inc"
};
static NJS_CNK_OBJECT hero_logs_upright_medium = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_medium.inc"
};
static Sint16 hero_logs_upright_tall_polygons[157] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_tall_polygons.inc"
};
static Sint32 hero_logs_upright_tall_vertices[111] = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_tall_vertices.inc"
};
static NJS_CNK_MODEL hero_logs_upright_tall_model = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_tall_model.inc"
};
static NJS_CNK_OBJECT hero_logs_upright_tall = {
#include "chaostgrace/chao_race_crab_model/hero_logs_upright_tall.inc"
};
static Sint16 dark_logs_base_polygons[113] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_base_polygons.inc"
};
static Sint32 dark_logs_base_vertices[75] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_base_vertices.inc"
};
static NJS_CNK_MODEL dark_logs_base_model = {
#include "chaostgrace/chao_race_crab_model/dark_logs_base_model.inc"
};
static NJS_CNK_OBJECT dark_logs_base = {
#include "chaostgrace/chao_race_crab_model/dark_logs_base.inc"
};
static Sint16 dark_logs_dark_log_a_polygons[565] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_a_polygons.inc"
};
static Sint32 dark_logs_dark_log_a_vertices[391] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_a_vertices.inc"
};
static NJS_CNK_MODEL dark_logs_dark_log_a_model = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_a_model.inc"
};
NJS_CNK_OBJECT race_dark_log_a = {
#include "chaostgrace/chao_race_crab_model/race_dark_log_a.inc"
};
static Sint16 dark_logs_dark_log_b_polygons[533] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_b_polygons.inc"
};
static Sint32 dark_logs_dark_log_b_vertices[355] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_b_vertices.inc"
};
static NJS_CNK_MODEL dark_logs_dark_log_b_model = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_b_model.inc"
};
NJS_CNK_OBJECT race_dark_log_b = {
#include "chaostgrace/chao_race_crab_model/race_dark_log_b.inc"
};
static Sint16 dark_logs_dark_log_c_polygons[463] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_c_polygons.inc"
};
static Sint32 dark_logs_dark_log_c_vertices[303] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_c_vertices.inc"
};
static NJS_CNK_MODEL dark_logs_dark_log_c_model = {
#include "chaostgrace/chao_race_crab_model/dark_logs_dark_log_c_model.inc"
};
NJS_CNK_OBJECT race_dark_log_c = {
#include "chaostgrace/chao_race_crab_model/race_dark_log_c.inc"
};
static Sint16 dark_logs_upright_short_polygons[135] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_short_polygons.inc"
};
static Sint32 dark_logs_upright_short_vertices[99] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_short_vertices.inc"
};
static NJS_CNK_MODEL dark_logs_upright_short_model = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_short_model.inc"
};
static NJS_CNK_OBJECT dark_logs_upright_short = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_short.inc"
};
static Sint16 dark_logs_upright_medium_polygons[135] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_medium_polygons.inc"
};
static Sint32 dark_logs_upright_medium_vertices[99] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_medium_vertices.inc"
};
static NJS_CNK_MODEL dark_logs_upright_medium_model = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_medium_model.inc"
};
static NJS_CNK_OBJECT dark_logs_upright_medium = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_medium.inc"
};
static Sint16 dark_logs_upright_tall_polygons[149] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_tall_polygons.inc"
};
static Sint32 dark_logs_upright_tall_vertices[107] = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_tall_vertices.inc"
};
static NJS_CNK_MODEL dark_logs_upright_tall_model = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_tall_model.inc"
};
static NJS_CNK_OBJECT dark_logs_upright_tall = {
#include "chaostgrace/chao_race_crab_model/dark_logs_upright_tall.inc"
};
