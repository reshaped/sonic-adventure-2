/* 1ST_READ.BIN's camera motions, from the disc. its D lies between enemy.c
 * and stage.c */
#include <ninja.h>

#include "stage/winner_camera.h"

static NJS_MKEY_F sonic_winner_camera_joint_0_position[80] = {
#include "stage/winner_camera/sonic_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F sonic_winner_camera_joint_0_target[68] = {
#include "stage/winner_camera/sonic_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 sonic_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/sonic_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 sonic_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/sonic_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 sonic_winner_camera_tracks[1] = {
#include "stage/winner_camera/sonic_winner_camera_tracks.inc"
};
NJS_MOTION sonic_winner_camera = {
#include "stage/winner_camera/sonic_winner_camera.inc"
};
static NJS_MKEY_F shadow_winner_camera_joint_0_position[71] = {
#include "stage/winner_camera/shadow_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F shadow_winner_camera_joint_0_target[37] = {
#include "stage/winner_camera/shadow_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 shadow_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/shadow_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 shadow_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/shadow_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 shadow_winner_camera_tracks[1] = {
#include "stage/winner_camera/shadow_winner_camera_tracks.inc"
};
NJS_MOTION shadow_winner_camera = {
#include "stage/winner_camera/shadow_winner_camera.inc"
};
static NJS_MKEY_F knuckles_winner_camera_joint_0_position[80] = {
#include "stage/winner_camera/knuckles_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F knuckles_winner_camera_joint_0_target[2] = {
#include "stage/winner_camera/knuckles_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 knuckles_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/knuckles_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 knuckles_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/knuckles_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 knuckles_winner_camera_tracks[1] = {
#include "stage/winner_camera/knuckles_winner_camera_tracks.inc"
};
NJS_MOTION knuckles_winner_camera = {
#include "stage/winner_camera/knuckles_winner_camera.inc"
};
static NJS_MKEY_F eggman_mech_winner_camera_joint_0_position[80] = {
#include "stage/winner_camera/eggman_mech_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F eggman_mech_winner_camera_joint_0_target[79] = {
#include "stage/winner_camera/eggman_mech_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 eggman_mech_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/eggman_mech_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 eggman_mech_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/eggman_mech_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 eggman_mech_winner_camera_tracks[1] = {
#include "stage/winner_camera/eggman_mech_winner_camera_tracks.inc"
};
NJS_MOTION eggman_mech_winner_camera = {
#include "stage/winner_camera/eggman_mech_winner_camera.inc"
};
static NJS_MKEY_F tails_mech_winner_camera_joint_0_position[80] = {
#include "stage/winner_camera/tails_mech_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F tails_mech_winner_camera_joint_0_target[2] = {
#include "stage/winner_camera/tails_mech_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 tails_mech_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/tails_mech_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 tails_mech_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/tails_mech_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 tails_mech_winner_camera_tracks[1] = {
#include "stage/winner_camera/tails_mech_winner_camera_tracks.inc"
};
NJS_MOTION tails_mech_winner_camera = {
#include "stage/winner_camera/tails_mech_winner_camera.inc"
};
static NJS_MKEY_F rouge_winner_camera_joint_0_position[139] = {
#include "stage/winner_camera/rouge_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F rouge_winner_camera_joint_0_target[33] = {
#include "stage/winner_camera/rouge_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 rouge_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/rouge_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 rouge_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/rouge_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 rouge_winner_camera_tracks[1] = {
#include "stage/winner_camera/rouge_winner_camera_tracks.inc"
};
NJS_MOTION rouge_winner_camera = {
#include "stage/winner_camera/rouge_winner_camera.inc"
};
static NJS_MKEY_F amy_winner_camera_joint_0_position[119] = {
#include "stage/winner_camera/amy_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F amy_winner_camera_joint_0_target[2] = {
#include "stage/winner_camera/amy_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 amy_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/amy_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 amy_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/amy_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 amy_winner_camera_tracks[1] = {
#include "stage/winner_camera/amy_winner_camera_tracks.inc"
};
NJS_MOTION amy_winner_camera = {
#include "stage/winner_camera/amy_winner_camera.inc"
};
static NJS_MKEY_F tikal_winner_camera_joint_0_position[120] = {
#include "stage/winner_camera/tikal_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F tikal_winner_camera_joint_0_target[119] = {
#include "stage/winner_camera/tikal_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 tikal_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/tikal_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 tikal_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/tikal_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 tikal_winner_camera_tracks[1] = {
#include "stage/winner_camera/tikal_winner_camera_tracks.inc"
};
NJS_MOTION tikal_winner_camera = {
#include "stage/winner_camera/tikal_winner_camera.inc"
};
static NJS_MKEY_F super_sonic_winner_camera_joint_0_position[49] = {
#include "stage/winner_camera/super_sonic_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F super_sonic_winner_camera_joint_0_target[52] = {
#include "stage/winner_camera/super_sonic_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 super_sonic_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/super_sonic_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 super_sonic_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/super_sonic_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 super_sonic_winner_camera_tracks[1] = {
#include "stage/winner_camera/super_sonic_winner_camera_tracks.inc"
};
NJS_MOTION super_sonic_winner_camera = {
#include "stage/winner_camera/super_sonic_winner_camera.inc"
};
static NJS_MKEY_F super_shadow_winner_camera_joint_0_position[49] = {
#include "stage/winner_camera/super_shadow_winner_camera_joint_0_position.inc"
};
static NJS_MKEY_F super_shadow_winner_camera_joint_0_target[43] = {
#include "stage/winner_camera/super_shadow_winner_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 super_shadow_winner_camera_joint_0_roll[2] = {
#include "stage/winner_camera/super_shadow_winner_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 super_shadow_winner_camera_joint_0_fov[2] = {
#include "stage/winner_camera/super_shadow_winner_camera_joint_0_fov.inc"
};
static NJS_MDATA4 super_shadow_winner_camera_tracks[1] = {
#include "stage/winner_camera/super_shadow_winner_camera_tracks.inc"
};
NJS_MOTION super_shadow_winner_camera = {
#include "stage/winner_camera/super_shadow_winner_camera.inc"
};
