/* the screen effect's nine rings, each with a channel of its own in
 * screen_effect_motion, and the camera's motion */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/screen_effect_art.h"

static NJS_CAMERA camera = {
#include "advertise/screen_effect_art/camera.inc"
};

static NJS_MKEY_F camera_position[3] = {
#include "advertise/screen_effect_art/camera_position.inc"
};

static NJS_MKEY_F camera_target[3] = {
#include "advertise/screen_effect_art/camera_target.inc"
};

static NJS_MKEY_A1 camera_roll[3] = {
#include "advertise/screen_effect_art/camera_roll.inc"
};

static NJS_MKEY_A1 camera_view_angle[3] = {
#include "advertise/screen_effect_art/camera_view_angle.inc"
};

static NJS_MDATA4 camera_channels = {
#include "advertise/screen_effect_art/camera_channels.inc"
};
NJS_MOTION screen_effect_camera_motion = {
#include "advertise/screen_effect_art/screen_effect_camera_motion.inc"
};
/* the camera and its motion as an action, which no code reads */
static NJS_CACTION camera_action = {
#include "advertise/screen_effect_art/camera_action.inc"
};

static Sint16 ring_9_polygons[211] = {
#include "advertise/screen_effect_art/ring_9_polygons.inc"
};

static Sint32 ring_9_vertices[387] = {
#include "advertise/screen_effect_art/ring_9_vertices.inc"
};

static NJS_CNK_MODEL ring_9_mesh = {
#include "advertise/screen_effect_art/ring_9_mesh.inc"
};

static NJS_CNK_OBJECT ring_9 = {
#include "advertise/screen_effect_art/ring_9.inc"
};

static Sint16 ring_8_polygons[211] = {
#include "advertise/screen_effect_art/ring_8_polygons.inc"
};

static Sint32 ring_8_vertices[387] = {
#include "advertise/screen_effect_art/ring_8_vertices.inc"
};

static NJS_CNK_MODEL ring_8_mesh = {
#include "advertise/screen_effect_art/ring_8_mesh.inc"
};

static NJS_CNK_OBJECT ring_8 = {
#include "advertise/screen_effect_art/ring_8.inc"
};

static Sint16 ring_7_polygons[211] = {
#include "advertise/screen_effect_art/ring_7_polygons.inc"
};

static Sint32 ring_7_vertices[387] = {
#include "advertise/screen_effect_art/ring_7_vertices.inc"
};

static NJS_CNK_MODEL ring_7_mesh = {
#include "advertise/screen_effect_art/ring_7_mesh.inc"
};

static NJS_CNK_OBJECT ring_7 = {
#include "advertise/screen_effect_art/ring_7.inc"
};

static Sint16 ring_6_polygons[211] = {
#include "advertise/screen_effect_art/ring_6_polygons.inc"
};

static Sint32 ring_6_vertices[387] = {
#include "advertise/screen_effect_art/ring_6_vertices.inc"
};

static NJS_CNK_MODEL ring_6_mesh = {
#include "advertise/screen_effect_art/ring_6_mesh.inc"
};

static NJS_CNK_OBJECT ring_6 = {
#include "advertise/screen_effect_art/ring_6.inc"
};

static Sint16 ring_5_polygons[211] = {
#include "advertise/screen_effect_art/ring_5_polygons.inc"
};

static Sint32 ring_5_vertices[387] = {
#include "advertise/screen_effect_art/ring_5_vertices.inc"
};

static NJS_CNK_MODEL ring_5_mesh = {
#include "advertise/screen_effect_art/ring_5_mesh.inc"
};

static NJS_CNK_OBJECT ring_5 = {
#include "advertise/screen_effect_art/ring_5.inc"
};

static Sint16 ring_4_polygons[211] = {
#include "advertise/screen_effect_art/ring_4_polygons.inc"
};

static Sint32 ring_4_vertices[387] = {
#include "advertise/screen_effect_art/ring_4_vertices.inc"
};

static NJS_CNK_MODEL ring_4_mesh = {
#include "advertise/screen_effect_art/ring_4_mesh.inc"
};

static NJS_CNK_OBJECT ring_4 = {
#include "advertise/screen_effect_art/ring_4.inc"
};

static Sint16 ring_3_polygons[211] = {
#include "advertise/screen_effect_art/ring_3_polygons.inc"
};

static Sint32 ring_3_vertices[387] = {
#include "advertise/screen_effect_art/ring_3_vertices.inc"
};

static NJS_CNK_MODEL ring_3_mesh = {
#include "advertise/screen_effect_art/ring_3_mesh.inc"
};

static NJS_CNK_OBJECT ring_3 = {
#include "advertise/screen_effect_art/ring_3.inc"
};

static Sint16 ring_2_polygons[211] = {
#include "advertise/screen_effect_art/ring_2_polygons.inc"
};

static Sint32 ring_2_vertices[387] = {
#include "advertise/screen_effect_art/ring_2_vertices.inc"
};

static NJS_CNK_MODEL ring_2_mesh = {
#include "advertise/screen_effect_art/ring_2_mesh.inc"
};

static NJS_CNK_OBJECT ring_2 = {
#include "advertise/screen_effect_art/ring_2.inc"
};

static Sint16 ring_1_polygons[211] = {
#include "advertise/screen_effect_art/ring_1_polygons.inc"
};

static Sint32 ring_1_vertices[387] = {
#include "advertise/screen_effect_art/ring_1_vertices.inc"
};

static NJS_CNK_MODEL ring_1_mesh = {
#include "advertise/screen_effect_art/ring_1_mesh.inc"
};

static NJS_CNK_OBJECT ring_1 = {
#include "advertise/screen_effect_art/ring_1.inc"
};

NJS_CNK_OBJECT screen_effect_object = {
#include "advertise/screen_effect_art/screen_effect_object.inc"
};

static NJS_MKEY_A ring_1_angles[29] = {
#include "advertise/screen_effect_art/ring_1_angles.inc"
};

static NJS_MKEY_A ring_2_angles[32] = {
#include "advertise/screen_effect_art/ring_2_angles.inc"
};

static NJS_MKEY_A ring_3_angles[12] = {
#include "advertise/screen_effect_art/ring_3_angles.inc"
};

static NJS_MKEY_A ring_4_angles[14] = {
#include "advertise/screen_effect_art/ring_4_angles.inc"
};

static NJS_MKEY_A ring_5_angles[14] = {
#include "advertise/screen_effect_art/ring_5_angles.inc"
};

static NJS_MKEY_A ring_6_angles[12] = {
#include "advertise/screen_effect_art/ring_6_angles.inc"
};

static NJS_MKEY_A ring_7_angles[12] = {
#include "advertise/screen_effect_art/ring_7_angles.inc"
};

static NJS_MKEY_A ring_8_angles[30] = {
#include "advertise/screen_effect_art/ring_8_angles.inc"
};

static NJS_MKEY_A ring_9_angles[32] = {
#include "advertise/screen_effect_art/ring_9_angles.inc"
};

static NJS_MDATA2 ring_channels[10] = {
#include "advertise/screen_effect_art/ring_channels.inc"
};

NJS_MOTION screen_effect_motion[1] = {
#include "advertise/screen_effect_art/screen_effect_motion.inc"
};
