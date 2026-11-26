/* CART.PRS's chunk models and texture lists, from the disc. its D lies
 * between kart_rouge.c and tornado_textures.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/kart_art.h"
#include "cart/kart_models.h"
#include "cart/shared_texture_names.h"

static NJS_CNK_OBJECT head_tip = {
#include "cart/kart_tails_story/head_tip.inc"
};
static Sint16 head_polygons[1681] = {
#include "cart/kart_tails_story/head_polygons.inc"
};
static Sint32 head_vertices[1269] = {
#include "cart/kart_tails_story/head_vertices.inc"
};
static NJS_CNK_MODEL head_mesh = {
#include "cart/kart_tails_story/head_mesh.inc"
};
static NJS_CNK_OBJECT head = {
#include "cart/kart_tails_story/head.inc"
};
static Sint16 left_eye_polygons[63] = {
#include "cart/kart_tails_story/left_eye_polygons.inc"
};
static Sint32 left_eye_vertices[69] = {
#include "cart/kart_tails_story/left_eye_vertices.inc"
};
static NJS_CNK_MODEL left_eye_mesh = {
#include "cart/kart_tails_story/left_eye_mesh.inc"
};
static NJS_CNK_OBJECT left_eye = {
#include "cart/kart_tails_story/left_eye.inc"
};
static Sint16 right_eye_polygons[63] = {
#include "cart/kart_tails_story/right_eye_polygons.inc"
};
static Sint32 right_eye_vertices[69] = {
#include "cart/kart_tails_story/right_eye_vertices.inc"
};
static NJS_CNK_MODEL right_eye_mesh = {
#include "cart/kart_tails_story/right_eye_mesh.inc"
};
static NJS_CNK_OBJECT right_eye = {
#include "cart/kart_tails_story/right_eye.inc"
};
static NJS_CNK_OBJECT eye_mount = {
#include "cart/kart_tails_story/eye_mount.inc"
};
static NJS_CNK_OBJECT head_pivot = {
#include "cart/kart_tails_story/head_pivot.inc"
};
static NJS_CNK_OBJECT neck = {
#include "cart/kart_tails_story/neck.inc"
};
static Sint16 left_glove_polygons[373] = {
#include "cart/kart_tails_story/left_glove_polygons.inc"
};
static Sint32 left_glove_vertices[267] = {
#include "cart/kart_tails_story/left_glove_vertices.inc"
};
static NJS_CNK_MODEL left_glove_mesh = {
#include "cart/kart_tails_story/left_glove_mesh.inc"
};
static NJS_CNK_OBJECT left_glove = {
#include "cart/kart_tails_story/left_glove.inc"
};
static NJS_CNK_OBJECT left_wrist = {
#include "cart/kart_tails_story/left_wrist.inc"
};
static Sint16 left_forearm_polygons[2] = {
#include "cart/kart_tails_story/left_forearm_polygons.inc"
};
static Sint32 left_forearm_vertices[164] = {
#include "cart/kart_tails_story/left_forearm_vertices.inc"
};
static NJS_CNK_MODEL left_forearm_mesh = {
#include "cart/kart_tails_story/left_forearm_mesh.inc"
};
static NJS_CNK_OBJECT left_forearm = {
#include "cart/kart_tails_story/left_forearm.inc"
};
static Sint32 left_upper_arm_vertices[172] = {
#include "cart/kart_tails_story/left_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL left_upper_arm_mesh = {
#include "cart/kart_tails_story/left_upper_arm_mesh.inc"
};
static NJS_CNK_OBJECT left_upper_arm = {
#include "cart/kart_tails_story/left_upper_arm.inc"
};
static NJS_CNK_OBJECT left_shoulder = {
#include "cart/kart_tails_story/left_shoulder.inc"
};
static Sint16 left_arm_strips_polygons[175] = {
#include "cart/kart_tails_story/left_arm_strips_polygons.inc"
};
static NJS_CNK_MODEL left_arm_strips_mesh = {
#include "cart/kart_tails_story/left_arm_strips_mesh.inc"
};
static NJS_CNK_OBJECT left_arm_strips = {
#include "cart/kart_tails_story/left_arm_strips.inc"
};
static Sint16 right_glove_polygons[369] = {
#include "cart/kart_tails_story/right_glove_polygons.inc"
};
static Sint32 right_glove_vertices[267] = {
#include "cart/kart_tails_story/right_glove_vertices.inc"
};
static NJS_CNK_MODEL right_glove_mesh = {
#include "cart/kart_tails_story/right_glove_mesh.inc"
};
static NJS_CNK_OBJECT right_glove = {
#include "cart/kart_tails_story/right_glove.inc"
};
static NJS_CNK_OBJECT right_wrist = {
#include "cart/kart_tails_story/right_wrist.inc"
};
static Sint16 right_forearm_polygons[2] = {
#include "cart/kart_tails_story/right_forearm_polygons.inc"
};
static Sint32 right_forearm_vertices[171] = {
#include "cart/kart_tails_story/right_forearm_vertices.inc"
};
static NJS_CNK_MODEL right_forearm_mesh = {
#include "cart/kart_tails_story/right_forearm_mesh.inc"
};
static NJS_CNK_OBJECT right_forearm = {
#include "cart/kart_tails_story/right_forearm.inc"
};
static Sint32 right_upper_arm_vertices[171] = {
#include "cart/kart_tails_story/right_upper_arm_vertices.inc"
};
static NJS_CNK_MODEL right_upper_arm_mesh = {
#include "cart/kart_tails_story/right_upper_arm_mesh.inc"
};
static NJS_CNK_OBJECT right_upper_arm = {
#include "cart/kart_tails_story/right_upper_arm.inc"
};
static NJS_CNK_OBJECT right_shoulder = {
#include "cart/kart_tails_story/right_shoulder.inc"
};
static Sint16 right_arm_strips_polygons[175] = {
#include "cart/kart_tails_story/right_arm_strips_polygons.inc"
};
static NJS_CNK_MODEL right_arm_strips_mesh = {
#include "cart/kart_tails_story/right_arm_strips_mesh.inc"
};
static NJS_CNK_OBJECT right_arm_strips = {
#include "cart/kart_tails_story/right_arm_strips.inc"
};
static Sint16 pilot_torso_polygons[439] = {
#include "cart/kart_tails_story/pilot_torso_polygons.inc"
};
static Sint32 pilot_torso_vertices[405] = {
#include "cart/kart_tails_story/pilot_torso_vertices.inc"
};
static NJS_CNK_MODEL pilot_torso_mesh = {
#include "cart/kart_tails_story/pilot_torso_mesh.inc"
};
static NJS_CNK_OBJECT pilot_torso = {
#include "cart/kart_tails_story/pilot_torso.inc"
};
static NJS_CNK_OBJECT pilot_mount = {
#include "cart/kart_tails_story/pilot_mount.inc"
};
static Sint16 kart_tails_story_chassis_polygons[3309] = {
#include "cart/kart_tails_story/kart_tails_story_chassis_polygons.inc"
};
static Sint32 kart_tails_story_chassis_vertices[2715] = {
#include "cart/kart_tails_story/kart_tails_story_chassis_vertices.inc"
};
static NJS_CNK_MODEL chassis_mesh = {
#include "cart/kart_tails_story/chassis_mesh.inc"
};
static NJS_CNK_OBJECT kart_tails_story_chassis = {
#include "cart/kart_tails_story/kart_tails_story_chassis.inc"
};
static Sint16 front_right_wheel_polygons[369] = {
#include "cart/kart_tails_story/front_right_wheel_polygons.inc"
};
static Sint32 front_right_wheel_vertices[231] = {
#include "cart/kart_tails_story/front_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL front_right_wheel_mesh = {
#include "cart/kart_tails_story/front_right_wheel_mesh.inc"
};
static NJS_CNK_OBJECT front_right_wheel = {
#include "cart/kart_tails_story/front_right_wheel.inc"
};
static Sint16 front_left_wheel_polygons[369] = {
#include "cart/kart_tails_story/front_left_wheel_polygons.inc"
};
static Sint32 front_left_wheel_vertices[231] = {
#include "cart/kart_tails_story/front_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL front_left_wheel_mesh = {
#include "cart/kart_tails_story/front_left_wheel_mesh.inc"
};
static NJS_CNK_OBJECT front_left_wheel = {
#include "cart/kart_tails_story/front_left_wheel.inc"
};
static Sint16 rear_right_wheel_polygons[361] = {
#include "cart/kart_tails_story/rear_right_wheel_polygons.inc"
};
static Sint32 rear_right_wheel_vertices[291] = {
#include "cart/kart_tails_story/rear_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL rear_right_wheel_mesh = {
#include "cart/kart_tails_story/rear_right_wheel_mesh.inc"
};
static NJS_CNK_OBJECT rear_right_wheel = {
#include "cart/kart_tails_story/rear_right_wheel.inc"
};
static Sint16 rear_left_wheel_polygons[361] = {
#include "cart/kart_tails_story/rear_left_wheel_polygons.inc"
};
static Sint32 rear_left_wheel_vertices[291] = {
#include "cart/kart_tails_story/rear_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL rear_left_wheel_mesh = {
#include "cart/kart_tails_story/rear_left_wheel_mesh.inc"
};
static NJS_CNK_OBJECT rear_left_wheel = {
#include "cart/kart_tails_story/rear_left_wheel.inc"
};
NJS_CNK_OBJECT cart_tails_story_model = {
#include "cart/kart_tails_story/cart_tails_story_model.inc"
};
static Sint16 kart_rouge_story_driver_detail_polygons[2265] = {
#include "cart/kart_tails_story/kart_rouge_story_driver_detail_polygons.inc"
};
static Sint32 kart_rouge_story_driver_detail_vertices[2139] = {
#include "cart/kart_tails_story/kart_rouge_story_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_driver_detail_model = {
#include "cart/kart_tails_story/kart_rouge_story_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_driver_detail = {
#include "cart/kart_tails_story/kart_rouge_story_driver_detail.inc"
};
static Sint16 kart_rouge_story_driver_polygons[1863] = {
#include "cart/kart_tails_story/kart_rouge_story_driver_polygons.inc"
};
static Sint32 kart_rouge_story_driver_vertices[1353] = {
#include "cart/kart_tails_story/kart_rouge_story_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_driver_model = {
#include "cart/kart_tails_story/kart_rouge_story_driver_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_driver = {
#include "cart/kart_tails_story/kart_rouge_story_driver.inc"
};
static Sint16 kart_rouge_story_chassis_polygons[2519] = {
#include "cart/kart_tails_story/kart_rouge_story_chassis_polygons.inc"
};
static Sint32 kart_rouge_story_chassis_vertices[2931] = {
#include "cart/kart_tails_story/kart_rouge_story_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_chassis_model = {
#include "cart/kart_tails_story/kart_rouge_story_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_chassis = {
#include "cart/kart_tails_story/kart_rouge_story_chassis.inc"
};
static Sint16 kart_rouge_story_right_wheel_polygons[139] = {
#include "cart/kart_tails_story/kart_rouge_story_right_wheel_polygons.inc"
};
static Sint32 kart_rouge_story_right_wheel_vertices[99] = {
#include "cart/kart_tails_story/kart_rouge_story_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_right_wheel_model = {
#include "cart/kart_tails_story/kart_rouge_story_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_right_wheel = {
#include "cart/kart_tails_story/kart_rouge_story_right_wheel.inc"
};
static Sint16 kart_rouge_story_left_wheel_polygons[139] = {
#include "cart/kart_tails_story/kart_rouge_story_left_wheel_polygons.inc"
};
static Sint32 kart_rouge_story_left_wheel_vertices[99] = {
#include "cart/kart_tails_story/kart_rouge_story_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_left_wheel_model = {
#include "cart/kart_tails_story/kart_rouge_story_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_left_wheel = {
#include "cart/kart_tails_story/kart_rouge_story_left_wheel.inc"
};
static Sint16 kart_rouge_story_paired_wheels_polygons[265] = {
#include "cart/kart_tails_story/kart_rouge_story_paired_wheels_polygons.inc"
};
static Sint32 kart_rouge_story_paired_wheels_vertices[195] = {
#include "cart/kart_tails_story/kart_rouge_story_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_paired_wheels_model = {
#include "cart/kart_tails_story/kart_rouge_story_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_paired_wheels = {
#include "cart/kart_tails_story/kart_rouge_story_paired_wheels.inc"
};
NJS_CNK_OBJECT cart_rouge_story_model = {
#include "cart/kart_tails_story/cart_rouge_story_model.inc"
};
static Sint16 kart_rouge_story_distant_driver_detail_polygons[1983] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver_detail_polygons.inc"
};
static Sint32 kart_rouge_story_distant_driver_detail_vertices[1893] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_distant_driver_detail_model = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_distant_driver_detail = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver_detail.inc"
};
static Sint16 kart_rouge_story_distant_driver_polygons[1273] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver_polygons.inc"
};
static Sint32 kart_rouge_story_distant_driver_vertices[915] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_distant_driver_model = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_distant_driver = {
#include "cart/kart_tails_story/kart_rouge_story_distant_driver.inc"
};
static Sint16 kart_rouge_story_distant_chassis_polygons[1433] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_chassis_polygons.inc"
};
static Sint32 kart_rouge_story_distant_chassis_vertices[1509] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_distant_chassis_model = {
#include "cart/kart_tails_story/kart_rouge_story_distant_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_distant_chassis = {
#include "cart/kart_tails_story/kart_rouge_story_distant_chassis.inc"
};
static Sint16 kart_rouge_story_distant_right_wheel_polygons[131] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_right_wheel_polygons.inc"
};
static Sint32 kart_rouge_story_distant_right_wheel_vertices[99] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_distant_right_wheel_model = {
#include "cart/kart_tails_story/kart_rouge_story_distant_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_distant_right_wheel = {
#include "cart/kart_tails_story/kart_rouge_story_distant_right_wheel.inc"
};
static Sint16 kart_rouge_story_distant_left_wheel_polygons[131] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_left_wheel_polygons.inc"
};
static Sint32 kart_rouge_story_distant_left_wheel_vertices[99] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_distant_left_wheel_model = {
#include "cart/kart_tails_story/kart_rouge_story_distant_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_distant_left_wheel = {
#include "cart/kart_tails_story/kart_rouge_story_distant_left_wheel.inc"
};
static Sint16 kart_rouge_story_distant_paired_wheels_polygons[251] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_paired_wheels_polygons.inc"
};
static Sint32 kart_rouge_story_distant_paired_wheels_vertices[195] = {
#include "cart/kart_tails_story/kart_rouge_story_distant_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_story_distant_paired_wheels_model = {
#include "cart/kart_tails_story/kart_rouge_story_distant_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_story_distant_paired_wheels = {
#include "cart/kart_tails_story/kart_rouge_story_distant_paired_wheels.inc"
};
NJS_CNK_OBJECT cart_rouge_story_second = {
#include "cart/kart_tails_story/cart_rouge_story_second.inc"
};
static NJS_TEXNAME limo_slots[9] = {
#include "cart/kart_tails_story/limo_slots.inc"
};
NJS_TEXLIST cart_limo_textures = {
#include "cart/kart_tails_story/cart_limo_textures.inc"
};
static Sint16 limo_volume_polygons[113] = {
#include "cart/kart_tails_story/limo_volume_polygons.inc"
};
static Sint32 limo_volume_vertices[63] = {
#include "cart/kart_tails_story/limo_volume_vertices.inc"
};
static NJS_CNK_MODEL limo_volume_model = {
#include "cart/kart_tails_story/limo_volume_model.inc"
};
static NJS_CNK_OBJECT limo_volume = {
#include "cart/kart_tails_story/limo_volume.inc"
};
static Sint16 rear_marker_polygons[25] = {
#include "cart/kart_tails_story/rear_marker_polygons.inc"
};
static Sint32 rear_marker_vertices[27] = {
#include "cart/kart_tails_story/rear_marker_vertices.inc"
};
static NJS_CNK_MODEL rear_marker_model = {
#include "cart/kart_tails_story/rear_marker_model.inc"
};
static NJS_CNK_OBJECT rear_marker = {
#include "cart/kart_tails_story/rear_marker.inc"
};
static Sint16 front_marker_polygons[25] = {
#include "cart/kart_tails_story/front_marker_polygons.inc"
};
static Sint32 front_marker_vertices[27] = {
#include "cart/kart_tails_story/front_marker_vertices.inc"
};
static NJS_CNK_MODEL front_marker_model = {
#include "cart/kart_tails_story/front_marker_model.inc"
};
static NJS_CNK_OBJECT front_marker = {
#include "cart/kart_tails_story/front_marker.inc"
};
static Sint16 front_axle_polygons[1149] = {
#include "cart/kart_tails_story/front_axle_polygons.inc"
};
static Sint32 front_axle_vertices[1155] = {
#include "cart/kart_tails_story/front_axle_vertices.inc"
};
static NJS_CNK_MODEL front_axle_model = {
#include "cart/kart_tails_story/front_axle_model.inc"
};
static NJS_CNK_OBJECT front_axle = {
#include "cart/kart_tails_story/front_axle.inc"
};
static Sint16 underside_polygons[245] = {
#include "cart/kart_tails_story/underside_polygons.inc"
};
static Sint32 underside_vertices[273] = {
#include "cart/kart_tails_story/underside_vertices.inc"
};
static NJS_CNK_MODEL underside_model = {
#include "cart/kart_tails_story/underside_model.inc"
};
static NJS_CNK_OBJECT underside = {
#include "cart/kart_tails_story/underside.inc"
};
static Sint16 body_polygons[5025] = {
#include "cart/kart_tails_story/body_polygons.inc"
};
static Sint32 body_vertices[5649] = {
#include "cart/kart_tails_story/body_vertices.inc"
};
static NJS_CNK_MODEL body_model = {
#include "cart/kart_tails_story/body_model.inc"
};
static NJS_CNK_OBJECT body = {
#include "cart/kart_tails_story/body.inc"
};
static Sint16 right_side_polygons[85] = {
#include "cart/kart_tails_story/right_side_polygons.inc"
};
static Sint32 right_side_vertices[147] = {
#include "cart/kart_tails_story/right_side_vertices.inc"
};
static NJS_CNK_MODEL right_side_model = {
#include "cart/kart_tails_story/right_side_model.inc"
};
static NJS_CNK_OBJECT right_side = {
#include "cart/kart_tails_story/right_side.inc"
};
static Sint16 front_door_handle_polygons[93] = {
#include "cart/kart_tails_story/front_door_handle_polygons.inc"
};
static Sint32 front_door_handle_vertices[99] = {
#include "cart/kart_tails_story/front_door_handle_vertices.inc"
};
static NJS_CNK_MODEL front_door_handle_model = {
#include "cart/kart_tails_story/front_door_handle_model.inc"
};
static NJS_CNK_OBJECT front_door_handle = {
#include "cart/kart_tails_story/front_door_handle.inc"
};
static Sint16 rear_door_handle_polygons[93] = {
#include "cart/kart_tails_story/rear_door_handle_polygons.inc"
};
static Sint32 rear_door_handle_vertices[99] = {
#include "cart/kart_tails_story/rear_door_handle_vertices.inc"
};
static NJS_CNK_MODEL rear_door_handle_model = {
#include "cart/kart_tails_story/rear_door_handle_model.inc"
};
static NJS_CNK_OBJECT rear_door_handle = {
#include "cart/kart_tails_story/rear_door_handle.inc"
};
static Sint16 rear_axle_polygons[1149] = {
#include "cart/kart_tails_story/rear_axle_polygons.inc"
};
static Sint32 rear_axle_vertices[1155] = {
#include "cart/kart_tails_story/rear_axle_vertices.inc"
};
static NJS_CNK_MODEL rear_axle_model = {
#include "cart/kart_tails_story/rear_axle_model.inc"
};
static NJS_CNK_OBJECT rear_axle = {
#include "cart/kart_tails_story/rear_axle.inc"
};
static Sint16 left_side_polygons[81] = {
#include "cart/kart_tails_story/left_side_polygons.inc"
};
static Sint32 left_side_vertices[147] = {
#include "cart/kart_tails_story/left_side_vertices.inc"
};
static NJS_CNK_MODEL left_side_model = {
#include "cart/kart_tails_story/left_side_model.inc"
};
static NJS_CNK_OBJECT left_side = {
#include "cart/kart_tails_story/left_side.inc"
};
NJS_CNK_OBJECT cart_limo_model = {
#include "cart/kart_tails_story/cart_limo_model.inc"
};
