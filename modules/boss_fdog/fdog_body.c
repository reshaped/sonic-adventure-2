/* BOSS_FDOG.PRS's chunk models and motions, from the disc. its D lies
 * between fdog.c and object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_fdog/model_art.h"
#include "boss_fdog/motion_art.h"

static NJS_CNK_OBJECT muzzle = {
#include "boss_fdog/fdog_body/muzzle.inc"
};
static Sint16 cannon_polygons[609] = {
#include "boss_fdog/fdog_body/cannon_polygons.inc"
};
static Sint32 cannon_vertices[381] = {
#include "boss_fdog/fdog_body/cannon_vertices.inc"
};
static NJS_CNK_MODEL cannon_mesh = {
#include "boss_fdog/fdog_body/cannon_mesh.inc"
};
NJS_CNK_OBJECT fdog_gun_node = {
#include "boss_fdog/fdog_body/fdog_gun_node.inc"
};
static Sint16 pod_b_polygons[275] = {
#include "boss_fdog/fdog_body/pod_b_polygons.inc"
};
static Sint32 pod_b_vertices[147] = {
#include "boss_fdog/fdog_body/pod_b_vertices.inc"
};
static NJS_CNK_MODEL pod_b_mesh = {
#include "boss_fdog/fdog_body/pod_b_mesh.inc"
};
static NJS_CNK_OBJECT pod_b = {
#include "boss_fdog/fdog_body/pod_b.inc"
};
static Sint16 pod_a_polygons[275] = {
#include "boss_fdog/fdog_body/pod_a_polygons.inc"
};
static Sint32 pod_a_vertices[147] = {
#include "boss_fdog/fdog_body/pod_a_vertices.inc"
};
static NJS_CNK_MODEL pod_a_mesh = {
#include "boss_fdog/fdog_body/pod_a_mesh.inc"
};
static NJS_CNK_OBJECT pod_a = {
#include "boss_fdog/fdog_body/pod_a.inc"
};
NJS_CNK_OBJECT fdog_laser_node = {
#include "boss_fdog/fdog_body/fdog_laser_node.inc"
};
static Sint16 turret_polygons[203] = {
#include "boss_fdog/fdog_body/turret_polygons.inc"
};
static Sint32 turret_vertices[147] = {
#include "boss_fdog/fdog_body/turret_vertices.inc"
};
static NJS_CNK_MODEL turret_mesh = {
#include "boss_fdog/fdog_body/turret_mesh.inc"
};
NJS_CNK_OBJECT fdog_turret_node = {
#include "boss_fdog/fdog_body/fdog_turret_node.inc"
};
static Sint16 body_polygons[5675] = {
#include "boss_fdog/fdog_body/body_polygons.inc"
};
static Sint32 body_vertices[4137] = {
#include "boss_fdog/fdog_body/body_vertices.inc"
};
static NJS_CNK_MODEL body_mesh = {
#include "boss_fdog/fdog_body/body_mesh.inc"
};
NJS_CNK_OBJECT fdog_body_node = {
#include "boss_fdog/fdog_body/fdog_body_node.inc"
};
NJS_CNK_OBJECT fdog_object = {
#include "boss_fdog/fdog_body/fdog_object.inc"
};
static Sint16 jet_marker_polygons[21] = {
#include "boss_fdog/fdog_body/jet_marker_polygons.inc"
};
Sint32 fdog_jet_marker_vertices[39] = {
#include "boss_fdog/fdog_body/fdog_jet_marker_vertices.inc"
};
static NJS_CNK_MODEL jet_marker_mesh = {
#include "boss_fdog/fdog_body/jet_marker_mesh.inc"
};
static NJS_CNK_OBJECT jet_marker = {
#include "boss_fdog/fdog_body/jet_marker.inc"
};
static NJS_MKEY_F fdog_hit_motion_joint_0_position[16] = {
#include "boss_fdog/fdog_body/fdog_hit_motion_joint_0_position.inc"
};
static NJS_MKEY_A fdog_hit_motion_joint_0_angles[16] = {
#include "boss_fdog/fdog_body/fdog_hit_motion_joint_0_angles.inc"
};
static NJS_MDATA2 fdog_hit_motion_tracks[8] = {
#include "boss_fdog/fdog_body/fdog_hit_motion_tracks.inc"
};
NJS_MOTION fdog_hit_motion = {
#include "boss_fdog/fdog_body/fdog_hit_motion.inc"
};
static NJS_MKEY_A fdog_open_motion_joint_0_angles[16] = {
#include "boss_fdog/fdog_body/fdog_open_motion_joint_0_angles.inc"
};
static NJS_MKEY_F fdog_open_motion_joint_1_position[2] = {
#include "boss_fdog/fdog_body/fdog_open_motion_joint_1_position.inc"
};
static NJS_MKEY_A fdog_open_motion_joint_4_angles[2] = {
#include "boss_fdog/fdog_body/fdog_open_motion_joint_4_angles.inc"
};
static NJS_MKEY_A fdog_open_motion_joint_5_angles[2] = {
#include "boss_fdog/fdog_body/fdog_open_motion_joint_5_angles.inc"
};
static NJS_MDATA2 fdog_open_motion_tracks[8] = {
#include "boss_fdog/fdog_body/fdog_open_motion_tracks.inc"
};
NJS_MOTION fdog_open_motion = {
#include "boss_fdog/fdog_body/fdog_open_motion.inc"
};
