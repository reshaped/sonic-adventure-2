/* BOSS_GOLEM.PRS's motions and their registry, from the disc. its D lies
 * between object_kinds.c and boss.c */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F sonic_sink_motion_joint_2_position[2] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_2_position.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_4_angles[62] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_4_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_7_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_7_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_8_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_8_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_9_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_9_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_13_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_13_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_14_angles[47] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_14_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_17_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_17_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_18_angles[58] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_18_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_19_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_19_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_23_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_23_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_24_angles[54] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_24_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_27_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_27_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_28_angles[62] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_28_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_29_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_29_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_31_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_31_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_32_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_32_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_33_angles[41] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_33_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_37_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_37_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_38_angles[57] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_38_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_39_angles[58] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_39_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_41_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_41_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_44_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_44_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_51_angles[2] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_51_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_53_angles[2] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_53_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_56_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_56_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_57_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_57_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_58_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_58_angles.inc"
};
static NJS_MKEY_A sonic_sink_motion_joint_60_angles[63] = {
#include "boss_golem/player_motions/sonic_sink_motion_joint_60_angles.inc"
};
static NJS_MDATA2 sonic_sink_motion_tracks[62] = {
#include "boss_golem/player_motions/sonic_sink_motion_tracks.inc"
};
static NJS_MOTION sonic_sink_motion = {
#include "boss_golem/player_motions/sonic_sink_motion.inc"
};
struct motion_entry module_motions[2] = {
#include "boss_golem/player_motions/module_motions.inc"
};
