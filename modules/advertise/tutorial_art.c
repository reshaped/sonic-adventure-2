/* the tutorial panel's art: the board and its picture, Omochao, the camera's
 * motion and the sprites' cells */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/tutorial_art.h"
#include "advertise/tutorial_panel.h"

static NJS_CAMERA camera = {
#include "advertise/tutorial_art/camera.inc"
};

static NJS_MKEY_F camera_position[16] = {
#include "advertise/tutorial_art/camera_position.inc"
};

static NJS_MKEY_F camera_target[16] = {
#include "advertise/tutorial_art/camera_target.inc"
};

static NJS_MKEY_A1 camera_roll[2] = {
#include "advertise/tutorial_art/camera_roll.inc"
};

static NJS_MKEY_A1 camera_view_angle[2] = {
#include "advertise/tutorial_art/camera_view_angle.inc"
};

static NJS_MDATA4 camera_channels = {
#include "advertise/tutorial_art/camera_channels.inc"
};
NJS_MOTION tutorial_camera_motion[1] = {
#include "advertise/tutorial_art/tutorial_camera_motion.inc"
};
/* the camera and its motion as an action, which no code reads */
static NJS_CACTION camera_action = {
#include "advertise/tutorial_art/camera_action.inc"
};

static Sint16 right_post_polygons[155] = {
#include "advertise/tutorial_art/right_post_polygons.inc"
};

static Sint32 right_post_vertices[123] = {
#include "advertise/tutorial_art/right_post_vertices.inc"
};

static NJS_CNK_MODEL right_post_mesh = {
#include "advertise/tutorial_art/right_post_mesh.inc"
};

static NJS_CNK_OBJECT right_post = {
#include "advertise/tutorial_art/right_post.inc"
};

static Sint16 left_post_polygons[149] = {
#include "advertise/tutorial_art/left_post_polygons.inc"
};

static Sint32 left_post_vertices[123] = {
#include "advertise/tutorial_art/left_post_vertices.inc"
};

static NJS_CNK_MODEL left_post_mesh = {
#include "advertise/tutorial_art/left_post_mesh.inc"
};

static NJS_CNK_OBJECT left_post = {
#include "advertise/tutorial_art/left_post.inc"
};

static Sint16 board_badge_polygons[393] = {
#include "advertise/tutorial_art/board_badge_polygons.inc"
};

static Sint32 board_badge_vertices[387] = {
#include "advertise/tutorial_art/board_badge_vertices.inc"
};

static NJS_CNK_MODEL board_badge_mesh = {
#include "advertise/tutorial_art/board_badge_mesh.inc"
};

static NJS_CNK_OBJECT board_badge = {
#include "advertise/tutorial_art/board_badge.inc"
};

static Sint16 board_panel_polygons[369] = {
#include "advertise/tutorial_art/board_panel_polygons.inc"
};

static Sint32 board_panel_vertices[219] = {
#include "advertise/tutorial_art/board_panel_vertices.inc"
};

static NJS_CNK_MODEL board_panel_mesh = {
#include "advertise/tutorial_art/board_panel_mesh.inc"
};

static NJS_CNK_OBJECT board_panel = {
#include "advertise/tutorial_art/board_panel.inc"
};

NJS_CNK_OBJECT tutorial_board_object = {
#include "advertise/tutorial_art/tutorial_board_object.inc"
};

struct tutorial_picture_polygons tutorial_picture_plist[1] = {
#include "advertise/tutorial_art/tutorial_picture_plist.inc"
};

static Sint32 picture_vertices[27] = {
#include "advertise/tutorial_art/picture_vertices.inc"
};

static NJS_CNK_MODEL picture_mesh = {
#include "advertise/tutorial_art/picture_mesh.inc"
};

NJS_CNK_OBJECT tutorial_picture_object = {
#include "advertise/tutorial_art/tutorial_picture_object.inc"
};

static Sint16 right_foot_polygons[189] = {
#include "advertise/tutorial_art/right_foot_polygons.inc"
};

static Sint32 right_foot_vertices[87] = {
#include "advertise/tutorial_art/right_foot_vertices.inc"
};

static NJS_CNK_MODEL right_foot_mesh = {
#include "advertise/tutorial_art/right_foot_mesh.inc"
};

static NJS_CNK_OBJECT right_foot = {
#include "advertise/tutorial_art/right_foot.inc"
};

static Sint16 right_arm_polygons[215] = {
#include "advertise/tutorial_art/right_arm_polygons.inc"
};

static Sint32 right_arm_vertices[99] = {
#include "advertise/tutorial_art/right_arm_vertices.inc"
};

static NJS_CNK_MODEL right_arm_mesh = {
#include "advertise/tutorial_art/right_arm_mesh.inc"
};

static NJS_CNK_OBJECT right_arm = {
#include "advertise/tutorial_art/right_arm.inc"
};

static Sint16 left_foot_polygons[189] = {
#include "advertise/tutorial_art/left_foot_polygons.inc"
};

static Sint32 left_foot_vertices[87] = {
#include "advertise/tutorial_art/left_foot_vertices.inc"
};

static NJS_CNK_MODEL left_foot_mesh = {
#include "advertise/tutorial_art/left_foot_mesh.inc"
};

static NJS_CNK_OBJECT left_foot = {
#include "advertise/tutorial_art/left_foot.inc"
};

static Sint16 left_arm_polygons[215] = {
#include "advertise/tutorial_art/left_arm_polygons.inc"
};

static Sint32 left_arm_vertices[99] = {
#include "advertise/tutorial_art/left_arm_vertices.inc"
};

static NJS_CNK_MODEL left_arm_mesh = {
#include "advertise/tutorial_art/left_arm_mesh.inc"
};

static NJS_CNK_OBJECT left_arm = {
#include "advertise/tutorial_art/left_arm.inc"
};

static Sint16 face_polygons[45] = {
#include "advertise/tutorial_art/face_polygons.inc"
};

static Sint32 face_vertices[63] = {
#include "advertise/tutorial_art/face_vertices.inc"
};

static NJS_CNK_MODEL face_mesh = {
#include "advertise/tutorial_art/face_mesh.inc"
};

static NJS_CNK_OBJECT face = {
#include "advertise/tutorial_art/face.inc"
};

static Sint16 spin_y_polygons[99] = {
#include "advertise/tutorial_art/spin_y_polygons.inc"
};

static Sint32 spin_y_vertices[69] = {
#include "advertise/tutorial_art/spin_y_vertices.inc"
};

static NJS_CNK_MODEL spin_y_mesh = {
#include "advertise/tutorial_art/spin_y_mesh.inc"
};

NJS_CNK_OBJECT tutorial_spin_y_object = {
#include "advertise/tutorial_art/tutorial_spin_y_object.inc"
};

static Sint16 head_polygons[519] = {
#include "advertise/tutorial_art/head_polygons.inc"
};

static Sint32 head_vertices[243] = {
#include "advertise/tutorial_art/head_vertices.inc"
};

static NJS_CNK_MODEL head_mesh = {
#include "advertise/tutorial_art/head_mesh.inc"
};

static NJS_CNK_OBJECT head = {
#include "advertise/tutorial_art/head.inc"
};

static Sint16 spin_z_polygons[137] = {
#include "advertise/tutorial_art/spin_z_polygons.inc"
};

static Sint32 spin_z_vertices[105] = {
#include "advertise/tutorial_art/spin_z_vertices.inc"
};

static NJS_CNK_MODEL spin_z_mesh = {
#include "advertise/tutorial_art/spin_z_mesh.inc"
};

NJS_CNK_OBJECT tutorial_spin_z_object = {
#include "advertise/tutorial_art/tutorial_spin_z_object.inc"
};

static Sint16 model_polygons[193] = {
#include "advertise/tutorial_art/model_polygons.inc"
};

static Sint32 model_vertices[135] = {
#include "advertise/tutorial_art/model_vertices.inc"
};

static NJS_CNK_MODEL model_mesh = {
#include "advertise/tutorial_art/model_mesh.inc"
};

NJS_CNK_OBJECT tutorial_model_object = {
#include "advertise/tutorial_art/tutorial_model_object.inc"
};

struct tutorial_picture tutorial_pictures[TUTORIAL_LESSONS] = {
#include "advertise/tutorial_art/tutorial_pictures.inc"
};

NJS_TEXANIM tutorial_cells[TUTORIAL_CELLS] = {
#include "advertise/tutorial_art/tutorial_cells.inc"
};
