/* the monkey's two hops, and the turn from the second to the first. played
 * backward, the turn goes from the first to the second */
#ifndef SHARED_MONKEY_ART_HOPS_H
#define SHARED_MONKEY_ART_HOPS_H

#include <ninja.h>

static NJS_MKEY_F hop_a_body_position[3] = {
#include "shared/monkey_art/hops/hop_a_body_position.inc"
};

static NJS_MKEY_A hop_a_body_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_body_rotation.inc"
};

static NJS_MKEY_F hop_a_tail_position[3] = {
#include "shared/monkey_art/hops/hop_a_tail_position.inc"
};

static NJS_MKEY_A hop_a_tail_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_tail_rotation.inc"
};

static NJS_MKEY_F hop_a_left_thigh_position[3] = {
#include "shared/monkey_art/hops/hop_a_left_thigh_position.inc"
};

static NJS_MKEY_A hop_a_left_thigh_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_left_thigh_rotation.inc"
};

static NJS_MKEY_A hop_a_left_shin_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_left_shin_rotation.inc"
};

static NJS_MKEY_A hop_a_left_foot_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_left_foot_rotation.inc"
};

static NJS_MKEY_F hop_a_right_thigh_position[3] = {
#include "shared/monkey_art/hops/hop_a_right_thigh_position.inc"
};

static NJS_MKEY_A hop_a_right_thigh_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_right_thigh_rotation.inc"
};

static NJS_MKEY_A hop_a_right_shin_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_right_shin_rotation.inc"
};

static NJS_MKEY_A hop_a_right_foot_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_right_foot_rotation.inc"
};

static NJS_MKEY_F hop_a_right_upper_arm_position[2] = {
#include "shared/monkey_art/hops/hop_a_right_upper_arm_position.inc"
};

static NJS_MKEY_A hop_a_right_upper_arm_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_right_upper_arm_rotation.inc"
};

static NJS_MKEY_A hop_a_right_forearm_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_right_forearm_rotation.inc"
};

static NJS_MKEY_A hop_a_right_hand_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_right_hand_rotation.inc"
};

static NJS_MKEY_F hop_a_left_upper_arm_position[2] = {
#include "shared/monkey_art/hops/hop_a_left_upper_arm_position.inc"
};

static NJS_MKEY_A hop_a_left_upper_arm_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_left_upper_arm_rotation.inc"
};

static NJS_MKEY_A hop_a_left_forearm_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_left_forearm_rotation.inc"
};

static NJS_MKEY_A hop_a_left_hand_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_left_hand_rotation.inc"
};

static NJS_MKEY_A hop_a_head_rotation[3] = {
#include "shared/monkey_art/hops/hop_a_head_rotation.inc"
};

static NJS_MDATA2 hop_a_tracks[15] = {
#include "shared/monkey_art/hops/hop_a_tracks.inc"
};

static NJS_MOTION monkey_hop_a_motion = {
#include "shared/monkey_art/hops/monkey_hop_a_motion.inc"
};

static NJS_MKEY_F hop_b_body_position[3] = {
#include "shared/monkey_art/hops/hop_b_body_position.inc"
};

static NJS_MKEY_A hop_b_body_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_body_rotation.inc"
};

static NJS_MKEY_F hop_b_tail_position[3] = {
#include "shared/monkey_art/hops/hop_b_tail_position.inc"
};

static NJS_MKEY_A hop_b_tail_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_tail_rotation.inc"
};

static NJS_MKEY_F hop_b_left_thigh_position[3] = {
#include "shared/monkey_art/hops/hop_b_left_thigh_position.inc"
};

static NJS_MKEY_A hop_b_left_thigh_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_left_thigh_rotation.inc"
};

static NJS_MKEY_A hop_b_left_shin_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_left_shin_rotation.inc"
};

static NJS_MKEY_A hop_b_left_foot_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_left_foot_rotation.inc"
};

static NJS_MKEY_F hop_b_right_thigh_position[3] = {
#include "shared/monkey_art/hops/hop_b_right_thigh_position.inc"
};

static NJS_MKEY_A hop_b_right_thigh_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_right_thigh_rotation.inc"
};

static NJS_MKEY_A hop_b_right_shin_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_right_shin_rotation.inc"
};

static NJS_MKEY_A hop_b_right_foot_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_right_foot_rotation.inc"
};

static NJS_MKEY_F hop_b_right_upper_arm_position[2] = {
#include "shared/monkey_art/hops/hop_b_right_upper_arm_position.inc"
};

static NJS_MKEY_A hop_b_right_upper_arm_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_right_upper_arm_rotation.inc"
};

static NJS_MKEY_A hop_b_right_forearm_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_right_forearm_rotation.inc"
};

static NJS_MKEY_A hop_b_right_hand_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_right_hand_rotation.inc"
};

static NJS_MKEY_F hop_b_left_upper_arm_position[2] = {
#include "shared/monkey_art/hops/hop_b_left_upper_arm_position.inc"
};

static NJS_MKEY_A hop_b_left_upper_arm_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_left_upper_arm_rotation.inc"
};

static NJS_MKEY_A hop_b_left_forearm_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_left_forearm_rotation.inc"
};

static NJS_MKEY_A hop_b_left_hand_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_left_hand_rotation.inc"
};

static NJS_MKEY_A hop_b_head_rotation[3] = {
#include "shared/monkey_art/hops/hop_b_head_rotation.inc"
};

static NJS_MDATA2 hop_b_tracks[15] = {
#include "shared/monkey_art/hops/hop_b_tracks.inc"
};

static NJS_MOTION monkey_hop_b_motion = {
#include "shared/monkey_art/hops/monkey_hop_b_motion.inc"
};

static NJS_MKEY_F turn_body_position[5] = {
#include "shared/monkey_art/hops/turn_body_position.inc"
};

static NJS_MKEY_A turn_body_rotation[5] = {
#include "shared/monkey_art/hops/turn_body_rotation.inc"
};

static NJS_MKEY_F turn_tail_position[5] = {
#include "shared/monkey_art/hops/turn_tail_position.inc"
};

static NJS_MKEY_A turn_tail_rotation[5] = {
#include "shared/monkey_art/hops/turn_tail_rotation.inc"
};

static NJS_MKEY_F turn_left_thigh_position[5] = {
#include "shared/monkey_art/hops/turn_left_thigh_position.inc"
};

static NJS_MKEY_A turn_left_thigh_rotation[5] = {
#include "shared/monkey_art/hops/turn_left_thigh_rotation.inc"
};

static NJS_MKEY_A turn_left_shin_rotation[5] = {
#include "shared/monkey_art/hops/turn_left_shin_rotation.inc"
};

static NJS_MKEY_A turn_left_foot_rotation[5] = {
#include "shared/monkey_art/hops/turn_left_foot_rotation.inc"
};

static NJS_MKEY_F turn_right_thigh_position[5] = {
#include "shared/monkey_art/hops/turn_right_thigh_position.inc"
};

static NJS_MKEY_A turn_right_thigh_rotation[5] = {
#include "shared/monkey_art/hops/turn_right_thigh_rotation.inc"
};

static NJS_MKEY_A turn_right_shin_rotation[5] = {
#include "shared/monkey_art/hops/turn_right_shin_rotation.inc"
};

static NJS_MKEY_A turn_right_foot_rotation[5] = {
#include "shared/monkey_art/hops/turn_right_foot_rotation.inc"
};

static NJS_MKEY_F turn_right_upper_arm_position[2] = {
#include "shared/monkey_art/hops/turn_right_upper_arm_position.inc"
};

static NJS_MKEY_A turn_right_upper_arm_rotation[5] = {
#include "shared/monkey_art/hops/turn_right_upper_arm_rotation.inc"
};

static NJS_MKEY_A turn_right_forearm_rotation[5] = {
#include "shared/monkey_art/hops/turn_right_forearm_rotation.inc"
};

static NJS_MKEY_A turn_right_hand_rotation[5] = {
#include "shared/monkey_art/hops/turn_right_hand_rotation.inc"
};

static NJS_MKEY_F turn_left_upper_arm_position[2] = {
#include "shared/monkey_art/hops/turn_left_upper_arm_position.inc"
};

static NJS_MKEY_A turn_left_upper_arm_rotation[5] = {
#include "shared/monkey_art/hops/turn_left_upper_arm_rotation.inc"
};

static NJS_MKEY_A turn_left_forearm_rotation[5] = {
#include "shared/monkey_art/hops/turn_left_forearm_rotation.inc"
};

static NJS_MKEY_A turn_left_hand_rotation[5] = {
#include "shared/monkey_art/hops/turn_left_hand_rotation.inc"
};

static NJS_MKEY_A turn_head_rotation[5] = {
#include "shared/monkey_art/hops/turn_head_rotation.inc"
};

static NJS_MDATA2 turn_tracks[15] = {
#include "shared/monkey_art/hops/turn_tracks.inc"
};

static NJS_MOTION monkey_turn_motion = {
#include "shared/monkey_art/hops/monkey_turn_motion.inc"
};

#endif
