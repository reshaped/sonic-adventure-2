/* 1ST_READ.BIN's camera motions, from the disc. its D lies between player.c
 * and mystic_melody.c */
#include <ninja.h>

#include "player/pose_camera.h"

static NJS_MKEY_F eggman_mech_pose_camera_joint_0_position[33] = {
#include "player/pose_camera/eggman_mech_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F eggman_mech_pose_camera_joint_0_target[13] = {
#include "player/pose_camera/eggman_mech_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 eggman_mech_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/eggman_mech_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 eggman_mech_pose_camera_joint_0_fov[2] = {
#include "player/pose_camera/eggman_mech_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 eggman_mech_pose_camera_tracks[1] = {
#include "player/pose_camera/eggman_mech_pose_camera_tracks.inc"
};
NJS_MOTION eggman_mech_pose_camera = {
#include "player/pose_camera/eggman_mech_pose_camera.inc"
};
static NJS_MKEY_F knuckles_pose_camera_joint_0_position[33] = {
#include "player/pose_camera/knuckles_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F knuckles_pose_camera_joint_0_target[2] = {
#include "player/pose_camera/knuckles_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 knuckles_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/knuckles_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 knuckles_pose_camera_joint_0_fov[2] = {
#include "player/pose_camera/knuckles_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 knuckles_pose_camera_tracks[1] = {
#include "player/pose_camera/knuckles_pose_camera_tracks.inc"
};
NJS_MOTION knuckles_pose_camera = {
#include "player/pose_camera/knuckles_pose_camera.inc"
};
static NJS_MKEY_F rouge_pose_camera_joint_0_position[25] = {
#include "player/pose_camera/rouge_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F rouge_pose_camera_joint_0_target[2] = {
#include "player/pose_camera/rouge_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 rouge_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/rouge_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 rouge_pose_camera_joint_0_fov[2] = {
#include "player/pose_camera/rouge_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 rouge_pose_camera_tracks[1] = {
#include "player/pose_camera/rouge_pose_camera_tracks.inc"
};
NJS_MOTION rouge_pose_camera = {
#include "player/pose_camera/rouge_pose_camera.inc"
};
static NJS_MKEY_F shadow_pose_camera_joint_0_position[49] = {
#include "player/pose_camera/shadow_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F shadow_pose_camera_joint_0_target[2] = {
#include "player/pose_camera/shadow_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 shadow_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/shadow_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 shadow_pose_camera_joint_0_fov[2] = {
#include "player/pose_camera/shadow_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 shadow_pose_camera_tracks[1] = {
#include "player/pose_camera/shadow_pose_camera_tracks.inc"
};
NJS_MOTION shadow_pose_camera = {
#include "player/pose_camera/shadow_pose_camera.inc"
};
static NJS_MKEY_F sonic_pose_camera_joint_0_position[37] = {
#include "player/pose_camera/sonic_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F sonic_pose_camera_joint_0_target[22] = {
#include "player/pose_camera/sonic_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 sonic_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/sonic_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 sonic_pose_camera_joint_0_fov[22] = {
#include "player/pose_camera/sonic_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 sonic_pose_camera_tracks[1] = {
#include "player/pose_camera/sonic_pose_camera_tracks.inc"
};
NJS_MOTION sonic_pose_camera = {
#include "player/pose_camera/sonic_pose_camera.inc"
};
static NJS_MKEY_F tails_mech_pose_camera_joint_0_position[25] = {
#include "player/pose_camera/tails_mech_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F tails_mech_pose_camera_joint_0_target[2] = {
#include "player/pose_camera/tails_mech_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 tails_mech_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/tails_mech_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 tails_mech_pose_camera_joint_0_fov[2] = {
#include "player/pose_camera/tails_mech_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 tails_mech_pose_camera_tracks[1] = {
#include "player/pose_camera/tails_mech_pose_camera_tracks.inc"
};
NJS_MOTION tails_mech_pose_camera = {
#include "player/pose_camera/tails_mech_pose_camera.inc"
};
static NJS_MKEY_F amy_pose_camera_joint_0_position[64] = {
#include "player/pose_camera/amy_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F amy_pose_camera_joint_0_target[62] = {
#include "player/pose_camera/amy_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 amy_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/amy_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 amy_pose_camera_joint_0_fov[2] = {
#include "player/pose_camera/amy_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 amy_pose_camera_tracks[1] = {
#include "player/pose_camera/amy_pose_camera_tracks.inc"
};
NJS_MOTION amy_pose_camera = {
#include "player/pose_camera/amy_pose_camera.inc"
};
static NJS_MKEY_F tikal_pose_camera_joint_0_position[120] = {
#include "player/pose_camera/tikal_pose_camera_joint_0_position.inc"
};
static NJS_MKEY_F tikal_pose_camera_joint_0_target[2] = {
#include "player/pose_camera/tikal_pose_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 tikal_pose_camera_joint_0_roll[2] = {
#include "player/pose_camera/tikal_pose_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 tikal_pose_camera_joint_0_fov[2] = {
#include "player/pose_camera/tikal_pose_camera_joint_0_fov.inc"
};
static NJS_MDATA4 tikal_pose_camera_tracks[1] = {
#include "player/pose_camera/tikal_pose_camera_tracks.inc"
};
NJS_MOTION tikal_pose_camera = {
#include "player/pose_camera/tikal_pose_camera.inc"
};
static NJS_MKEY_F eggman_mech_versus_camera_joint_0_position[120] = {
#include "player/pose_camera/eggman_mech_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F eggman_mech_versus_camera_joint_0_target[119] = {
#include "player/pose_camera/eggman_mech_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 eggman_mech_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/eggman_mech_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 eggman_mech_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/eggman_mech_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 eggman_mech_versus_camera_tracks[1] = {
#include "player/pose_camera/eggman_mech_versus_camera_tracks.inc"
};
NJS_MOTION eggman_mech_versus_camera = {
#include "player/pose_camera/eggman_mech_versus_camera.inc"
};
static NJS_MKEY_F knuckles_versus_camera_joint_0_position[117] = {
#include "player/pose_camera/knuckles_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F knuckles_versus_camera_joint_0_target[2] = {
#include "player/pose_camera/knuckles_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 knuckles_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/knuckles_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 knuckles_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/knuckles_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 knuckles_versus_camera_tracks[1] = {
#include "player/pose_camera/knuckles_versus_camera_tracks.inc"
};
NJS_MOTION knuckles_versus_camera = {
#include "player/pose_camera/knuckles_versus_camera.inc"
};
static NJS_MKEY_F rouge_versus_camera_joint_0_position[120] = {
#include "player/pose_camera/rouge_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F rouge_versus_camera_joint_0_target[2] = {
#include "player/pose_camera/rouge_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 rouge_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/rouge_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 rouge_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/rouge_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 rouge_versus_camera_tracks[1] = {
#include "player/pose_camera/rouge_versus_camera_tracks.inc"
};
NJS_MOTION rouge_versus_camera = {
#include "player/pose_camera/rouge_versus_camera.inc"
};
static NJS_MKEY_F shadow_versus_camera_joint_0_position[120] = {
#include "player/pose_camera/shadow_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F shadow_versus_camera_joint_0_target[2] = {
#include "player/pose_camera/shadow_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 shadow_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/shadow_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 shadow_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/shadow_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 shadow_versus_camera_tracks[1] = {
#include "player/pose_camera/shadow_versus_camera_tracks.inc"
};
NJS_MOTION shadow_versus_camera = {
#include "player/pose_camera/shadow_versus_camera.inc"
};
static NJS_MKEY_F sonic_versus_camera_joint_0_position[120] = {
#include "player/pose_camera/sonic_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F sonic_versus_camera_joint_0_target[2] = {
#include "player/pose_camera/sonic_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 sonic_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/sonic_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 sonic_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/sonic_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 sonic_versus_camera_tracks[1] = {
#include "player/pose_camera/sonic_versus_camera_tracks.inc"
};
NJS_MOTION sonic_versus_camera = {
#include "player/pose_camera/sonic_versus_camera.inc"
};
static NJS_MKEY_F tails_mech_versus_camera_joint_0_position[110] = {
#include "player/pose_camera/tails_mech_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F tails_mech_versus_camera_joint_0_target[105] = {
#include "player/pose_camera/tails_mech_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 tails_mech_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/tails_mech_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 tails_mech_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/tails_mech_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 tails_mech_versus_camera_tracks[1] = {
#include "player/pose_camera/tails_mech_versus_camera_tracks.inc"
};
NJS_MOTION tails_mech_versus_camera = {
#include "player/pose_camera/tails_mech_versus_camera.inc"
};
static NJS_MKEY_F metal_sonic_versus_camera_joint_0_position[97] = {
#include "player/pose_camera/metal_sonic_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F metal_sonic_versus_camera_joint_0_target[79] = {
#include "player/pose_camera/metal_sonic_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 metal_sonic_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/metal_sonic_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 metal_sonic_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/metal_sonic_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 metal_sonic_versus_camera_tracks[1] = {
#include "player/pose_camera/metal_sonic_versus_camera_tracks.inc"
};
NJS_MOTION metal_sonic_versus_camera = {
#include "player/pose_camera/metal_sonic_versus_camera.inc"
};
static NJS_MKEY_F amy_versus_camera_joint_0_position[2] = {
#include "player/pose_camera/amy_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F amy_versus_camera_joint_0_target[2] = {
#include "player/pose_camera/amy_versus_camera_joint_0_target.inc"
};
static NJS_MDATA2 amy_versus_camera_tracks[1] = {
#include "player/pose_camera/amy_versus_camera_tracks.inc"
};
NJS_MOTION amy_versus_camera = {
#include "player/pose_camera/amy_versus_camera.inc"
};
static NJS_MKEY_F chao_walker_versus_camera_joint_0_position[41] = {
#include "player/pose_camera/chao_walker_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F chao_walker_versus_camera_joint_0_target[2] = {
#include "player/pose_camera/chao_walker_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 chao_walker_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/chao_walker_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 chao_walker_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/chao_walker_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 chao_walker_versus_camera_tracks[1] = {
#include "player/pose_camera/chao_walker_versus_camera_tracks.inc"
};
NJS_MOTION chao_walker_versus_camera = {
#include "player/pose_camera/chao_walker_versus_camera.inc"
};
static NJS_MKEY_F tikal_versus_camera_joint_0_position[120] = {
#include "player/pose_camera/tikal_versus_camera_joint_0_position.inc"
};
static NJS_MKEY_F tikal_versus_camera_joint_0_target[118] = {
#include "player/pose_camera/tikal_versus_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 tikal_versus_camera_joint_0_roll[2] = {
#include "player/pose_camera/tikal_versus_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 tikal_versus_camera_joint_0_fov[2] = {
#include "player/pose_camera/tikal_versus_camera_joint_0_fov.inc"
};
static NJS_MDATA4 tikal_versus_camera_tracks[1] = {
#include "player/pose_camera/tikal_versus_camera_tracks.inc"
};
NJS_MOTION tikal_versus_camera = {
#include "player/pose_camera/tikal_versus_camera.inc"
};
