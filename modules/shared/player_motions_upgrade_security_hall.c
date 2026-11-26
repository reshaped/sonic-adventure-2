/* the motions and their registry of STG08.PRS and STG32.PRS, from the disc.
 * its D's neighbours differ by image */
#include <ninja.h>

#include "player/player.h"
#include "shared/motions.h"

static NJS_MKEY_F upgrade_security_hall_motion_joint_2_position[2] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_2_position.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_4_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_7_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_8_angles[3] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_9_angles[12] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_13_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_17_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_18_angles[2] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_19_angles[11] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_23_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_24_angles[15] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_24_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_27_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_28_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_29_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_31_angles[2] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_31_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_32_angles[16] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_33_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_37_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_38_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_39_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_41_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_44_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_51_angles[16] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_53_angles[9] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_56_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_57_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_58_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_security_hall_motion_joint_60_angles[17] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_security_hall_motion_tracks[62] = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion_tracks.inc"
};
static NJS_MOTION upgrade_security_hall_motion = {
#include "shared/player_motions_upgrade_security_hall/upgrade_security_hall_motion.inc"
};
struct motion_entry module_motions[2] = {
#include "shared/player_motions_upgrade_security_hall/module_motions.inc"
};
