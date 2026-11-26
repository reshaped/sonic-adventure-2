/* CART.PRS's chunk models and motions, from the disc. its D lies between
 * tornado_textures.c and race_clock.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/kart_art.h"

static NJS_CNK_OBJECT torso_tip = {
#include "cart/kart_art/torso_tip.inc"
};
static Sint16 torso_polygons[173] = {
#include "cart/kart_art/torso_polygons.inc"
};
static Sint32 torso_vertices[135] = {
#include "cart/kart_art/torso_vertices.inc"
};
static NJS_CNK_MODEL torso_mesh = {
#include "cart/kart_art/torso_mesh.inc"
};
static NJS_CNK_OBJECT torso = {
#include "cart/kart_art/torso.inc"
};
static NJS_CNK_OBJECT right_hand_tip = {
#include "cart/kart_art/right_hand_tip.inc"
};
static Sint16 right_hand_polygons[101] = {
#include "cart/kart_art/right_hand_polygons.inc"
};
static Sint32 right_hand_vertices[75] = {
#include "cart/kart_art/right_hand_vertices.inc"
};
static NJS_CNK_MODEL right_hand_mesh = {
#include "cart/kart_art/right_hand_mesh.inc"
};
static NJS_CNK_OBJECT right_hand = {
#include "cart/kart_art/right_hand.inc"
};
static NJS_CNK_OBJECT right_wrist = {
#include "cart/kart_art/right_wrist.inc"
};
static Sint16 right_forearm_polygons[125] = {
#include "cart/kart_art/right_forearm_polygons.inc"
};
static Sint32 right_forearm_vertices[99] = {
#include "cart/kart_art/right_forearm_vertices.inc"
};
static NJS_CNK_MODEL right_forearm_mesh = {
#include "cart/kart_art/right_forearm_mesh.inc"
};
static NJS_CNK_OBJECT right_forearm = {
#include "cart/kart_art/right_forearm.inc"
};
static NJS_CNK_OBJECT right_elbow = {
#include "cart/kart_art/right_elbow.inc"
};
static Sint16 right_upper_arm_polygons[69] = {
#include "cart/kart_art/right_upper_arm_polygons.inc"
};
static Sint32 right_upper_arm_vertices[51] = {
#include "cart/kart_art/right_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL right_upper_arm_mesh = {
#include "cart/kart_art/right_upper_arm_mesh.inc"
};
static NJS_CNK_OBJECT right_upper_arm = {
#include "cart/kart_art/right_upper_arm.inc"
};
static NJS_CNK_OBJECT right_shoulder_pivot = {
#include "cart/kart_art/right_shoulder_pivot.inc"
};
static NJS_CNK_OBJECT right_shoulder = {
#include "cart/kart_art/right_shoulder.inc"
};
static NJS_CNK_OBJECT left_hand_tip = {
#include "cart/kart_art/left_hand_tip.inc"
};
static Sint16 left_hand_polygons[101] = {
#include "cart/kart_art/left_hand_polygons.inc"
};
static Sint32 left_hand_vertices[75] = {
#include "cart/kart_art/left_hand_vertices.inc"
};
static NJS_CNK_MODEL left_hand_mesh = {
#include "cart/kart_art/left_hand_mesh.inc"
};
static NJS_CNK_OBJECT left_hand = {
#include "cart/kart_art/left_hand.inc"
};
static NJS_CNK_OBJECT left_wrist = {
#include "cart/kart_art/left_wrist.inc"
};
static Sint16 left_forearm_polygons[125] = {
#include "cart/kart_art/left_forearm_polygons.inc"
};
static Sint32 left_forearm_vertices[99] = {
#include "cart/kart_art/left_forearm_vertices.inc"
};
static NJS_CNK_MODEL left_forearm_mesh = {
#include "cart/kart_art/left_forearm_mesh.inc"
};
static NJS_CNK_OBJECT left_forearm = {
#include "cart/kart_art/left_forearm.inc"
};
static NJS_CNK_OBJECT left_elbow = {
#include "cart/kart_art/left_elbow.inc"
};
static Sint16 left_upper_arm_polygons[69] = {
#include "cart/kart_art/left_upper_arm_polygons.inc"
};
static Sint32 left_upper_arm_vertices[51] = {
#include "cart/kart_art/left_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL left_upper_arm_mesh = {
#include "cart/kart_art/left_upper_arm_mesh.inc"
};
static NJS_CNK_OBJECT left_upper_arm = {
#include "cart/kart_art/left_upper_arm.inc"
};
static NJS_CNK_OBJECT left_shoulder_pivot = {
#include "cart/kart_art/left_shoulder_pivot.inc"
};
static NJS_CNK_OBJECT left_shoulder = {
#include "cart/kart_art/left_shoulder.inc"
};
static NJS_CNK_OBJECT head_tip = {
#include "cart/kart_art/head_tip.inc"
};
static Sint16 head_polygons[1483] = {
#include "cart/kart_art/head_polygons.inc"
};
static Sint32 head_vertices[1215] = {
#include "cart/kart_art/head_vertices.inc"
};
static NJS_CNK_MODEL head_mesh = {
#include "cart/kart_art/head_mesh.inc"
};
static NJS_CNK_OBJECT head = {
#include "cart/kart_art/head.inc"
};
static NJS_CNK_OBJECT head_pivot = {
#include "cart/kart_art/head_pivot.inc"
};
static NJS_CNK_OBJECT neck = {
#include "cart/kart_art/neck.inc"
};
static NJS_CNK_OBJECT pilot_pivot = {
#include "cart/kart_art/pilot_pivot.inc"
};
static NJS_CNK_OBJECT pilot_mount = {
#include "cart/kart_art/pilot_mount.inc"
};
static Sint16 left_wing_trail_polygons[291] = {
#include "cart/kart_art/left_wing_trail_polygons.inc"
};
static Sint32 left_wing_trail_vertices[249] = {
#include "cart/kart_art/left_wing_trail_vertices.inc"
};
static NJS_CNK_MODEL left_wing_trail_mesh = {
#include "cart/kart_art/left_wing_trail_mesh.inc"
};
static NJS_CNK_OBJECT left_wing_trail = {
#include "cart/kart_art/left_wing_trail.inc"
};
static Sint16 right_skid_polygons[119] = {
#include "cart/kart_art/right_skid_polygons.inc"
};
static Sint32 right_skid_vertices[87] = {
#include "cart/kart_art/right_skid_vertices.inc"
};
static NJS_CNK_MODEL right_skid_mesh = {
#include "cart/kart_art/right_skid_mesh.inc"
};
static NJS_CNK_OBJECT right_skid = {
#include "cart/kart_art/right_skid.inc"
};
static Sint16 left_skid_polygons[119] = {
#include "cart/kart_art/left_skid_polygons.inc"
};
static Sint32 left_skid_vertices[87] = {
#include "cart/kart_art/left_skid_vertices.inc"
};
static NJS_CNK_MODEL left_skid_mesh = {
#include "cart/kart_art/left_skid_mesh.inc"
};
static NJS_CNK_OBJECT left_skid = {
#include "cart/kart_art/left_skid.inc"
};
static Sint16 canopy_polygons[65] = {
#include "cart/kart_art/canopy_polygons.inc"
};
static Sint32 canopy_vertices[75] = {
#include "cart/kart_art/canopy_vertices.inc"
};
static NJS_CNK_MODEL canopy_mesh = {
#include "cart/kart_art/canopy_mesh.inc"
};
static NJS_CNK_OBJECT canopy = {
#include "cart/kart_art/canopy.inc"
};
static Sint16 fuselage_polygons[2055] = {
#include "cart/kart_art/fuselage_polygons.inc"
};
static Sint32 fuselage_vertices[1611] = {
#include "cart/kart_art/fuselage_vertices.inc"
};
static NJS_CNK_MODEL fuselage_mesh = {
#include "cart/kart_art/fuselage_mesh.inc"
};
static NJS_CNK_OBJECT fuselage = {
#include "cart/kart_art/fuselage.inc"
};
static NJS_CNK_OBJECT tail_tip = {
#include "cart/kart_art/tail_tip.inc"
};
static NJS_CNK_OBJECT tail_mount = {
#include "cart/kart_art/tail_mount.inc"
};
static Sint16 tail_polygons[153] = {
#include "cart/kart_art/tail_polygons.inc"
};
static Sint32 tail_vertices[135] = {
#include "cart/kart_art/tail_vertices.inc"
};
static NJS_CNK_MODEL tail_mesh = {
#include "cart/kart_art/tail_mesh.inc"
};
static NJS_CNK_OBJECT tail = {
#include "cart/kart_art/tail.inc"
};
static Sint16 right_wing_trail_polygons[291] = {
#include "cart/kart_art/right_wing_trail_polygons.inc"
};
static Sint32 right_wing_trail_vertices[249] = {
#include "cart/kart_art/right_wing_trail_vertices.inc"
};
static NJS_CNK_MODEL right_wing_trail_mesh = {
#include "cart/kart_art/right_wing_trail_mesh.inc"
};
static NJS_CNK_OBJECT right_wing_trail = {
#include "cart/kart_art/right_wing_trail.inc"
};
static NJS_CNK_OBJECT cannon_tip = {
#include "cart/kart_art/cannon_tip.inc"
};
static Sint16 cannon_polygons[403] = {
#include "cart/kart_art/cannon_polygons.inc"
};
static Sint32 cannon_vertices[309] = {
#include "cart/kart_art/cannon_vertices.inc"
};
static NJS_CNK_MODEL cannon_mesh = {
#include "cart/kart_art/cannon_mesh.inc"
};
static NJS_CNK_OBJECT cannon = {
#include "cart/kart_art/cannon.inc"
};
static Sint16 gun_mount_polygons[151] = {
#include "cart/kart_art/gun_mount_polygons.inc"
};
static Sint32 gun_mount_vertices[111] = {
#include "cart/kart_art/gun_mount_vertices.inc"
};
static NJS_CNK_MODEL gun_mount_mesh = {
#include "cart/kart_art/gun_mount_mesh.inc"
};
static NJS_CNK_OBJECT gun_mount = {
#include "cart/kart_art/gun_mount.inc"
};
static Sint16 left_wheel_polygons[183] = {
#include "cart/kart_art/left_wheel_polygons.inc"
};
static Sint32 left_wheel_vertices[159] = {
#include "cart/kart_art/left_wheel_vertices.inc"
};
static NJS_CNK_MODEL left_wheel_mesh = {
#include "cart/kart_art/left_wheel_mesh.inc"
};
static NJS_CNK_OBJECT left_wheel = {
#include "cart/kart_art/left_wheel.inc"
};
static Sint16 left_gear_front_polygons[203] = {
#include "cart/kart_art/left_gear_front_polygons.inc"
};
static Sint32 left_gear_front_vertices[123] = {
#include "cart/kart_art/left_gear_front_vertices.inc"
};
static NJS_CNK_MODEL left_gear_front_mesh = {
#include "cart/kart_art/left_gear_front_mesh.inc"
};
static NJS_CNK_OBJECT left_gear_front = {
#include "cart/kart_art/left_gear_front.inc"
};
static Sint16 left_gear_polygons[179] = {
#include "cart/kart_art/left_gear_polygons.inc"
};
static Sint32 left_gear_vertices[135] = {
#include "cart/kart_art/left_gear_vertices.inc"
};
static NJS_CNK_MODEL left_gear_mesh = {
#include "cart/kart_art/left_gear_mesh.inc"
};
static NJS_CNK_OBJECT left_gear = {
#include "cart/kart_art/left_gear.inc"
};
static Sint16 right_wheel_polygons[183] = {
#include "cart/kart_art/right_wheel_polygons.inc"
};
static Sint32 right_wheel_vertices[159] = {
#include "cart/kart_art/right_wheel_vertices.inc"
};
static NJS_CNK_MODEL right_wheel_mesh = {
#include "cart/kart_art/right_wheel_mesh.inc"
};
static NJS_CNK_OBJECT right_wheel = {
#include "cart/kart_art/right_wheel.inc"
};
static Sint16 right_gear_front_polygons[209] = {
#include "cart/kart_art/right_gear_front_polygons.inc"
};
static Sint32 right_gear_front_vertices[123] = {
#include "cart/kart_art/right_gear_front_vertices.inc"
};
static NJS_CNK_MODEL right_gear_front_mesh = {
#include "cart/kart_art/right_gear_front_mesh.inc"
};
static NJS_CNK_OBJECT right_gear_front = {
#include "cart/kart_art/right_gear_front.inc"
};
static Sint16 right_gear_polygons[179] = {
#include "cart/kart_art/right_gear_polygons.inc"
};
static Sint32 right_gear_vertices[135] = {
#include "cart/kart_art/right_gear_vertices.inc"
};
static NJS_CNK_MODEL right_gear_mesh = {
#include "cart/kart_art/right_gear_mesh.inc"
};
static NJS_CNK_OBJECT right_gear = {
#include "cart/kart_art/right_gear.inc"
};
static NJS_CNK_OBJECT gear_mount = {
#include "cart/kart_art/gear_mount.inc"
};
static Sint16 right_engine_brace_polygons[121] = {
#include "cart/kart_art/right_engine_brace_polygons.inc"
};
static Sint32 right_engine_brace_vertices[87] = {
#include "cart/kart_art/right_engine_brace_vertices.inc"
};
static NJS_CNK_MODEL right_engine_brace_mesh = {
#include "cart/kart_art/right_engine_brace_mesh.inc"
};
static NJS_CNK_OBJECT right_engine_brace = {
#include "cart/kart_art/right_engine_brace.inc"
};
static Sint16 right_wing_polygons[435] = {
#include "cart/kart_art/right_wing_polygons.inc"
};
static Sint32 right_wing_vertices[381] = {
#include "cart/kart_art/right_wing_vertices.inc"
};
static NJS_CNK_MODEL right_wing_mesh = {
#include "cart/kart_art/right_wing_mesh.inc"
};
static NJS_CNK_OBJECT right_wing = {
#include "cart/kart_art/right_wing.inc"
};
static Sint16 right_wing_strut_polygons[387] = {
#include "cart/kart_art/right_wing_strut_polygons.inc"
};
static Sint32 right_wing_strut_vertices[159] = {
#include "cart/kart_art/right_wing_strut_vertices.inc"
};
static NJS_CNK_MODEL right_wing_strut_mesh = {
#include "cart/kart_art/right_wing_strut_mesh.inc"
};
static NJS_CNK_OBJECT right_wing_strut = {
#include "cart/kart_art/right_wing_strut.inc"
};
static Sint16 left_wing_polygons[429] = {
#include "cart/kart_art/left_wing_polygons.inc"
};
static Sint32 left_wing_vertices[381] = {
#include "cart/kart_art/left_wing_vertices.inc"
};
static NJS_CNK_MODEL left_wing_mesh = {
#include "cart/kart_art/left_wing_mesh.inc"
};
static NJS_CNK_OBJECT left_wing = {
#include "cart/kart_art/left_wing.inc"
};
static Sint16 left_wing_strut_polygons[387] = {
#include "cart/kart_art/left_wing_strut_polygons.inc"
};
static Sint32 left_wing_strut_vertices[159] = {
#include "cart/kart_art/left_wing_strut_vertices.inc"
};
static NJS_CNK_MODEL left_wing_strut_mesh = {
#include "cart/kart_art/left_wing_strut_mesh.inc"
};
static NJS_CNK_OBJECT left_wing_strut = {
#include "cart/kart_art/left_wing_strut.inc"
};
static Sint16 exhaust_polygons[71] = {
#include "cart/kart_art/exhaust_polygons.inc"
};
static Sint32 exhaust_vertices[81] = {
#include "cart/kart_art/exhaust_vertices.inc"
};
static NJS_CNK_MODEL exhaust_mesh = {
#include "cart/kart_art/exhaust_mesh.inc"
};
static NJS_CNK_OBJECT exhaust = {
#include "cart/kart_art/exhaust.inc"
};
static Sint16 engine_polygons[353] = {
#include "cart/kart_art/engine_polygons.inc"
};
static Sint32 engine_vertices[309] = {
#include "cart/kart_art/engine_vertices.inc"
};
static NJS_CNK_MODEL engine_mesh = {
#include "cart/kart_art/engine_mesh.inc"
};
static NJS_CNK_OBJECT engine = {
#include "cart/kart_art/engine.inc"
};
static Sint16 left_engine_brace_polygons[121] = {
#include "cart/kart_art/left_engine_brace_polygons.inc"
};
static Sint32 left_engine_brace_vertices[87] = {
#include "cart/kart_art/left_engine_brace_vertices.inc"
};
static NJS_CNK_MODEL left_engine_brace_mesh = {
#include "cart/kart_art/left_engine_brace_mesh.inc"
};
static NJS_CNK_OBJECT left_engine_brace = {
#include "cart/kart_art/left_engine_brace.inc"
};
static NJS_CNK_OBJECT engine_mount = {
#include "cart/kart_art/engine_mount.inc"
};
NJS_CNK_OBJECT cart_tornado_model = {
#include "cart/kart_art/cart_tornado_model.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_0_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_0_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_0_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_0_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_0_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_0_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_1_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_1_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_1_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_1_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_1_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_1_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_2_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_2_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_2_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_2_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_2_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_2_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_3_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_3_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_3_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_3_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_3_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_3_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_4_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_4_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_4_angles[10] = {
#include "cart/kart_art/cart_tornado_motion_joint_4_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_4_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_4_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_5_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_5_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_5_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_5_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_5_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_5_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_6_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_6_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_6_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_6_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_6_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_6_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_7_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_7_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_7_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_7_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_7_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_7_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_8_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_8_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_8_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_8_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_8_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_8_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_9_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_9_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_9_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_9_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_9_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_9_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_10_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_10_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_10_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_10_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_10_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_10_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_11_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_11_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_11_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_11_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_11_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_11_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_12_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_12_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_12_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_12_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_12_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_12_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_13_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_13_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_13_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_13_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_13_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_13_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_14_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_14_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_14_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_14_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_14_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_14_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_15_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_15_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_15_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_15_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_15_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_15_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_16_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_16_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_16_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_16_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_16_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_16_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_17_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_17_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_17_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_17_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_17_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_17_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_18_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_18_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_18_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_18_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_18_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_18_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_19_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_19_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_19_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_19_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_19_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_19_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_20_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_20_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_20_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_20_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_20_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_20_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_21_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_21_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_21_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_21_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_21_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_21_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_22_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_22_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_22_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_22_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_22_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_22_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_23_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_23_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_23_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_23_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_23_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_23_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_24_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_24_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_24_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_24_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_24_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_24_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_25_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_25_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_25_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_25_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_25_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_25_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_26_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_26_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_26_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_26_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_26_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_26_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_27_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_27_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_27_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_27_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_27_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_27_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_28_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_28_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_28_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_28_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_28_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_28_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_29_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_29_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_29_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_29_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_29_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_29_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_30_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_30_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_30_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_30_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_30_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_30_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_31_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_31_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_31_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_31_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_31_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_31_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_32_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_32_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_32_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_32_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_32_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_32_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_33_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_33_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_33_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_33_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_33_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_33_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_34_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_34_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_34_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_34_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_34_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_34_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_35_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_35_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_35_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_35_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_35_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_35_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_36_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_36_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_36_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_36_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_36_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_36_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_37_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_37_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_37_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_37_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_37_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_37_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_38_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_38_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_38_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_38_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_38_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_38_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_39_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_39_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_39_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_39_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_39_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_39_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_40_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_40_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_40_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_40_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_40_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_40_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_41_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_41_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_41_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_41_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_41_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_41_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_42_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_42_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_42_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_42_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_42_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_42_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_43_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_43_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_43_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_43_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_43_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_43_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_44_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_44_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_44_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_44_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_44_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_44_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_45_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_45_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_45_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_45_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_45_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_45_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_46_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_46_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_46_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_46_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_46_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_46_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_47_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_47_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_47_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_47_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_47_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_47_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_48_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_48_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_48_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_48_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_48_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_48_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_49_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_49_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_49_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_49_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_49_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_49_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_50_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_50_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_50_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_50_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_50_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_50_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_51_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_51_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_51_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_51_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_51_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_51_scale.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_52_position[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_52_position.inc"
};
static NJS_MKEY_A cart_tornado_motion_joint_52_angles[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_52_angles.inc"
};
static NJS_MKEY_F cart_tornado_motion_joint_52_scale[2] = {
#include "cart/kart_art/cart_tornado_motion_joint_52_scale.inc"
};
static NJS_MDATA3 cart_tornado_motion_tracks[53] = {
#include "cart/kart_art/cart_tornado_motion_tracks.inc"
};
NJS_MOTION cart_tornado_motion[1] = {
#include "cart/kart_art/cart_tornado_motion.inc"
};
static Sint16 racer_shadow_polygons[287] = {
#include "cart/kart_art/racer_shadow_polygons.inc"
};
static Sint32 racer_shadow_vertices[162] = {
#include "cart/kart_art/racer_shadow_vertices.inc"
};
static NJS_CNK_MODEL racer_shadow_model = {
#include "cart/kart_art/racer_shadow_model.inc"
};
NJS_CNK_OBJECT cart_racer_shadow = {
#include "cart/kart_art/cart_racer_shadow.inc"
};
static Sint16 rouge_shadow_polygons[125] = {
#include "cart/kart_art/rouge_shadow_polygons.inc"
};
static Sint32 rouge_shadow_vertices[69] = {
#include "cart/kart_art/rouge_shadow_vertices.inc"
};
static NJS_CNK_MODEL rouge_shadow_model = {
#include "cart/kart_art/rouge_shadow_model.inc"
};
NJS_CNK_OBJECT cart_rouge_shadow = {
#include "cart/kart_art/cart_rouge_shadow.inc"
};
static Sint16 tails_shadow_polygons[287] = {
#include "cart/kart_art/tails_shadow_polygons.inc"
};
static Sint32 tails_shadow_vertices[174] = {
#include "cart/kart_art/tails_shadow_vertices.inc"
};
static NJS_CNK_MODEL tails_shadow_model = {
#include "cart/kart_art/tails_shadow_model.inc"
};
NJS_CNK_OBJECT cart_tails_shadow = {
#include "cart/kart_art/cart_tails_shadow.inc"
};
