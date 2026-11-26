/* the monkey's throw of a heavy bomb, and the bomb's motion through it */
#ifndef SHARED_MONKEY_ART_HEAVY_THROW_H
#define SHARED_MONKEY_ART_HEAVY_THROW_H

#include <ninja.h>

static NJS_MKEY_F heavy_throw_bomb_bomb_position[6] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_bomb_bomb_position.inc"
};

static NJS_MKEY_A heavy_throw_bomb_bomb_rotation[6] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_bomb_bomb_rotation.inc"
};

static NJS_MDATA2 heavy_throw_bomb_tracks[1] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_bomb_tracks.inc"
};

static NJS_MOTION monkey_heavy_throw_bomb_motion = {
#include "shared/monkey_art/heavy_throw/monkey_heavy_throw_bomb_motion.inc"
};

static NJS_MKEY_F heavy_throw_body_position[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_body_position.inc"
};

static NJS_MKEY_A heavy_throw_body_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_body_rotation.inc"
};

static NJS_MKEY_F heavy_throw_tail_position[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_tail_position.inc"
};

static NJS_MKEY_A heavy_throw_tail_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_tail_rotation.inc"
};

static NJS_MKEY_F heavy_throw_left_thigh_position[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_thigh_position.inc"
};

static NJS_MKEY_A heavy_throw_left_thigh_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_thigh_rotation.inc"
};

static NJS_MKEY_A heavy_throw_left_shin_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_shin_rotation.inc"
};

static NJS_MKEY_A heavy_throw_left_foot_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_foot_rotation.inc"
};

static NJS_MKEY_F heavy_throw_right_thigh_position[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_thigh_position.inc"
};

static NJS_MKEY_A heavy_throw_right_thigh_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_thigh_rotation.inc"
};

static NJS_MKEY_A heavy_throw_right_shin_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_shin_rotation.inc"
};

static NJS_MKEY_A heavy_throw_right_foot_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_foot_rotation.inc"
};

static NJS_MKEY_F heavy_throw_right_upper_arm_position[2] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_upper_arm_position.inc"
};

static NJS_MKEY_A heavy_throw_right_upper_arm_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_upper_arm_rotation.inc"
};

static NJS_MKEY_A heavy_throw_right_forearm_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_forearm_rotation.inc"
};

static NJS_MKEY_A heavy_throw_right_hand_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_right_hand_rotation.inc"
};

static NJS_MKEY_F heavy_throw_left_upper_arm_position[2] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_upper_arm_position.inc"
};

static NJS_MKEY_A heavy_throw_left_upper_arm_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_upper_arm_rotation.inc"
};

static NJS_MKEY_A heavy_throw_left_forearm_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_forearm_rotation.inc"
};

static NJS_MKEY_A heavy_throw_left_hand_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_left_hand_rotation.inc"
};

static NJS_MKEY_A heavy_throw_head_rotation[10] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_head_rotation.inc"
};

static NJS_MDATA2 heavy_throw_tracks[15] = {
#include "shared/monkey_art/heavy_throw/heavy_throw_tracks.inc"
};

static NJS_MOTION monkey_heavy_throw_motion = {
#include "shared/monkey_art/heavy_throw/monkey_heavy_throw_motion.inc"
};

#endif
