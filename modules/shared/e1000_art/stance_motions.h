/* the E-1000's motions from waiting to standing and back, and standing. each
 * has a channel for each of e1000_object's 14 nodes */
#ifndef SHARED_E1000_ART_STANCE_MOTIONS_H
#define SHARED_E1000_ART_STANCE_MOTIONS_H

#include <ninja.h>

static NJS_MKEY_F to_stand_body_position[3] = {
#include "shared/e1000_art/stance_motions/to_stand_body_position.inc"
};

static NJS_MKEY_F to_stand_housing_position[3] = {
#include "shared/e1000_art/stance_motions/to_stand_housing_position.inc"
};

static NJS_MKEY_A to_stand_housing_link_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_housing_link_rotation.inc"
};

static NJS_MKEY_A to_stand_high_thigh_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_high_thigh_rotation.inc"
};

static NJS_MKEY_A to_stand_high_shin_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_high_shin_rotation.inc"
};

static NJS_MKEY_A to_stand_high_foot_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_high_foot_rotation.inc"
};

static NJS_MKEY_A to_stand_gun_high_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_gun_high_rotation.inc"
};

static NJS_MKEY_A to_stand_barrel_high_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_barrel_high_rotation.inc"
};

static NJS_MKEY_A to_stand_low_thigh_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_low_thigh_rotation.inc"
};

static NJS_MKEY_A to_stand_low_shin_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_low_shin_rotation.inc"
};

static NJS_MKEY_A to_stand_low_foot_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_low_foot_rotation.inc"
};

static NJS_MKEY_A to_stand_gun_low_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_gun_low_rotation.inc"
};

static NJS_MKEY_A to_stand_barrel_low_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_stand_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_to_stand_tracks[14] = {
#include "shared/e1000_art/stance_motions/e1000_to_stand_tracks.inc"
};

static NJS_MOTION e1000_to_stand_motion = {
#include "shared/e1000_art/stance_motions/e1000_to_stand_motion.inc"
};

static NJS_MKEY_F to_wait_body_position[3] = {
#include "shared/e1000_art/stance_motions/to_wait_body_position.inc"
};

static NJS_MKEY_F to_wait_housing_position[3] = {
#include "shared/e1000_art/stance_motions/to_wait_housing_position.inc"
};

static NJS_MKEY_A to_wait_housing_link_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_housing_link_rotation.inc"
};

static NJS_MKEY_A to_wait_high_thigh_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_high_thigh_rotation.inc"
};

static NJS_MKEY_A to_wait_high_shin_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_high_shin_rotation.inc"
};

static NJS_MKEY_A to_wait_high_foot_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_high_foot_rotation.inc"
};

static NJS_MKEY_A to_wait_gun_high_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_gun_high_rotation.inc"
};

static NJS_MKEY_A to_wait_barrel_high_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_barrel_high_rotation.inc"
};

static NJS_MKEY_A to_wait_low_thigh_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_low_thigh_rotation.inc"
};

static NJS_MKEY_A to_wait_low_shin_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_low_shin_rotation.inc"
};

static NJS_MKEY_A to_wait_low_foot_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_low_foot_rotation.inc"
};

static NJS_MKEY_A to_wait_gun_low_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_gun_low_rotation.inc"
};

static NJS_MKEY_A to_wait_barrel_low_rotation[3] = {
#include "shared/e1000_art/stance_motions/to_wait_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_to_wait_tracks[14] = {
#include "shared/e1000_art/stance_motions/e1000_to_wait_tracks.inc"
};

static NJS_MOTION e1000_to_wait_motion = {
#include "shared/e1000_art/stance_motions/e1000_to_wait_motion.inc"
};

static NJS_MKEY_F unused_pose_body_position[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_body_position.inc"
};

static NJS_MKEY_F unused_pose_housing_position[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_housing_position.inc"
};

static NJS_MKEY_A unused_pose_housing_link_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_housing_link_rotation.inc"
};

static NJS_MKEY_A unused_pose_high_thigh_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_high_thigh_rotation.inc"
};

static NJS_MKEY_A unused_pose_high_shin_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_high_shin_rotation.inc"
};

static NJS_MKEY_A unused_pose_high_foot_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_high_foot_rotation.inc"
};

static NJS_MKEY_A unused_pose_gun_high_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_gun_high_rotation.inc"
};

static NJS_MKEY_A unused_pose_barrel_high_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_barrel_high_rotation.inc"
};

static NJS_MKEY_A unused_pose_low_thigh_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_low_thigh_rotation.inc"
};

static NJS_MKEY_A unused_pose_low_shin_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_low_shin_rotation.inc"
};

static NJS_MKEY_A unused_pose_low_foot_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_low_foot_rotation.inc"
};

static NJS_MKEY_A unused_pose_gun_low_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_gun_low_rotation.inc"
};

static NJS_MKEY_A unused_pose_barrel_low_rotation[2] = {
#include "shared/e1000_art/stance_motions/unused_pose_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_unused_pose_tracks[14] = {
#include "shared/e1000_art/stance_motions/e1000_unused_pose_tracks.inc"
};

/* a pose that no action of e1000_actions plays */
static NJS_MOTION e1000_unused_pose_motion = {
#include "shared/e1000_art/stance_motions/e1000_unused_pose_motion.inc"
};

static NJS_MKEY_F stand_body_position[6] = {
#include "shared/e1000_art/stance_motions/stand_body_position.inc"
};

static NJS_MKEY_A stand_body_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_body_rotation.inc"
};

static NJS_MKEY_F stand_housing_position[6] = {
#include "shared/e1000_art/stance_motions/stand_housing_position.inc"
};

static NJS_MKEY_A stand_housing_link_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_housing_link_rotation.inc"
};

static NJS_MKEY_A stand_high_thigh_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_high_thigh_rotation.inc"
};

static NJS_MKEY_A stand_high_shin_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_high_shin_rotation.inc"
};

static NJS_MKEY_A stand_high_foot_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_high_foot_rotation.inc"
};

static NJS_MKEY_A stand_gun_high_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_gun_high_rotation.inc"
};

static NJS_MKEY_A stand_barrel_high_rotation[5] = {
#include "shared/e1000_art/stance_motions/stand_barrel_high_rotation.inc"
};

static NJS_MKEY_A stand_low_thigh_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_low_thigh_rotation.inc"
};

static NJS_MKEY_A stand_low_shin_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_low_shin_rotation.inc"
};

static NJS_MKEY_A stand_low_foot_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_low_foot_rotation.inc"
};

static NJS_MKEY_A stand_gun_low_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_gun_low_rotation.inc"
};

static NJS_MKEY_A stand_barrel_low_rotation[6] = {
#include "shared/e1000_art/stance_motions/stand_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_stand_tracks[14] = {
#include "shared/e1000_art/stance_motions/e1000_stand_tracks.inc"
};

static NJS_MOTION e1000_stand_motion = {
#include "shared/e1000_art/stance_motions/e1000_stand_motion.inc"
};

#endif
