/* the motions and actions of BOSS_FDOG.PRS and BOSS_HOTSHOT.PRS, from the
 * disc. its D lies between sight.c and mech_boss_sight.c */
#include <ninja.h>

#include "shared/gun_mech/effect_art.h"

static NJS_MKEY_F mech_boss_sight_ring_motion_joint_1_scale[3] = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_ring_motion_joint_1_scale.inc"
};
static NJS_MDATA3 mech_boss_sight_ring_motion_tracks[2] = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_ring_motion_tracks.inc"
};
NJS_MOTION mech_boss_sight_ring_motion = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_ring_motion.inc"
};
static NJS_ACTION sight_ring_action = {
#include "shared/gun_mech_ring_motion/sight_ring_action.inc"
};
static NJS_MKEY_A mech_boss_sight_motion_joint_1_angles[3] = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_motion_joint_1_angles.inc"
};
static NJS_MKEY_A mech_boss_sight_motion_joint_2_angles[3] = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_motion_joint_2_angles.inc"
};
static NJS_MKEY_A mech_boss_sight_motion_joint_3_angles[3] = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_motion_joint_3_angles.inc"
};
static NJS_MKEY_F mech_boss_sight_motion_joint_4_scale[3] = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_motion_joint_4_scale.inc"
};
static NJS_MDATA3 mech_boss_sight_motion_tracks[5] = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_motion_tracks.inc"
};
NJS_MOTION mech_boss_sight_motion = {
#include "shared/gun_mech_ring_motion/mech_boss_sight_motion.inc"
};
static NJS_ACTION sight_action = {
#include "shared/gun_mech_ring_motion/sight_action.inc"
};
