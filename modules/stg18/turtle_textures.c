#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/turtle_art.h"

/* The shell and eye texture lists share these word-aligned filenames. */
const char turtle_texture_filenames[31] = {
#include "stg18/turtle_textures/turtle_texture_filenames.inc"
};

/* STG18.PRS's texture lists, chunk models, Basic models and motions, from
 * the disc. its D lies between objects.c and turtle.c */
static NJS_TEXNAME kame_textures[2] = {
#include "stg18/turtle_textures/kame_textures.inc"
};
NJS_TEXLIST turtle_texlist = {
#include "stg18/turtle_textures/turtle_texlist.inc"
};
static Sint16 kame_fore_right_polygons[73] = {
#include "stg18/turtle_textures/kame_fore_right_polygons.inc"
};
static Sint32 kame_fore_right_vertices[57] = {
#include "stg18/turtle_textures/kame_fore_right_vertices.inc"
};
static NJS_CNK_MODEL kame_fore_right_model = {
#include "stg18/turtle_textures/kame_fore_right_model.inc"
};
static NJS_CNK_OBJECT kame_fore_right = {
#include "stg18/turtle_textures/kame_fore_right.inc"
};
static Sint16 kame_eyes_polygons[165] = {
#include "stg18/turtle_textures/kame_eyes_polygons.inc"
};
static Sint32 kame_eyes_vertices[123] = {
#include "stg18/turtle_textures/kame_eyes_vertices.inc"
};
static NJS_CNK_MODEL kame_eyes_model = {
#include "stg18/turtle_textures/kame_eyes_model.inc"
};
NJS_CNK_OBJECT turtle_eyes[1] = {
#include "stg18/turtle_textures/turtle_eyes.inc"
};
static Sint16 kame_fore_left_polygons[73] = {
#include "stg18/turtle_textures/kame_fore_left_polygons.inc"
};
static Sint32 kame_fore_left_vertices[57] = {
#include "stg18/turtle_textures/kame_fore_left_vertices.inc"
};
static NJS_CNK_MODEL kame_fore_left_model = {
#include "stg18/turtle_textures/kame_fore_left_model.inc"
};
static NJS_CNK_OBJECT kame_fore_left = {
#include "stg18/turtle_textures/kame_fore_left.inc"
};
static Sint16 kame_hind_left_polygons[79] = {
#include "stg18/turtle_textures/kame_hind_left_polygons.inc"
};
static Sint32 kame_hind_left_vertices[57] = {
#include "stg18/turtle_textures/kame_hind_left_vertices.inc"
};
static NJS_CNK_MODEL kame_hind_left_model = {
#include "stg18/turtle_textures/kame_hind_left_model.inc"
};
static NJS_CNK_OBJECT kame_hind_left = {
#include "stg18/turtle_textures/kame_hind_left.inc"
};
static Sint16 kame_hind_right_polygons[73] = {
#include "stg18/turtle_textures/kame_hind_right_polygons.inc"
};
static Sint32 kame_hind_right_vertices[57] = {
#include "stg18/turtle_textures/kame_hind_right_vertices.inc"
};
static NJS_CNK_MODEL kame_hind_right_model = {
#include "stg18/turtle_textures/kame_hind_right_model.inc"
};
static NJS_CNK_OBJECT kame_hind_right = {
#include "stg18/turtle_textures/kame_hind_right.inc"
};
static Sint16 kame_shell_polygons[331] = {
#include "stg18/turtle_textures/kame_shell_polygons.inc"
};
static Sint32 kame_shell_vertices[249] = {
#include "stg18/turtle_textures/kame_shell_vertices.inc"
};
static NJS_CNK_MODEL kame_shell_model = {
#include "stg18/turtle_textures/kame_shell_model.inc"
};
NJS_CNK_OBJECT turtle_object = {
#include "stg18/turtle_textures/turtle_object.inc"
};
static NJS_MATERIAL kame_ground_solid_materials[1] = {
#include "stg18/turtle_textures/kame_ground_solid_materials.inc"
};
static Sint16 kame_ground_solid_strips[55] = {
#include "stg18/turtle_textures/kame_ground_solid_strips.inc"
};
static NJS_MESHSET kame_ground_solid_faces[1] = {
#include "stg18/turtle_textures/kame_ground_solid_faces.inc"
};
static NJS_POINT3 kame_ground_solid_points[22] = {
#include "stg18/turtle_textures/kame_ground_solid_points.inc"
};
static NJS_POINT3 kame_ground_solid_normals[22] = {
#include "stg18/turtle_textures/kame_ground_solid_normals.inc"
};
static NJS_MODEL kame_ground_solid_model = {
#include "stg18/turtle_textures/kame_ground_solid_model.inc"
};
NJS_OBJECT turtle_ground[1] = {
#include "stg18/turtle_textures/turtle_ground.inc"
};
static NJS_TEXNAME kame_eye_0_textures[1] = {
#include "stg18/turtle_textures/kame_eye_0_textures.inc"
};
static NJS_TEXLIST kame_eye_0_texlist = {
#include "stg18/turtle_textures/kame_eye_0_texlist.inc"
};
static Sint16 kame_eye_0_polygons[165] = {
#include "stg18/turtle_textures/kame_eye_0_polygons.inc"
};
static Sint32 kame_eye_0_vertices[83] = {
#include "stg18/turtle_textures/kame_eye_0_vertices.inc"
};
NJS_CNK_MODEL turtle_eye_model_0 = {
#include "stg18/turtle_textures/turtle_eye_model_0.inc"
};
static NJS_CNK_OBJECT kame_eye_0_object = {
#include "stg18/turtle_textures/kame_eye_0_object.inc"
};
static NJS_TEXNAME kame_eye_1_textures[1] = {
#include "stg18/turtle_textures/kame_eye_1_textures.inc"
};
static NJS_TEXLIST kame_eye_1_texlist = {
#include "stg18/turtle_textures/kame_eye_1_texlist.inc"
};
static Sint16 kame_eye_1_polygons[165] = {
#include "stg18/turtle_textures/kame_eye_1_polygons.inc"
};
static Sint32 kame_eye_1_vertices[83] = {
#include "stg18/turtle_textures/kame_eye_1_vertices.inc"
};
NJS_CNK_MODEL turtle_eye_model_1 = {
#include "stg18/turtle_textures/turtle_eye_model_1.inc"
};
static NJS_CNK_OBJECT kame_eye_1_object = {
#include "stg18/turtle_textures/kame_eye_1_object.inc"
};
static NJS_TEXNAME kame_eye_2_textures[1] = {
#include "stg18/turtle_textures/kame_eye_2_textures.inc"
};
static NJS_TEXLIST kame_eye_2_texlist = {
#include "stg18/turtle_textures/kame_eye_2_texlist.inc"
};
static Sint16 kame_eye_2_polygons[165] = {
#include "stg18/turtle_textures/kame_eye_2_polygons.inc"
};
static Sint32 kame_eye_2_vertices[83] = {
#include "stg18/turtle_textures/kame_eye_2_vertices.inc"
};
NJS_CNK_MODEL turtle_eye_model_2 = {
#include "stg18/turtle_textures/turtle_eye_model_2.inc"
};
static NJS_CNK_OBJECT kame_eye_2_object = {
#include "stg18/turtle_textures/kame_eye_2_object.inc"
};
static NJS_TEXNAME kame_eye_3_textures[1] = {
#include "stg18/turtle_textures/kame_eye_3_textures.inc"
};
static NJS_TEXLIST kame_eye_3_texlist = {
#include "stg18/turtle_textures/kame_eye_3_texlist.inc"
};
static Sint16 kame_eye_3_polygons[165] = {
#include "stg18/turtle_textures/kame_eye_3_polygons.inc"
};
static Sint32 kame_eye_3_vertices[83] = {
#include "stg18/turtle_textures/kame_eye_3_vertices.inc"
};
NJS_CNK_MODEL turtle_eye_model_3 = {
#include "stg18/turtle_textures/turtle_eye_model_3.inc"
};
static NJS_CNK_OBJECT kame_eye_3_object = {
#include "stg18/turtle_textures/kame_eye_3_object.inc"
};
static NJS_TEXNAME kame_eye_4_textures[1] = {
#include "stg18/turtle_textures/kame_eye_4_textures.inc"
};
static NJS_TEXLIST kame_eye_4_texlist = {
#include "stg18/turtle_textures/kame_eye_4_texlist.inc"
};
static Sint16 kame_eye_4_polygons[165] = {
#include "stg18/turtle_textures/kame_eye_4_polygons.inc"
};
static Sint32 kame_eye_4_vertices[83] = {
#include "stg18/turtle_textures/kame_eye_4_vertices.inc"
};
NJS_CNK_MODEL turtle_eye_model_4 = {
#include "stg18/turtle_textures/turtle_eye_model_4.inc"
};
static NJS_CNK_OBJECT kame_eye_4_object = {
#include "stg18/turtle_textures/kame_eye_4_object.inc"
};
static NJS_MKEY_F turtle_swim_motion_joint_0_position[120] = {
#include "stg18/turtle_textures/turtle_swim_motion_joint_0_position.inc"
};
static NJS_MKEY_A turtle_swim_motion_joint_0_angles[88] = {
#include "stg18/turtle_textures/turtle_swim_motion_joint_0_angles.inc"
};
static NJS_MKEY_A turtle_swim_motion_joint_1_angles[120] = {
#include "stg18/turtle_textures/turtle_swim_motion_joint_1_angles.inc"
};
static NJS_MKEY_A turtle_swim_motion_joint_2_angles[120] = {
#include "stg18/turtle_textures/turtle_swim_motion_joint_2_angles.inc"
};
static NJS_MKEY_A turtle_swim_motion_joint_3_angles[120] = {
#include "stg18/turtle_textures/turtle_swim_motion_joint_3_angles.inc"
};
static NJS_MKEY_A turtle_swim_motion_joint_4_angles[98] = {
#include "stg18/turtle_textures/turtle_swim_motion_joint_4_angles.inc"
};
static NJS_MKEY_A turtle_swim_motion_joint_5_angles[120] = {
#include "stg18/turtle_textures/turtle_swim_motion_joint_5_angles.inc"
};
static NJS_MDATA2 turtle_swim_motion_tracks[6] = {
#include "stg18/turtle_textures/turtle_swim_motion_tracks.inc"
};
NJS_MOTION turtle_swim_motion = {
#include "stg18/turtle_textures/turtle_swim_motion.inc"
};
static NJS_MKEY_F turtle_stop_motion_joint_0_position[121] = {
#include "stg18/turtle_textures/turtle_stop_motion_joint_0_position.inc"
};
static NJS_MKEY_A turtle_stop_motion_joint_0_angles[73] = {
#include "stg18/turtle_textures/turtle_stop_motion_joint_0_angles.inc"
};
static NJS_MKEY_A turtle_stop_motion_joint_1_angles[92] = {
#include "stg18/turtle_textures/turtle_stop_motion_joint_1_angles.inc"
};
static NJS_MKEY_A turtle_stop_motion_joint_2_angles[93] = {
#include "stg18/turtle_textures/turtle_stop_motion_joint_2_angles.inc"
};
static NJS_MKEY_A turtle_stop_motion_joint_3_angles[107] = {
#include "stg18/turtle_textures/turtle_stop_motion_joint_3_angles.inc"
};
static NJS_MKEY_A turtle_stop_motion_joint_4_angles[96] = {
#include "stg18/turtle_textures/turtle_stop_motion_joint_4_angles.inc"
};
static NJS_MKEY_A turtle_stop_motion_joint_5_angles[107] = {
#include "stg18/turtle_textures/turtle_stop_motion_joint_5_angles.inc"
};
static NJS_MDATA2 turtle_stop_motion_tracks[6] = {
#include "stg18/turtle_textures/turtle_stop_motion_tracks.inc"
};
NJS_MOTION turtle_stop_motion = {
#include "stg18/turtle_textures/turtle_stop_motion.inc"
};
static NJS_MKEY_F turtle_wait_motion_joint_0_position[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_0_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_joint_0_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_0_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_joint_1_position[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_1_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_joint_1_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_1_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_joint_2_position[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_2_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_joint_2_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_2_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_joint_3_position[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_3_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_joint_3_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_3_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_joint_4_position[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_4_position.inc"
};
static NJS_MKEY_F turtle_wait_motion_joint_5_position[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_5_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_joint_5_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_joint_5_angles.inc"
};
static NJS_MDATA2 turtle_wait_motion_tracks[6] = {
#include "stg18/turtle_textures/turtle_wait_motion_tracks.inc"
};
NJS_MOTION turtle_wait_motion = {
#include "stg18/turtle_textures/turtle_wait_motion.inc"
};
static NJS_MKEY_F turtle_wait_motion_2_joint_0_position[60] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_0_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_2_joint_0_angles[60] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_0_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_2_joint_1_position[60] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_1_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_2_joint_1_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_1_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_2_joint_2_position[60] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_2_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_2_joint_2_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_2_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_2_joint_3_position[60] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_3_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_2_joint_3_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_3_angles.inc"
};
static NJS_MKEY_F turtle_wait_motion_2_joint_4_position[60] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_4_position.inc"
};
static NJS_MKEY_F turtle_wait_motion_2_joint_5_position[60] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_5_position.inc"
};
static NJS_MKEY_A turtle_wait_motion_2_joint_5_angles[2] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_joint_5_angles.inc"
};
static NJS_MDATA2 turtle_wait_motion_2_tracks[6] = {
#include "stg18/turtle_textures/turtle_wait_motion_2_tracks.inc"
};
NJS_MOTION turtle_wait_motion_2 = {
#include "stg18/turtle_textures/turtle_wait_motion_2.inc"
};
static NJS_MKEY_F turtle_arrive_motion_joint_0_position[432] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_0_position.inc"
};
static NJS_MKEY_A turtle_arrive_motion_joint_0_angles[333] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_0_angles.inc"
};
static NJS_MKEY_F turtle_arrive_motion_joint_1_position[329] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_1_position.inc"
};
static NJS_MKEY_A turtle_arrive_motion_joint_1_angles[432] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_1_angles.inc"
};
static NJS_MKEY_F turtle_arrive_motion_joint_2_position[338] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_2_position.inc"
};
static NJS_MKEY_A turtle_arrive_motion_joint_2_angles[403] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_2_angles.inc"
};
static NJS_MKEY_F turtle_arrive_motion_joint_3_position[415] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_3_position.inc"
};
static NJS_MKEY_A turtle_arrive_motion_joint_3_angles[433] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_3_angles.inc"
};
static NJS_MKEY_F turtle_arrive_motion_joint_4_position[306] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_4_position.inc"
};
static NJS_MKEY_A turtle_arrive_motion_joint_4_angles[417] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_4_angles.inc"
};
static NJS_MKEY_F turtle_arrive_motion_joint_5_position[415] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_5_position.inc"
};
static NJS_MKEY_A turtle_arrive_motion_joint_5_angles[436] = {
#include "stg18/turtle_textures/turtle_arrive_motion_joint_5_angles.inc"
};
static NJS_MDATA2 turtle_arrive_motion_tracks[6] = {
#include "stg18/turtle_textures/turtle_arrive_motion_tracks.inc"
};
NJS_MOTION turtle_arrive_motion = {
#include "stg18/turtle_textures/turtle_arrive_motion.inc"
};
NJS_POINT3 turtle_path_points[141] = {
#include "stg18/turtle_textures/turtle_path_points.inc"
};
