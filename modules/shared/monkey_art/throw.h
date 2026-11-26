/* the monkey's throw of a bomb that isn't heavy, and the bomb's motion through
 * it */
#ifndef SHARED_MONKEY_ART_THROW_H
#define SHARED_MONKEY_ART_THROW_H

#include <ninja.h>

static NJS_MKEY_F throw_bomb_bomb_position[6] = {
#include "shared/monkey_art/throw/throw_bomb_bomb_position.inc"
};

static NJS_MKEY_A throw_bomb_bomb_rotation[6] = {
#include "shared/monkey_art/throw/throw_bomb_bomb_rotation.inc"
};

static NJS_MDATA2 throw_bomb_tracks[1] = {
#include "shared/monkey_art/throw/throw_bomb_tracks.inc"
};

static NJS_MOTION monkey_throw_bomb_motion = {
#include "shared/monkey_art/throw/monkey_throw_bomb_motion.inc"
};

static NJS_MKEY_F throw_body_position[9] = {
#include "shared/monkey_art/throw/throw_body_position.inc"
};

static NJS_MKEY_A throw_body_rotation[9] = {
#include "shared/monkey_art/throw/throw_body_rotation.inc"
};

static NJS_MKEY_F throw_tail_position[9] = {
#include "shared/monkey_art/throw/throw_tail_position.inc"
};

static NJS_MKEY_A throw_tail_rotation[9] = {
#include "shared/monkey_art/throw/throw_tail_rotation.inc"
};

static NJS_MKEY_F throw_left_thigh_position[9] = {
#include "shared/monkey_art/throw/throw_left_thigh_position.inc"
};

static NJS_MKEY_A throw_left_thigh_rotation[9] = {
#include "shared/monkey_art/throw/throw_left_thigh_rotation.inc"
};

static NJS_MKEY_A throw_left_shin_rotation[9] = {
#include "shared/monkey_art/throw/throw_left_shin_rotation.inc"
};

static NJS_MKEY_A throw_left_foot_rotation[9] = {
#include "shared/monkey_art/throw/throw_left_foot_rotation.inc"
};

static NJS_MKEY_F throw_right_thigh_position[9] = {
#include "shared/monkey_art/throw/throw_right_thigh_position.inc"
};

static NJS_MKEY_A throw_right_thigh_rotation[9] = {
#include "shared/monkey_art/throw/throw_right_thigh_rotation.inc"
};

static NJS_MKEY_A throw_right_shin_rotation[9] = {
#include "shared/monkey_art/throw/throw_right_shin_rotation.inc"
};

static NJS_MKEY_A throw_right_foot_rotation[9] = {
#include "shared/monkey_art/throw/throw_right_foot_rotation.inc"
};

static NJS_MKEY_F throw_right_upper_arm_position[2] = {
#include "shared/monkey_art/throw/throw_right_upper_arm_position.inc"
};

static NJS_MKEY_A throw_right_upper_arm_rotation[9] = {
#include "shared/monkey_art/throw/throw_right_upper_arm_rotation.inc"
};

static NJS_MKEY_A throw_right_forearm_rotation[9] = {
#include "shared/monkey_art/throw/throw_right_forearm_rotation.inc"
};

static NJS_MKEY_A throw_right_hand_rotation[9] = {
#include "shared/monkey_art/throw/throw_right_hand_rotation.inc"
};

static NJS_MKEY_F throw_left_upper_arm_position[2] = {
#include "shared/monkey_art/throw/throw_left_upper_arm_position.inc"
};

static NJS_MKEY_A throw_left_upper_arm_rotation[9] = {
#include "shared/monkey_art/throw/throw_left_upper_arm_rotation.inc"
};

static NJS_MKEY_A throw_left_forearm_rotation[9] = {
#include "shared/monkey_art/throw/throw_left_forearm_rotation.inc"
};

static NJS_MKEY_A throw_left_hand_rotation[9] = {
#include "shared/monkey_art/throw/throw_left_hand_rotation.inc"
};

static NJS_MKEY_A throw_head_rotation[9] = {
#include "shared/monkey_art/throw/throw_head_rotation.inc"
};

static NJS_MDATA2 throw_tracks[15] = {
#include "shared/monkey_art/throw/throw_tracks.inc"
};

static NJS_MOTION monkey_throw_motion = {
#include "shared/monkey_art/throw/monkey_throw_motion.inc"
};

#endif
