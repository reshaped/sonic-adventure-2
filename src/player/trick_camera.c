/* 1ST_READ.BIN's camera motions, from the disc. its D lies between
 * title_card.c and sonic.c */
#include <ninja.h>

#include "player/trick_camera.h"

static NJS_MKEY_F soap_grade_a_camera_joint_0_position[36] = {
#include "player/trick_camera/soap_grade_a_camera_joint_0_position.inc"
};
static NJS_MKEY_F soap_grade_a_camera_joint_0_target[2] = {
#include "player/trick_camera/soap_grade_a_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 soap_grade_a_camera_joint_0_roll[2] = {
#include "player/trick_camera/soap_grade_a_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 soap_grade_a_camera_joint_0_fov[2] = {
#include "player/trick_camera/soap_grade_a_camera_joint_0_fov.inc"
};
static NJS_MDATA4 soap_grade_a_camera_tracks[1] = {
#include "player/trick_camera/soap_grade_a_camera_tracks.inc"
};
NJS_MOTION soap_grade_a_camera[1] = {
#include "player/trick_camera/soap_grade_a_camera.inc"
};
static NJS_MKEY_F soap_grade_b_camera_joint_0_position[28] = {
#include "player/trick_camera/soap_grade_b_camera_joint_0_position.inc"
};
static NJS_MKEY_F soap_grade_b_camera_joint_0_target[21] = {
#include "player/trick_camera/soap_grade_b_camera_joint_0_target.inc"
};
static NJS_MKEY_A1 soap_grade_b_camera_joint_0_roll[2] = {
#include "player/trick_camera/soap_grade_b_camera_joint_0_roll.inc"
};
static NJS_MKEY_A1 soap_grade_b_camera_joint_0_fov[2] = {
#include "player/trick_camera/soap_grade_b_camera_joint_0_fov.inc"
};
static NJS_MDATA4 soap_grade_b_camera_tracks[1] = {
#include "player/trick_camera/soap_grade_b_camera_tracks.inc"
};
NJS_MOTION soap_grade_b_camera = {
#include "player/trick_camera/soap_grade_b_camera.inc"
};
