/* the motions and actions that 3 images place, from the disc. its D lies
 * between fragments.c and charge.c */
#include <ninja.h>

static NJS_MKEY_F fragment_motion_joint_1_position[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_1_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_1_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_1_angles.inc"
};
static NJS_MKEY_F fragment_motion_joint_2_position[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_2_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_2_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_2_angles.inc"
};
static NJS_MKEY_F fragment_motion_joint_3_position[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_3_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_3_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_3_angles.inc"
};
static NJS_MKEY_F fragment_motion_joint_4_position[8] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_4_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_4_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_4_angles.inc"
};
static NJS_MKEY_F fragment_motion_joint_5_position[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_5_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_5_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_5_angles.inc"
};
static NJS_MKEY_F fragment_motion_joint_6_position[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_6_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_6_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_6_angles.inc"
};
static NJS_MKEY_F fragment_motion_joint_7_position[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_7_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_7_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_7_angles.inc"
};
static NJS_MKEY_F fragment_motion_joint_8_position[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_8_position.inc"
};
static NJS_MKEY_A fragment_motion_joint_8_angles[11] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_joint_8_angles.inc"
};
static NJS_MDATA2 fragment_motion_tracks[9] = {
#include "shared/gun_mech_fragment_motion/fragment_motion_tracks.inc"
};
static NJS_MOTION fragment_motion = {
#include "shared/gun_mech_fragment_motion/fragment_motion.inc"
};
static NJS_ACTION fragment_action = {
#include "shared/gun_mech_fragment_motion/fragment_action.inc"
};
