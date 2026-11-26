/* the chunk models and motions that 26 images place, from the disc. its D
 * lies between enemy_ai_art.c and enemy_ai.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/enemy_ai_art.h"

static Sint16 ai_gun_right_foot_polygons[123] = {
#include "shared/enemy_ai_art_models/ai_gun_right_foot_polygons.inc"
};
static Sint32 ai_gun_right_foot_vertices[69] = {
#include "shared/enemy_ai_art_models/ai_gun_right_foot_vertices.inc"
};
NJS_CNK_MODEL ai_gun_right_foot_model = {
#include "shared/enemy_ai_art_models/ai_gun_right_foot_model.inc"
};
static NJS_CNK_OBJECT ai_gun_right_foot = {
#include "shared/enemy_ai_art_models/ai_gun_right_foot.inc"
};
static Sint16 ai_gun_right_shin_polygons[139] = {
#include "shared/enemy_ai_art_models/ai_gun_right_shin_polygons.inc"
};
static Sint32 ai_gun_right_shin_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_gun_right_shin_vertices.inc"
};
NJS_CNK_MODEL ai_gun_right_shin_model = {
#include "shared/enemy_ai_art_models/ai_gun_right_shin_model.inc"
};
static NJS_CNK_OBJECT ai_gun_right_shin = {
#include "shared/enemy_ai_art_models/ai_gun_right_shin.inc"
};
static Sint16 ai_gun_right_thigh_polygons[149] = {
#include "shared/enemy_ai_art_models/ai_gun_right_thigh_polygons.inc"
};
static Sint32 ai_gun_right_thigh_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_gun_right_thigh_vertices.inc"
};
NJS_CNK_MODEL ai_gun_right_thigh_model = {
#include "shared/enemy_ai_art_models/ai_gun_right_thigh_model.inc"
};
static NJS_CNK_OBJECT ai_gun_right_thigh = {
#include "shared/enemy_ai_art_models/ai_gun_right_thigh.inc"
};
static Sint16 ai_gun_right_hand_polygons[287] = {
#include "shared/enemy_ai_art_models/ai_gun_right_hand_polygons.inc"
};
static Sint32 ai_gun_right_hand_vertices[201] = {
#include "shared/enemy_ai_art_models/ai_gun_right_hand_vertices.inc"
};
NJS_CNK_MODEL ai_gun_right_hand_model = {
#include "shared/enemy_ai_art_models/ai_gun_right_hand_model.inc"
};
static NJS_CNK_OBJECT ai_gun_right_hand = {
#include "shared/enemy_ai_art_models/ai_gun_right_hand.inc"
};
static Sint16 ai_gun_right_forearm_polygons[93] = {
#include "shared/enemy_ai_art_models/ai_gun_right_forearm_polygons.inc"
};
static Sint32 ai_gun_right_forearm_vertices[87] = {
#include "shared/enemy_ai_art_models/ai_gun_right_forearm_vertices.inc"
};
NJS_CNK_MODEL ai_gun_right_forearm_model = {
#include "shared/enemy_ai_art_models/ai_gun_right_forearm_model.inc"
};
static NJS_CNK_OBJECT ai_gun_right_forearm = {
#include "shared/enemy_ai_art_models/ai_gun_right_forearm.inc"
};
static Sint16 ai_gun_right_upper_arm_polygons[141] = {
#include "shared/enemy_ai_art_models/ai_gun_right_upper_arm_polygons.inc"
};
static Sint32 ai_gun_right_upper_arm_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_gun_right_upper_arm_vertices.inc"
};
NJS_CNK_MODEL ai_gun_right_upper_arm_model = {
#include "shared/enemy_ai_art_models/ai_gun_right_upper_arm_model.inc"
};
static NJS_CNK_OBJECT ai_gun_right_upper_arm = {
#include "shared/enemy_ai_art_models/ai_gun_right_upper_arm.inc"
};
static Sint16 ai_gun_left_foot_polygons[123] = {
#include "shared/enemy_ai_art_models/ai_gun_left_foot_polygons.inc"
};
static Sint32 ai_gun_left_foot_vertices[69] = {
#include "shared/enemy_ai_art_models/ai_gun_left_foot_vertices.inc"
};
NJS_CNK_MODEL ai_gun_left_foot_model = {
#include "shared/enemy_ai_art_models/ai_gun_left_foot_model.inc"
};
static NJS_CNK_OBJECT ai_gun_left_foot = {
#include "shared/enemy_ai_art_models/ai_gun_left_foot.inc"
};
static Sint16 ai_gun_left_shin_polygons[139] = {
#include "shared/enemy_ai_art_models/ai_gun_left_shin_polygons.inc"
};
static Sint32 ai_gun_left_shin_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_gun_left_shin_vertices.inc"
};
NJS_CNK_MODEL ai_gun_left_shin_model = {
#include "shared/enemy_ai_art_models/ai_gun_left_shin_model.inc"
};
static NJS_CNK_OBJECT ai_gun_left_shin = {
#include "shared/enemy_ai_art_models/ai_gun_left_shin.inc"
};
static Sint16 ai_gun_left_thigh_polygons[149] = {
#include "shared/enemy_ai_art_models/ai_gun_left_thigh_polygons.inc"
};
static Sint32 ai_gun_left_thigh_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_gun_left_thigh_vertices.inc"
};
NJS_CNK_MODEL ai_gun_left_thigh_model = {
#include "shared/enemy_ai_art_models/ai_gun_left_thigh_model.inc"
};
static NJS_CNK_OBJECT ai_gun_left_thigh = {
#include "shared/enemy_ai_art_models/ai_gun_left_thigh.inc"
};
static Sint16 ai_gun_left_forearm_polygons[245] = {
#include "shared/enemy_ai_art_models/ai_gun_left_forearm_polygons.inc"
};
static Sint32 ai_gun_left_forearm_vertices[237] = {
#include "shared/enemy_ai_art_models/ai_gun_left_forearm_vertices.inc"
};
NJS_CNK_MODEL ai_gun_left_forearm_model = {
#include "shared/enemy_ai_art_models/ai_gun_left_forearm_model.inc"
};
static NJS_CNK_OBJECT ai_gun_left_forearm = {
#include "shared/enemy_ai_art_models/ai_gun_left_forearm.inc"
};
static Sint16 ai_gun_left_upper_arm_polygons[135] = {
#include "shared/enemy_ai_art_models/ai_gun_left_upper_arm_polygons.inc"
};
static Sint32 ai_gun_left_upper_arm_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_gun_left_upper_arm_vertices.inc"
};
NJS_CNK_MODEL ai_gun_left_upper_arm_model = {
#include "shared/enemy_ai_art_models/ai_gun_left_upper_arm_model.inc"
};
static NJS_CNK_OBJECT ai_gun_left_upper_arm = {
#include "shared/enemy_ai_art_models/ai_gun_left_upper_arm.inc"
};
static Sint16 ai_gun_weapon_polygons[251] = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_polygons.inc"
};
static Sint32 ai_gun_weapon_vertices[189] = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_vertices.inc"
};
NJS_CNK_MODEL ai_gun_weapon_model = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_model.inc"
};
NJS_CNK_OBJECT ai_gun_weapon_object = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_object.inc"
};
static Sint16 ai_gun_weapon_mount_polygons[441] = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_mount_polygons.inc"
};
static Sint32 ai_gun_weapon_mount_vertices[279] = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_mount_vertices.inc"
};
NJS_CNK_MODEL ai_gun_weapon_mount_model = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_mount_model.inc"
};
static NJS_CNK_OBJECT ai_gun_weapon_mount = {
#include "shared/enemy_ai_art_models/ai_gun_weapon_mount.inc"
};
static Sint16 ai_gun_body_polygons[163] = {
#include "shared/enemy_ai_art_models/ai_gun_body_polygons.inc"
};
static Sint32 ai_gun_body_vertices[111] = {
#include "shared/enemy_ai_art_models/ai_gun_body_vertices.inc"
};
NJS_CNK_MODEL ai_gun_body_model = {
#include "shared/enemy_ai_art_models/ai_gun_body_model.inc"
};
NJS_CNK_OBJECT ai_gun_object[1] = {
#include "shared/enemy_ai_art_models/ai_gun_object.inc"
};
static Sint16 ai_laser_right_foot_polygons[123] = {
#include "shared/enemy_ai_art_models/ai_laser_right_foot_polygons.inc"
};
static Sint32 ai_laser_right_foot_vertices[69] = {
#include "shared/enemy_ai_art_models/ai_laser_right_foot_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_right_foot_model = {
#include "shared/enemy_ai_art_models/ai_laser_right_foot_model.inc"
};
static NJS_CNK_OBJECT ai_laser_right_foot = {
#include "shared/enemy_ai_art_models/ai_laser_right_foot.inc"
};
static Sint16 ai_laser_right_shin_polygons[139] = {
#include "shared/enemy_ai_art_models/ai_laser_right_shin_polygons.inc"
};
static Sint32 ai_laser_right_shin_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_laser_right_shin_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_right_shin_model = {
#include "shared/enemy_ai_art_models/ai_laser_right_shin_model.inc"
};
static NJS_CNK_OBJECT ai_laser_right_shin = {
#include "shared/enemy_ai_art_models/ai_laser_right_shin.inc"
};
static Sint16 ai_laser_right_thigh_polygons[149] = {
#include "shared/enemy_ai_art_models/ai_laser_right_thigh_polygons.inc"
};
static Sint32 ai_laser_right_thigh_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_laser_right_thigh_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_right_thigh_model = {
#include "shared/enemy_ai_art_models/ai_laser_right_thigh_model.inc"
};
static NJS_CNK_OBJECT ai_laser_right_thigh = {
#include "shared/enemy_ai_art_models/ai_laser_right_thigh.inc"
};
static Sint16 ai_laser_right_hand_polygons[155] = {
#include "shared/enemy_ai_art_models/ai_laser_right_hand_polygons.inc"
};
static Sint32 ai_laser_right_hand_vertices[129] = {
#include "shared/enemy_ai_art_models/ai_laser_right_hand_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_right_hand_model = {
#include "shared/enemy_ai_art_models/ai_laser_right_hand_model.inc"
};
static NJS_CNK_OBJECT ai_laser_right_hand = {
#include "shared/enemy_ai_art_models/ai_laser_right_hand.inc"
};
static Sint16 ai_laser_right_forearm_polygons[163] = {
#include "shared/enemy_ai_art_models/ai_laser_right_forearm_polygons.inc"
};
static Sint32 ai_laser_right_forearm_vertices[141] = {
#include "shared/enemy_ai_art_models/ai_laser_right_forearm_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_right_forearm_model = {
#include "shared/enemy_ai_art_models/ai_laser_right_forearm_model.inc"
};
static NJS_CNK_OBJECT ai_laser_right_forearm = {
#include "shared/enemy_ai_art_models/ai_laser_right_forearm.inc"
};
static Sint16 ai_laser_right_upper_arm_polygons[141] = {
#include "shared/enemy_ai_art_models/ai_laser_right_upper_arm_polygons.inc"
};
static Sint32 ai_laser_right_upper_arm_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_laser_right_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_right_upper_arm_model = {
#include "shared/enemy_ai_art_models/ai_laser_right_upper_arm_model.inc"
};
static NJS_CNK_OBJECT ai_laser_right_upper_arm = {
#include "shared/enemy_ai_art_models/ai_laser_right_upper_arm.inc"
};
static Sint16 ai_laser_left_foot_polygons[123] = {
#include "shared/enemy_ai_art_models/ai_laser_left_foot_polygons.inc"
};
static Sint32 ai_laser_left_foot_vertices[69] = {
#include "shared/enemy_ai_art_models/ai_laser_left_foot_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_left_foot_model = {
#include "shared/enemy_ai_art_models/ai_laser_left_foot_model.inc"
};
static NJS_CNK_OBJECT ai_laser_left_foot = {
#include "shared/enemy_ai_art_models/ai_laser_left_foot.inc"
};
static Sint16 ai_laser_left_shin_polygons[139] = {
#include "shared/enemy_ai_art_models/ai_laser_left_shin_polygons.inc"
};
static Sint32 ai_laser_left_shin_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_laser_left_shin_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_left_shin_model = {
#include "shared/enemy_ai_art_models/ai_laser_left_shin_model.inc"
};
static NJS_CNK_OBJECT ai_laser_left_shin = {
#include "shared/enemy_ai_art_models/ai_laser_left_shin.inc"
};
static Sint16 ai_laser_left_thigh_polygons[149] = {
#include "shared/enemy_ai_art_models/ai_laser_left_thigh_polygons.inc"
};
static Sint32 ai_laser_left_thigh_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_laser_left_thigh_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_left_thigh_model = {
#include "shared/enemy_ai_art_models/ai_laser_left_thigh_model.inc"
};
static NJS_CNK_OBJECT ai_laser_left_thigh = {
#include "shared/enemy_ai_art_models/ai_laser_left_thigh.inc"
};
static Sint16 ai_laser_left_forearm_polygons[245] = {
#include "shared/enemy_ai_art_models/ai_laser_left_forearm_polygons.inc"
};
static Sint32 ai_laser_left_forearm_vertices[237] = {
#include "shared/enemy_ai_art_models/ai_laser_left_forearm_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_left_forearm_model = {
#include "shared/enemy_ai_art_models/ai_laser_left_forearm_model.inc"
};
static NJS_CNK_OBJECT ai_laser_left_forearm = {
#include "shared/enemy_ai_art_models/ai_laser_left_forearm.inc"
};
static Sint16 ai_laser_left_upper_arm_polygons[135] = {
#include "shared/enemy_ai_art_models/ai_laser_left_upper_arm_polygons.inc"
};
static Sint32 ai_laser_left_upper_arm_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_laser_left_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_left_upper_arm_model = {
#include "shared/enemy_ai_art_models/ai_laser_left_upper_arm_model.inc"
};
static NJS_CNK_OBJECT ai_laser_left_upper_arm = {
#include "shared/enemy_ai_art_models/ai_laser_left_upper_arm.inc"
};
static Sint16 ai_laser_weapon_polygons[251] = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_polygons.inc"
};
static Sint32 ai_laser_weapon_vertices[189] = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_weapon_model = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_model.inc"
};
NJS_CNK_OBJECT ai_laser_weapon_object = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_object.inc"
};
static Sint16 ai_laser_weapon_mount_polygons[441] = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_mount_polygons.inc"
};
static Sint32 ai_laser_weapon_mount_vertices[279] = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_mount_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_weapon_mount_model = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_mount_model.inc"
};
static NJS_CNK_OBJECT ai_laser_weapon_mount = {
#include "shared/enemy_ai_art_models/ai_laser_weapon_mount.inc"
};
static Sint16 ai_laser_body_polygons[163] = {
#include "shared/enemy_ai_art_models/ai_laser_body_polygons.inc"
};
static Sint32 ai_laser_body_vertices[111] = {
#include "shared/enemy_ai_art_models/ai_laser_body_vertices.inc"
};
static NJS_CNK_MODEL ai_laser_body_model = {
#include "shared/enemy_ai_art_models/ai_laser_body_model.inc"
};
NJS_CNK_OBJECT ai_laser_object = {
#include "shared/enemy_ai_art_models/ai_laser_object.inc"
};
static Sint16 ai_shielder_right_foot_polygons[123] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_foot_polygons.inc"
};
static Sint32 ai_shielder_right_foot_vertices[69] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_foot_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_right_foot_model = {
#include "shared/enemy_ai_art_models/ai_shielder_right_foot_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_right_foot = {
#include "shared/enemy_ai_art_models/ai_shielder_right_foot.inc"
};
static Sint16 ai_shielder_right_shin_polygons[139] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_shin_polygons.inc"
};
static Sint32 ai_shielder_right_shin_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_shin_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_right_shin_model = {
#include "shared/enemy_ai_art_models/ai_shielder_right_shin_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_right_shin = {
#include "shared/enemy_ai_art_models/ai_shielder_right_shin.inc"
};
static Sint16 ai_shielder_right_thigh_polygons[149] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_thigh_polygons.inc"
};
static Sint32 ai_shielder_right_thigh_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_thigh_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_right_thigh_model = {
#include "shared/enemy_ai_art_models/ai_shielder_right_thigh_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_right_thigh = {
#include "shared/enemy_ai_art_models/ai_shielder_right_thigh.inc"
};
static Sint16 ai_shielder_right_hand_polygons[155] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_hand_polygons.inc"
};
static Sint32 ai_shielder_right_hand_vertices[129] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_hand_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_right_hand_model = {
#include "shared/enemy_ai_art_models/ai_shielder_right_hand_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_right_hand = {
#include "shared/enemy_ai_art_models/ai_shielder_right_hand.inc"
};
static Sint16 ai_shielder_right_forearm_polygons[163] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_forearm_polygons.inc"
};
static Sint32 ai_shielder_right_forearm_vertices[141] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_forearm_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_right_forearm_model = {
#include "shared/enemy_ai_art_models/ai_shielder_right_forearm_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_right_forearm = {
#include "shared/enemy_ai_art_models/ai_shielder_right_forearm.inc"
};
static Sint16 ai_shielder_right_upper_arm_polygons[141] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_upper_arm_polygons.inc"
};
static Sint32 ai_shielder_right_upper_arm_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_shielder_right_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_right_upper_arm_model = {
#include "shared/enemy_ai_art_models/ai_shielder_right_upper_arm_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_right_upper_arm = {
#include "shared/enemy_ai_art_models/ai_shielder_right_upper_arm.inc"
};
static Sint16 ai_shielder_left_foot_polygons[123] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_foot_polygons.inc"
};
static Sint32 ai_shielder_left_foot_vertices[69] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_foot_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_left_foot_model = {
#include "shared/enemy_ai_art_models/ai_shielder_left_foot_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_left_foot = {
#include "shared/enemy_ai_art_models/ai_shielder_left_foot.inc"
};
static Sint16 ai_shielder_left_shin_polygons[139] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_shin_polygons.inc"
};
static Sint32 ai_shielder_left_shin_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_shin_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_left_shin_model = {
#include "shared/enemy_ai_art_models/ai_shielder_left_shin_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_left_shin = {
#include "shared/enemy_ai_art_models/ai_shielder_left_shin.inc"
};
static Sint16 ai_shielder_left_thigh_polygons[149] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_thigh_polygons.inc"
};
static Sint32 ai_shielder_left_thigh_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_thigh_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_left_thigh_model = {
#include "shared/enemy_ai_art_models/ai_shielder_left_thigh_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_left_thigh = {
#include "shared/enemy_ai_art_models/ai_shielder_left_thigh.inc"
};
static Sint16 ai_shielder_left_forearm_polygons[135] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_forearm_polygons.inc"
};
static Sint32 ai_shielder_left_forearm_vertices[129] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_forearm_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_left_forearm_model = {
#include "shared/enemy_ai_art_models/ai_shielder_left_forearm_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_left_forearm = {
#include "shared/enemy_ai_art_models/ai_shielder_left_forearm.inc"
};
static Sint16 ai_shielder_left_upper_arm_polygons[135] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_upper_arm_polygons.inc"
};
static Sint32 ai_shielder_left_upper_arm_vertices[75] = {
#include "shared/enemy_ai_art_models/ai_shielder_left_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_left_upper_arm_model = {
#include "shared/enemy_ai_art_models/ai_shielder_left_upper_arm_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_left_upper_arm = {
#include "shared/enemy_ai_art_models/ai_shielder_left_upper_arm.inc"
};
static Sint16 ai_shielder_weapon_polygons[247] = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_polygons.inc"
};
static Sint32 ai_shielder_weapon_vertices[189] = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_weapon_model = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_model.inc"
};
NJS_CNK_OBJECT ai_shielder_weapon_object = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_object.inc"
};
static Sint16 ai_shielder_weapon_mount_polygons[441] = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_mount_polygons.inc"
};
static Sint32 ai_shielder_weapon_mount_vertices[279] = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_mount_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_weapon_mount_model = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_mount_model.inc"
};
static NJS_CNK_OBJECT ai_shielder_weapon_mount = {
#include "shared/enemy_ai_art_models/ai_shielder_weapon_mount.inc"
};
static Sint16 ai_shielder_body_polygons[163] = {
#include "shared/enemy_ai_art_models/ai_shielder_body_polygons.inc"
};
static Sint32 ai_shielder_body_vertices[111] = {
#include "shared/enemy_ai_art_models/ai_shielder_body_vertices.inc"
};
static NJS_CNK_MODEL ai_shielder_body_model = {
#include "shared/enemy_ai_art_models/ai_shielder_body_model.inc"
};
NJS_CNK_OBJECT ai_shielder_object = {
#include "shared/enemy_ai_art_models/ai_shielder_object.inc"
};
static NJS_MKEY_F ai_land_motion_joint_0_position[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_land_motion_joint_0_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_land_motion_joint_1_position[7] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_land_motion_joint_3_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_4_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_5_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_6_angles[13] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_7_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_8_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_9_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_9_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_11_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_12_angles[13] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_land_motion_joint_13_angles[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_land_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_land_motion_tracks.inc"
};
NJS_MOTION ai_land_motion = {
#include "shared/enemy_ai_art_models/ai_land_motion.inc"
};
static NJS_MKEY_F ai_shoot_motion_joint_0_position[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_0_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_0_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_3_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_4_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_5_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_6_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_7_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_8_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_9_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_9_angles.inc"
};
static NJS_MKEY_F ai_shoot_motion_joint_10_position[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_10_position.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_11_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_12_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_shoot_motion_joint_13_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_shoot_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_shoot_motion_tracks.inc"
};
NJS_MOTION ai_shoot_motion = {
#include "shared/enemy_ai_art_models/ai_shoot_motion.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_0_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_fly_shoot_motion_joint_1_position[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_3_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_4_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_5_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_6_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_7_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_8_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_9_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_9_angles.inc"
};
static NJS_MKEY_F ai_fly_shoot_motion_joint_10_position[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_10_position.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_11_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_12_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_fly_shoot_motion_joint_13_angles[9] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_fly_shoot_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion_tracks.inc"
};
NJS_MOTION ai_fly_shoot_motion = {
#include "shared/enemy_ai_art_models/ai_fly_shoot_motion.inc"
};
static NJS_MKEY_A ai_fly_motion_joint_0_angles[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_fly_motion_joint_1_position[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_fly_motion_joint_5_angles[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_fly_motion_joint_6_angles[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_fly_motion_joint_7_angles[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_fly_motion_joint_11_angles[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_fly_motion_joint_12_angles[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_fly_motion_joint_13_angles[3] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_fly_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_fly_motion_tracks.inc"
};
NJS_MOTION ai_fly_motion = {
#include "shared/enemy_ai_art_models/ai_fly_motion.inc"
};
static NJS_MKEY_F ai_shielder_close_motion_joint_0_position[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_0_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_shielder_close_motion_joint_1_position[10] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_1_position.inc"
};
static NJS_MKEY_F ai_shielder_close_motion_joint_2_position[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_2_position.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_3_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_4_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_5_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_6_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_7_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_8_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_9_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_9_angles.inc"
};
static NJS_MKEY_F ai_shielder_close_motion_joint_10_position[10] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_10_position.inc"
};
static NJS_MKEY_F ai_shielder_close_motion_joint_11_position[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_11_position.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_11_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_12_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_shielder_close_motion_joint_13_angles[11] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_shielder_close_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion_tracks.inc"
};
NJS_MOTION ai_shielder_close_motion = {
#include "shared/enemy_ai_art_models/ai_shielder_close_motion.inc"
};
static NJS_MKEY_F ai_shielder_open_motion_joint_0_position[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_0_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_shielder_open_motion_joint_2_position[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_2_position.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_3_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_4_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_5_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_6_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_7_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_8_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_9_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_9_angles.inc"
};
static NJS_MKEY_F ai_shielder_open_motion_joint_11_position[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_11_position.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_11_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_12_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_shielder_open_motion_joint_13_angles[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_shielder_open_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion_tracks.inc"
};
NJS_MOTION ai_shielder_open_motion = {
#include "shared/enemy_ai_art_models/ai_shielder_open_motion.inc"
};
static NJS_MKEY_F ai_shielder_deflect_motion_joint_0_position[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_0_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_shielder_deflect_motion_joint_1_position[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_1_position.inc"
};
static NJS_MKEY_F ai_shielder_deflect_motion_joint_2_position[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_2_position.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_3_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_4_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_5_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_6_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_7_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_8_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_9_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_9_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_11_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_12_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_shielder_deflect_motion_joint_13_angles[5] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_shielder_deflect_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion_tracks.inc"
};
NJS_MOTION ai_shielder_deflect_motion = {
#include "shared/enemy_ai_art_models/ai_shielder_deflect_motion.inc"
};
static NJS_MKEY_F ai_shielder_stand_motion_joint_0_position[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_0_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_shielder_stand_motion_joint_1_position[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_3_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_4_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_5_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_6_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_7_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_8_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_9_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_9_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_11_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_12_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_shielder_stand_motion_joint_13_angles[4] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_shielder_stand_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion_tracks.inc"
};
NJS_MOTION ai_shielder_stand_motion = {
#include "shared/enemy_ai_art_models/ai_shielder_stand_motion.inc"
};
static NJS_MKEY_F ai_shielder_turn_motion_joint_0_position[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_0_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_shielder_turn_motion_joint_2_position[2] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_2_position.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_3_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_4_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_5_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_6_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_7_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_8_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_9_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_9_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_11_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_12_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_shielder_turn_motion_joint_13_angles[8] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_shielder_turn_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion_tracks.inc"
};
NJS_MOTION ai_shielder_turn_motion = {
#include "shared/enemy_ai_art_models/ai_shielder_turn_motion.inc"
};
static NJS_MKEY_F ai_stand_motion_joint_0_position[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_0_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_stand_motion_joint_1_position[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_3_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_4_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_5_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_6_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_7_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_11_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_12_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_stand_motion_joint_13_angles[4] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_stand_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_stand_motion_tracks.inc"
};
NJS_MOTION ai_stand_motion = {
#include "shared/enemy_ai_art_models/ai_stand_motion.inc"
};
static NJS_MKEY_F ai_rise_motion_joint_0_position[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_0_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_rise_motion_joint_1_position[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_3_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_4_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_5_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_6_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_7_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_8_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_9_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_9_angles.inc"
};
static NJS_MKEY_F ai_rise_motion_joint_10_position[3] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_10_position.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_11_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_12_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_rise_motion_joint_13_angles[13] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_rise_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_rise_motion_tracks.inc"
};
NJS_MOTION ai_rise_motion = {
#include "shared/enemy_ai_art_models/ai_rise_motion.inc"
};
static NJS_MKEY_F ai_unused_turn_motion_joint_0_position[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_0_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_unused_turn_motion_joint_1_position[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_3_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_4_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_5_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_6_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_7_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_8_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_8_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_9_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_9_angles.inc"
};
static NJS_MKEY_F ai_unused_turn_motion_joint_10_position[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_10_position.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_11_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_12_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_unused_turn_motion_joint_13_angles[9] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_unused_turn_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion_tracks.inc"
};
NJS_MOTION ai_unused_turn_motion = {
#include "shared/enemy_ai_art_models/ai_unused_turn_motion.inc"
};
static NJS_MKEY_F ai_turn_motion_joint_0_position[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_0_position.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_0_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_0_angles.inc"
};
static NJS_MKEY_F ai_turn_motion_joint_1_position[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_1_position.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_3_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_3_angles.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_4_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_4_angles.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_5_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_5_angles.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_6_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_6_angles.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_7_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_7_angles.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_11_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_11_angles.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_12_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_12_angles.inc"
};
static NJS_MKEY_A ai_turn_motion_joint_13_angles[5] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_joint_13_angles.inc"
};
static NJS_MDATA2 ai_turn_motion_tracks[14] = {
#include "shared/enemy_ai_art_models/ai_turn_motion_tracks.inc"
};
NJS_MOTION ai_turn_motion = {
#include "shared/enemy_ai_art_models/ai_turn_motion.inc"
};
