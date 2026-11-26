/* the monkey's hold of a heavy bomb, and the bomb's motion through it */
#ifndef SHARED_MONKEY_ART_HOLD_H
#define SHARED_MONKEY_ART_HOLD_H

#include <ninja.h>

static NJS_MKEY_F heavy_hold_bomb_bomb_position[4] = {
#include "shared/monkey_art/hold/heavy_hold_bomb_bomb_position.inc"
};

static NJS_MKEY_A heavy_hold_bomb_bomb_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_bomb_bomb_rotation.inc"
};

static NJS_MDATA2 heavy_hold_bomb_tracks[1] = {
#include "shared/monkey_art/hold/heavy_hold_bomb_tracks.inc"
};

static NJS_MOTION monkey_heavy_hold_bomb_motion = {
#include "shared/monkey_art/hold/monkey_heavy_hold_bomb_motion.inc"
};

static NJS_MKEY_F heavy_hold_body_position[4] = {
#include "shared/monkey_art/hold/heavy_hold_body_position.inc"
};

static NJS_MKEY_A heavy_hold_body_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_body_rotation.inc"
};

static NJS_MKEY_F heavy_hold_tail_position[4] = {
#include "shared/monkey_art/hold/heavy_hold_tail_position.inc"
};

static NJS_MKEY_A heavy_hold_tail_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_tail_rotation.inc"
};

static NJS_MKEY_F heavy_hold_left_thigh_position[4] = {
#include "shared/monkey_art/hold/heavy_hold_left_thigh_position.inc"
};

static NJS_MKEY_A heavy_hold_left_thigh_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_left_thigh_rotation.inc"
};

static NJS_MKEY_A heavy_hold_left_shin_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_left_shin_rotation.inc"
};

static NJS_MKEY_A heavy_hold_left_foot_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_left_foot_rotation.inc"
};

static NJS_MKEY_F heavy_hold_right_thigh_position[4] = {
#include "shared/monkey_art/hold/heavy_hold_right_thigh_position.inc"
};

static NJS_MKEY_A heavy_hold_right_thigh_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_right_thigh_rotation.inc"
};

static NJS_MKEY_A heavy_hold_right_shin_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_right_shin_rotation.inc"
};

static NJS_MKEY_A heavy_hold_right_foot_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_right_foot_rotation.inc"
};

static NJS_MKEY_F heavy_hold_right_upper_arm_position[2] = {
#include "shared/monkey_art/hold/heavy_hold_right_upper_arm_position.inc"
};

static NJS_MKEY_A heavy_hold_right_upper_arm_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_right_upper_arm_rotation.inc"
};

static NJS_MKEY_A heavy_hold_right_forearm_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_right_forearm_rotation.inc"
};

static NJS_MKEY_A heavy_hold_right_hand_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_right_hand_rotation.inc"
};

static NJS_MKEY_F heavy_hold_left_upper_arm_position[2] = {
#include "shared/monkey_art/hold/heavy_hold_left_upper_arm_position.inc"
};

static NJS_MKEY_A heavy_hold_left_upper_arm_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_left_upper_arm_rotation.inc"
};

static NJS_MKEY_A heavy_hold_left_forearm_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_left_forearm_rotation.inc"
};

static NJS_MKEY_A heavy_hold_left_hand_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_left_hand_rotation.inc"
};

static NJS_MKEY_A heavy_hold_head_rotation[4] = {
#include "shared/monkey_art/hold/heavy_hold_head_rotation.inc"
};

static NJS_MDATA2 heavy_hold_tracks[15] = {
#include "shared/monkey_art/hold/heavy_hold_tracks.inc"
};

static NJS_MOTION monkey_heavy_hold_motion = {
#include "shared/monkey_art/hold/monkey_heavy_hold_motion.inc"
};

#endif
