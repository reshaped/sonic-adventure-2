/* CHAOSTGKINDER.PRS's motions, from the disc. its D lies between
 * door_models.c and extra_motion_a.c */
#include <ninja.h>

#include "chaostgkinder/character_art.h"
#include "chaostgkinder/door_art.h"

static NJS_MKEY_A kinder_classroom_door_motion_joint_1_angles[8] = {
#include "chaostgkinder/door_motions/kinder_classroom_door_motion_joint_1_angles.inc"
};
static NJS_MDATA2 kinder_classroom_door_motion_tracks[2] = {
#include "chaostgkinder/door_motions/kinder_classroom_door_motion_tracks.inc"
};
NJS_MOTION kinder_classroom_door_motion = {
#include "chaostgkinder/door_motions/kinder_classroom_door_motion.inc"
};
static NJS_MKEY_A kinder_corridor_door_motion_joint_1_angles[8] = {
#include "chaostgkinder/door_motions/kinder_corridor_door_motion_joint_1_angles.inc"
};
static NJS_MKEY_A kinder_corridor_door_motion_joint_2_angles[8] = {
#include "chaostgkinder/door_motions/kinder_corridor_door_motion_joint_2_angles.inc"
};
static NJS_MDATA2 kinder_corridor_door_motion_tracks[3] = {
#include "chaostgkinder/door_motions/kinder_corridor_door_motion_tracks.inc"
};
NJS_MOTION kinder_corridor_door_motion = {
#include "chaostgkinder/door_motions/kinder_corridor_door_motion.inc"
};
static NJS_MKEY_A kinder_doctor_door_motion_joint_1_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_door_motion_joint_1_angles.inc"
};
static NJS_MDATA2 kinder_doctor_door_motion_tracks[2] = {
#include "chaostgkinder/door_motions/kinder_doctor_door_motion_tracks.inc"
};
NJS_MOTION kinder_doctor_door_motion = {
#include "chaostgkinder/door_motions/kinder_doctor_door_motion.inc"
};
static NJS_MKEY_A kinder_extra_door_motion_joint_1_angles[8] = {
#include "chaostgkinder/door_motions/kinder_extra_door_motion_joint_1_angles.inc"
};
static NJS_MDATA2 kinder_extra_door_motion_tracks[2] = {
#include "chaostgkinder/door_motions/kinder_extra_door_motion_tracks.inc"
};
NJS_MOTION kinder_extra_door_motion = {
#include "chaostgkinder/door_motions/kinder_extra_door_motion.inc"
};
static NJS_MKEY_A kinder_principal_door_motion_joint_1_angles[8] = {
#include "chaostgkinder/door_motions/kinder_principal_door_motion_joint_1_angles.inc"
};
static NJS_MDATA2 kinder_principal_door_motion_tracks[2] = {
#include "chaostgkinder/door_motions/kinder_principal_door_motion_tracks.inc"
};
NJS_MOTION kinder_principal_door_motion = {
#include "chaostgkinder/door_motions/kinder_principal_door_motion.inc"
};
static NJS_MKEY_F kinder_doctor_motion_b_joint_1_position[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_1_position.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_1_angles[9] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_1_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_2_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_2_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_3_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_3_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_4_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_4_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_5_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_5_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_6_angles[15] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_6_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_7_angles[15] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_7_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_8_angles[9] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_8_angles.inc"
};
static NJS_MKEY_F kinder_doctor_motion_b_joint_9_position[9] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_9_position.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_9_angles[15] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_9_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_10_angles[9] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_10_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_b_joint_11_angles[9] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_11_angles.inc"
};
static NJS_MKEY_F kinder_doctor_motion_b_joint_19_position[9] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_joint_19_position.inc"
};
static NJS_MDATA2 kinder_doctor_motion_b_tracks[20] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b_tracks.inc"
};
NJS_MOTION kinder_doctor_motion_b = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_b.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_0_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_0_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_1_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_1_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_2_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_2_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_3_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_3_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_4_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_4_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_5_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_5_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_6_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_6_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_7_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_7_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_8_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_8_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_10_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_10_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_11_angles[16] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_11_angles.inc"
};
static NJS_MKEY_F kinder_doctor_motion_c_joint_19_position[10] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_19_position.inc"
};
static NJS_MKEY_A kinder_doctor_motion_c_joint_19_angles[2] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_joint_19_angles.inc"
};
static NJS_MDATA2 kinder_doctor_motion_c_tracks[20] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c_tracks.inc"
};
NJS_MOTION kinder_doctor_motion_c = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_c.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_1_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_1_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_2_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_2_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_3_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_3_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_4_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_4_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_5_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_5_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_6_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_6_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_7_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_7_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_8_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_8_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_10_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_10_angles.inc"
};
static NJS_MKEY_A kinder_doctor_motion_a_joint_11_angles[8] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_11_angles.inc"
};
static NJS_MKEY_F kinder_doctor_motion_a_joint_19_position[6] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_joint_19_position.inc"
};
static NJS_MDATA2 kinder_doctor_motion_a_tracks[20] = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a_tracks.inc"
};
NJS_MOTION kinder_doctor_motion_a = {
#include "chaostgkinder/door_motions/kinder_doctor_motion_a.inc"
};
static NJS_MKEY_A kinder_house_door_motion_joint_1_angles[16] = {
#include "chaostgkinder/door_motions/kinder_house_door_motion_joint_1_angles.inc"
};
static NJS_MKEY_A kinder_house_door_motion_joint_2_angles[16] = {
#include "chaostgkinder/door_motions/kinder_house_door_motion_joint_2_angles.inc"
};
static NJS_MDATA2 kinder_house_door_motion_tracks[3] = {
#include "chaostgkinder/door_motions/kinder_house_door_motion_tracks.inc"
};
NJS_MOTION kinder_house_door_motion = {
#include "chaostgkinder/door_motions/kinder_house_door_motion.inc"
};
