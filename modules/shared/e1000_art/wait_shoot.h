/* the E-1000's motions as it waits and as it shoots. each has a channel for
 * each of e1000_object's 14 nodes */
#ifndef SHARED_E1000_ART_WAIT_SHOOT_H
#define SHARED_E1000_ART_WAIT_SHOOT_H

#include <ninja.h>

static NJS_MKEY_F wait_body_position[2] = {
#include "shared/e1000_art/wait_shoot/wait_body_position.inc"
};

static NJS_MKEY_F wait_housing_position[2] = {
#include "shared/e1000_art/wait_shoot/wait_housing_position.inc"
};

static NJS_MKEY_A wait_housing_link_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_housing_link_rotation.inc"
};

static NJS_MKEY_A wait_high_thigh_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_high_thigh_rotation.inc"
};

static NJS_MKEY_A wait_high_shin_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_high_shin_rotation.inc"
};

static NJS_MKEY_A wait_high_foot_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_high_foot_rotation.inc"
};

static NJS_MKEY_A wait_gun_high_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_gun_high_rotation.inc"
};

static NJS_MKEY_A wait_barrel_high_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_barrel_high_rotation.inc"
};

static NJS_MKEY_A wait_low_thigh_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_low_thigh_rotation.inc"
};

static NJS_MKEY_A wait_low_shin_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_low_shin_rotation.inc"
};

static NJS_MKEY_A wait_low_foot_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_low_foot_rotation.inc"
};

static NJS_MKEY_A wait_gun_low_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_gun_low_rotation.inc"
};

static NJS_MKEY_A wait_barrel_low_rotation[2] = {
#include "shared/e1000_art/wait_shoot/wait_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_wait_tracks[14] = {
#include "shared/e1000_art/wait_shoot/e1000_wait_tracks.inc"
};

static NJS_MOTION e1000_wait_motion = {
#include "shared/e1000_art/wait_shoot/e1000_wait_motion.inc"
};

static NJS_MKEY_F shoot_body_position[7] = {
#include "shared/e1000_art/wait_shoot/shoot_body_position.inc"
};

static NJS_MKEY_A shoot_body_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_body_rotation.inc"
};

static NJS_MKEY_F shoot_housing_position[7] = {
#include "shared/e1000_art/wait_shoot/shoot_housing_position.inc"
};

static NJS_MKEY_A shoot_housing_link_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_housing_link_rotation.inc"
};

static NJS_MKEY_A shoot_high_thigh_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_high_thigh_rotation.inc"
};

static NJS_MKEY_A shoot_high_shin_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_high_shin_rotation.inc"
};

static NJS_MKEY_A shoot_high_foot_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_high_foot_rotation.inc"
};

static NJS_MKEY_A shoot_gun_high_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_gun_high_rotation.inc"
};

static NJS_MKEY_A shoot_barrel_high_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_barrel_high_rotation.inc"
};

static NJS_MKEY_A shoot_low_thigh_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_low_thigh_rotation.inc"
};

static NJS_MKEY_A shoot_low_shin_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_low_shin_rotation.inc"
};

static NJS_MKEY_A shoot_low_foot_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_low_foot_rotation.inc"
};

static NJS_MKEY_A shoot_gun_low_rotation[7] = {
#include "shared/e1000_art/wait_shoot/shoot_gun_low_rotation.inc"
};

static NJS_MKEY_A shoot_barrel_low_rotation[5] = {
#include "shared/e1000_art/wait_shoot/shoot_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_shoot_tracks[14] = {
#include "shared/e1000_art/wait_shoot/e1000_shoot_tracks.inc"
};

static NJS_MOTION e1000_shoot_motion = {
#include "shared/e1000_art/wait_shoot/e1000_shoot_motion.inc"
};

#endif
