#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "shared/path_enemy_art.h"
#include "stage/path.h"
#include "system/angle.h"

#include "shared/path_enemy_art/text.inc"

/* the path enemies' six texture names, each on a four-byte boundary */
const char patrol_texture_filenames[62] =
    TEXT_1 TEXT_2 TEXT_3 TEXT_4 TEXT_5 TEXT_6;

/* the texture lists, chunk models, motions, paths, chunk model pointers,
 * path list and enemy actions that 25 images place, from the disc. its D
 * precedes path_enemy.c */
static NJS_TEXNAME path_kumi_textures[4] = {
#include "shared/path_enemy_art/path_kumi_textures.inc"
};
NJS_TEXLIST path_kumi_texlist = {
#include "shared/path_enemy_art/path_kumi_texlist.inc"
};
static NJS_TEXNAME path_kyoko_textures[5] = {
#include "shared/path_enemy_art/path_kyoko_textures.inc"
};
NJS_TEXLIST path_kyoko_texlist = {
#include "shared/path_enemy_art/path_kyoko_texlist.inc"
};
static Sint16 path_bombkumi_joint_3_polygons[357] = {
#include "shared/path_enemy_art/path_bombkumi_joint_3_polygons.inc"
};
static Sint32 path_bombkumi_joint_3_vertices[213] = {
#include "shared/path_enemy_art/path_bombkumi_joint_3_vertices.inc"
};
static NJS_CNK_MODEL path_bombkumi_joint_3_model = {
#include "shared/path_enemy_art/path_bombkumi_joint_3_model.inc"
};
static NJS_CNK_OBJECT path_bombkumi_joint_3 = {
#include "shared/path_enemy_art/path_bombkumi_joint_3.inc"
};
static Sint16 path_bombkumi_joint_2_polygons[631] = {
#include "shared/path_enemy_art/path_bombkumi_joint_2_polygons.inc"
};
static Sint32 path_bombkumi_joint_2_vertices[327] = {
#include "shared/path_enemy_art/path_bombkumi_joint_2_vertices.inc"
};
static NJS_CNK_MODEL path_bombkumi_joint_2_model = {
#include "shared/path_enemy_art/path_bombkumi_joint_2_model.inc"
};
static NJS_CNK_OBJECT path_bombkumi_joint_2 = {
#include "shared/path_enemy_art/path_bombkumi_joint_2.inc"
};
static Sint16 path_bombkumi_joint_1_polygons[359] = {
#include "shared/path_enemy_art/path_bombkumi_joint_1_polygons.inc"
};
static Sint32 path_bombkumi_joint_1_vertices[255] = {
#include "shared/path_enemy_art/path_bombkumi_joint_1_vertices.inc"
};
static NJS_CNK_MODEL path_bombkumi_joint_1_model = {
#include "shared/path_enemy_art/path_bombkumi_joint_1_model.inc"
};
static NJS_CNK_OBJECT path_bombkumi_joint_1 = {
#include "shared/path_enemy_art/path_bombkumi_joint_1.inc"
};
NJS_CNK_OBJECT path_bombkumi_object[1] = {
#include "shared/path_enemy_art/path_bombkumi_object.inc"
};
static Sint16 path_gunkumi_joint_4_polygons[641] = {
#include "shared/path_enemy_art/path_gunkumi_joint_4_polygons.inc"
};
static Sint32 path_gunkumi_joint_4_vertices[333] = {
#include "shared/path_enemy_art/path_gunkumi_joint_4_vertices.inc"
};
static NJS_CNK_MODEL path_gunkumi_joint_4_model = {
#include "shared/path_enemy_art/path_gunkumi_joint_4_model.inc"
};
static NJS_CNK_OBJECT path_gunkumi_joint_4 = {
#include "shared/path_enemy_art/path_gunkumi_joint_4.inc"
};
static Sint16 path_gunkumi_joint_3_polygons[217] = {
#include "shared/path_enemy_art/path_gunkumi_joint_3_polygons.inc"
};
static Sint32 path_gunkumi_joint_3_vertices[153] = {
#include "shared/path_enemy_art/path_gunkumi_joint_3_vertices.inc"
};
static NJS_CNK_MODEL path_gunkumi_joint_3_model = {
#include "shared/path_enemy_art/path_gunkumi_joint_3_model.inc"
};
static NJS_CNK_OBJECT path_gunkumi_joint_3 = {
#include "shared/path_enemy_art/path_gunkumi_joint_3.inc"
};
static Sint16 path_gunkumi_joint_2_polygons[465] = {
#include "shared/path_enemy_art/path_gunkumi_joint_2_polygons.inc"
};
static Sint32 path_gunkumi_joint_2_vertices[309] = {
#include "shared/path_enemy_art/path_gunkumi_joint_2_vertices.inc"
};
static NJS_CNK_MODEL path_gunkumi_joint_2_model = {
#include "shared/path_enemy_art/path_gunkumi_joint_2_model.inc"
};
static NJS_CNK_OBJECT path_gunkumi_joint_2 = {
#include "shared/path_enemy_art/path_gunkumi_joint_2.inc"
};
static Sint16 path_gunkumi_joint_1_polygons[359] = {
#include "shared/path_enemy_art/path_gunkumi_joint_1_polygons.inc"
};
static Sint32 path_gunkumi_joint_1_vertices[255] = {
#include "shared/path_enemy_art/path_gunkumi_joint_1_vertices.inc"
};
static NJS_CNK_MODEL path_gunkumi_joint_1_model = {
#include "shared/path_enemy_art/path_gunkumi_joint_1_model.inc"
};
static NJS_CNK_OBJECT path_gunkumi_joint_1 = {
#include "shared/path_enemy_art/path_gunkumi_joint_1.inc"
};
NJS_CNK_OBJECT path_gunkumi_object[1] = {
#include "shared/path_enemy_art/path_gunkumi_object.inc"
};
static Sint16 path_kyoko_joint_10_polygons[101] = {
#include "shared/path_enemy_art/path_kyoko_joint_10_polygons.inc"
};
static Sint32 path_kyoko_joint_10_vertices[75] = {
#include "shared/path_enemy_art/path_kyoko_joint_10_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_10_model = {
#include "shared/path_enemy_art/path_kyoko_joint_10_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_10 = {
#include "shared/path_enemy_art/path_kyoko_joint_10.inc"
};
static Sint16 path_kyoko_joint_9_polygons[157] = {
#include "shared/path_enemy_art/path_kyoko_joint_9_polygons.inc"
};
static Sint32 path_kyoko_joint_9_vertices[117] = {
#include "shared/path_enemy_art/path_kyoko_joint_9_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_9_model = {
#include "shared/path_enemy_art/path_kyoko_joint_9_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_9 = {
#include "shared/path_enemy_art/path_kyoko_joint_9.inc"
};
static Sint16 path_kyoko_joint_8_polygons[163] = {
#include "shared/path_enemy_art/path_kyoko_joint_8_polygons.inc"
};
static Sint32 path_kyoko_joint_8_vertices[117] = {
#include "shared/path_enemy_art/path_kyoko_joint_8_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_8_model = {
#include "shared/path_enemy_art/path_kyoko_joint_8_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_8 = {
#include "shared/path_enemy_art/path_kyoko_joint_8.inc"
};
static Sint16 path_kyoko_joint_7_polygons[101] = {
#include "shared/path_enemy_art/path_kyoko_joint_7_polygons.inc"
};
static Sint32 path_kyoko_joint_7_vertices[75] = {
#include "shared/path_enemy_art/path_kyoko_joint_7_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_7_model = {
#include "shared/path_enemy_art/path_kyoko_joint_7_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_7 = {
#include "shared/path_enemy_art/path_kyoko_joint_7.inc"
};
static Sint16 path_kyoko_joint_6_polygons[353] = {
#include "shared/path_enemy_art/path_kyoko_joint_6_polygons.inc"
};
static Sint32 path_kyoko_joint_6_vertices[267] = {
#include "shared/path_enemy_art/path_kyoko_joint_6_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_6_model = {
#include "shared/path_enemy_art/path_kyoko_joint_6_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_6 = {
#include "shared/path_enemy_art/path_kyoko_joint_6.inc"
};
static Sint16 path_kyoko_joint_5_polygons[57] = {
#include "shared/path_enemy_art/path_kyoko_joint_5_polygons.inc"
};
static Sint32 path_kyoko_joint_5_vertices[39] = {
#include "shared/path_enemy_art/path_kyoko_joint_5_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_5_model = {
#include "shared/path_enemy_art/path_kyoko_joint_5_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_5 = {
#include "shared/path_enemy_art/path_kyoko_joint_5.inc"
};
static Sint16 path_kyoko_joint_4_polygons[57] = {
#include "shared/path_enemy_art/path_kyoko_joint_4_polygons.inc"
};
static Sint32 path_kyoko_joint_4_vertices[39] = {
#include "shared/path_enemy_art/path_kyoko_joint_4_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_4_model = {
#include "shared/path_enemy_art/path_kyoko_joint_4_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_4 = {
#include "shared/path_enemy_art/path_kyoko_joint_4.inc"
};
static Sint16 path_kyoko_joint_3_polygons[353] = {
#include "shared/path_enemy_art/path_kyoko_joint_3_polygons.inc"
};
static Sint32 path_kyoko_joint_3_vertices[267] = {
#include "shared/path_enemy_art/path_kyoko_joint_3_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_3_model = {
#include "shared/path_enemy_art/path_kyoko_joint_3_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_3 = {
#include "shared/path_enemy_art/path_kyoko_joint_3.inc"
};
static Sint16 path_kyoko_joint_2_polygons[65] = {
#include "shared/path_enemy_art/path_kyoko_joint_2_polygons.inc"
};
static Sint32 path_kyoko_joint_2_vertices[75] = {
#include "shared/path_enemy_art/path_kyoko_joint_2_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_2_model = {
#include "shared/path_enemy_art/path_kyoko_joint_2_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_2 = {
#include "shared/path_enemy_art/path_kyoko_joint_2.inc"
};
static Sint16 path_kyoko_joint_1_polygons[207] = {
#include "shared/path_enemy_art/path_kyoko_joint_1_polygons.inc"
};
static Sint32 path_kyoko_joint_1_vertices[123] = {
#include "shared/path_enemy_art/path_kyoko_joint_1_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_joint_1_model = {
#include "shared/path_enemy_art/path_kyoko_joint_1_model.inc"
};
static NJS_CNK_OBJECT path_kyoko_joint_1 = {
#include "shared/path_enemy_art/path_kyoko_joint_1.inc"
};
static Sint16 path_kyoko_object_polygons[445] = {
#include "shared/path_enemy_art/path_kyoko_object_polygons.inc"
};
static Sint32 path_kyoko_object_vertices[261] = {
#include "shared/path_enemy_art/path_kyoko_object_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_object_model = {
#include "shared/path_enemy_art/path_kyoko_object_model.inc"
};
NJS_CNK_OBJECT path_kyoko_object[1] = {
#include "shared/path_enemy_art/path_kyoko_object.inc"
};
static Sint16 path_backkyoko_joint_10_polygons[101] = {
#include "shared/path_enemy_art/path_backkyoko_joint_10_polygons.inc"
};
static Sint32 path_backkyoko_joint_10_vertices[75] = {
#include "shared/path_enemy_art/path_backkyoko_joint_10_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_10_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_10_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_10 = {
#include "shared/path_enemy_art/path_backkyoko_joint_10.inc"
};
static Sint16 path_backkyoko_joint_9_polygons[157] = {
#include "shared/path_enemy_art/path_backkyoko_joint_9_polygons.inc"
};
static Sint32 path_backkyoko_joint_9_vertices[117] = {
#include "shared/path_enemy_art/path_backkyoko_joint_9_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_9_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_9_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_9 = {
#include "shared/path_enemy_art/path_backkyoko_joint_9.inc"
};
static Sint16 path_backkyoko_joint_8_polygons[163] = {
#include "shared/path_enemy_art/path_backkyoko_joint_8_polygons.inc"
};
static Sint32 path_backkyoko_joint_8_vertices[117] = {
#include "shared/path_enemy_art/path_backkyoko_joint_8_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_8_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_8_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_8 = {
#include "shared/path_enemy_art/path_backkyoko_joint_8.inc"
};
static Sint16 path_backkyoko_joint_7_polygons[101] = {
#include "shared/path_enemy_art/path_backkyoko_joint_7_polygons.inc"
};
static Sint32 path_backkyoko_joint_7_vertices[75] = {
#include "shared/path_enemy_art/path_backkyoko_joint_7_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_7_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_7_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_7 = {
#include "shared/path_enemy_art/path_backkyoko_joint_7.inc"
};
static Sint16 path_backkyoko_joint_6_polygons[353] = {
#include "shared/path_enemy_art/path_backkyoko_joint_6_polygons.inc"
};
static Sint32 path_backkyoko_joint_6_vertices[267] = {
#include "shared/path_enemy_art/path_backkyoko_joint_6_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_6_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_6_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_6 = {
#include "shared/path_enemy_art/path_backkyoko_joint_6.inc"
};
static Sint16 path_backkyoko_joint_5_polygons[353] = {
#include "shared/path_enemy_art/path_backkyoko_joint_5_polygons.inc"
};
static Sint32 path_backkyoko_joint_5_vertices[267] = {
#include "shared/path_enemy_art/path_backkyoko_joint_5_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_5_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_5_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_5 = {
#include "shared/path_enemy_art/path_backkyoko_joint_5.inc"
};
static Sint16 path_backkyoko_joint_4_polygons[57] = {
#include "shared/path_enemy_art/path_backkyoko_joint_4_polygons.inc"
};
static Sint32 path_backkyoko_joint_4_vertices[39] = {
#include "shared/path_enemy_art/path_backkyoko_joint_4_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_4_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_4_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_4 = {
#include "shared/path_enemy_art/path_backkyoko_joint_4.inc"
};
static Sint16 path_backkyoko_joint_3_polygons[57] = {
#include "shared/path_enemy_art/path_backkyoko_joint_3_polygons.inc"
};
static Sint32 path_backkyoko_joint_3_vertices[39] = {
#include "shared/path_enemy_art/path_backkyoko_joint_3_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_3_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_3_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_3 = {
#include "shared/path_enemy_art/path_backkyoko_joint_3.inc"
};
static Sint16 path_backkyoko_joint_2_polygons[65] = {
#include "shared/path_enemy_art/path_backkyoko_joint_2_polygons.inc"
};
static Sint32 path_backkyoko_joint_2_vertices[75] = {
#include "shared/path_enemy_art/path_backkyoko_joint_2_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_2_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_2_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_2 = {
#include "shared/path_enemy_art/path_backkyoko_joint_2.inc"
};
static Sint16 path_backkyoko_joint_1_polygons[207] = {
#include "shared/path_enemy_art/path_backkyoko_joint_1_polygons.inc"
};
static Sint32 path_backkyoko_joint_1_vertices[123] = {
#include "shared/path_enemy_art/path_backkyoko_joint_1_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_joint_1_model = {
#include "shared/path_enemy_art/path_backkyoko_joint_1_model.inc"
};
static NJS_CNK_OBJECT path_backkyoko_joint_1 = {
#include "shared/path_enemy_art/path_backkyoko_joint_1.inc"
};
static Sint16 path_backkyoko_object_polygons[445] = {
#include "shared/path_enemy_art/path_backkyoko_object_polygons.inc"
};
static Sint32 path_backkyoko_object_vertices[261] = {
#include "shared/path_enemy_art/path_backkyoko_object_vertices.inc"
};
static NJS_CNK_MODEL path_backkyoko_object_model = {
#include "shared/path_enemy_art/path_backkyoko_object_model.inc"
};
NJS_CNK_OBJECT path_backkyoko_object[1] = {
#include "shared/path_enemy_art/path_backkyoko_object.inc"
};
static Sint16 path_kyoko_shadow_object_polygons[53] = {
#include "shared/path_enemy_art/path_kyoko_shadow_object_polygons.inc"
};
static Sint32 path_kyoko_shadow_object_vertices[33] = {
#include "shared/path_enemy_art/path_kyoko_shadow_object_vertices.inc"
};
static NJS_CNK_MODEL path_kyoko_shadow_object_model = {
#include "shared/path_enemy_art/path_kyoko_shadow_object_model.inc"
};
NJS_CNK_OBJECT path_kyoko_shadow_object = {
#include "shared/path_enemy_art/path_kyoko_shadow_object.inc"
};
static NJS_MKEY_F patrol_bombkumi_motion_joint_2_position[3] = {
#include "shared/path_enemy_art/patrol_bombkumi_motion_joint_2_position.inc"
};
static NJS_MDATA2 patrol_bombkumi_motion_tracks[4] = {
#include "shared/path_enemy_art/patrol_bombkumi_motion_tracks.inc"
};
static NJS_MOTION patrol_bombkumi_motion = {
#include "shared/path_enemy_art/patrol_bombkumi_motion.inc"
};
static NJS_MKEY_F patrol_gunkumi_fire_motion_joint_2_position[5] = {
#include "shared/path_enemy_art/patrol_gunkumi_fire_motion_joint_2_position.inc"
};
static NJS_MKEY_F patrol_gunkumi_fire_motion_joint_3_position[5] = {
#include "shared/path_enemy_art/patrol_gunkumi_fire_motion_joint_3_position.inc"
};
static NJS_MKEY_F patrol_gunkumi_fire_motion_joint_4_position[5] = {
#include "shared/path_enemy_art/patrol_gunkumi_fire_motion_joint_4_position.inc"
};
static NJS_MDATA2 patrol_gunkumi_fire_motion_tracks[5] = {
#include "shared/path_enemy_art/patrol_gunkumi_fire_motion_tracks.inc"
};
static NJS_MOTION patrol_gunkumi_fire_motion = {
#include "shared/path_enemy_art/patrol_gunkumi_fire_motion.inc"
};
static NJS_MKEY_F patrol_gunkumi_motion_joint_4_position[4] = {
#include "shared/path_enemy_art/patrol_gunkumi_motion_joint_4_position.inc"
};
static NJS_MDATA2 patrol_gunkumi_motion_tracks[5] = {
#include "shared/path_enemy_art/patrol_gunkumi_motion_tracks.inc"
};
static NJS_MOTION patrol_gunkumi_motion = {
#include "shared/path_enemy_art/patrol_gunkumi_motion.inc"
};
static NJS_MKEY_F patrol_kyoko_motion_joint_0_position[4] = {
#include "shared/path_enemy_art/patrol_kyoko_motion_joint_0_position.inc"
};
static NJS_MKEY_F patrol_kyoko_motion_joint_1_position[4] = {
#include "shared/path_enemy_art/patrol_kyoko_motion_joint_1_position.inc"
};
static NJS_MKEY_F patrol_kyoko_motion_joint_2_position[4] = {
#include "shared/path_enemy_art/patrol_kyoko_motion_joint_2_position.inc"
};
static NJS_MDATA2 patrol_kyoko_motion_tracks[11] = {
#include "shared/path_enemy_art/patrol_kyoko_motion_tracks.inc"
};
static NJS_MOTION patrol_kyoko_motion = {
#include "shared/path_enemy_art/patrol_kyoko_motion.inc"
};
static NJS_MKEY_F patrol_backkyoko_motion_joint_0_position[4] = {
#include "shared/path_enemy_art/patrol_backkyoko_motion_joint_0_position.inc"
};
static NJS_MKEY_F patrol_backkyoko_motion_joint_1_position[4] = {
#include "shared/path_enemy_art/patrol_backkyoko_motion_joint_1_position.inc"
};
static NJS_MKEY_F patrol_backkyoko_motion_joint_2_position[4] = {
#include "shared/path_enemy_art/patrol_backkyoko_motion_joint_2_position.inc"
};
static NJS_MDATA2 patrol_backkyoko_motion_tracks[11] = {
#include "shared/path_enemy_art/patrol_backkyoko_motion_tracks.inc"
};
static NJS_MOTION patrol_backkyoko_motion = {
#include "shared/path_enemy_art/patrol_backkyoko_motion.inc"
};
static struct path_point route_0_points[17] = {
#include "shared/path_enemy_art/route_0_points.inc"
};
static struct path route_0 = {
#include "shared/path_enemy_art/route_0.inc"
};
static struct path_point route_1_points[13] = {
#include "shared/path_enemy_art/route_1_points.inc"
};
static struct path route_1 = {
#include "shared/path_enemy_art/route_1.inc"
};
static struct path_point route_2_points[27] = {
#include "shared/path_enemy_art/route_2_points.inc"
};
static struct path route_2 = {
#include "shared/path_enemy_art/route_2.inc"
};
static struct path_point route_3_points[16] = {
#include "shared/path_enemy_art/route_3_points.inc"
};
static struct path route_3 = {
#include "shared/path_enemy_art/route_3.inc"
};
static struct path_point route_4_points[67] = {
#include "shared/path_enemy_art/route_4_points.inc"
};
static struct path route_4 = {
#include "shared/path_enemy_art/route_4.inc"
};
static struct path_point route_5_points[32] = {
#include "shared/path_enemy_art/route_5_points.inc"
};
static struct path route_5 = {
#include "shared/path_enemy_art/route_5.inc"
};
static struct path_point route_6_points[4] = {
#include "shared/path_enemy_art/route_6_points.inc"
};
static struct path route_6 = {
#include "shared/path_enemy_art/route_6.inc"
};
static struct path_point route_7_points[11] = {
#include "shared/path_enemy_art/route_7_points.inc"
};
static struct path route_7 = {
#include "shared/path_enemy_art/route_7.inc"
};
static struct path_point route_8_points[25] = {
#include "shared/path_enemy_art/route_8_points.inc"
};
static struct path route_8 = {
#include "shared/path_enemy_art/route_8.inc"
};
static struct path_point route_9_points[17] = {
#include "shared/path_enemy_art/route_9_points.inc"
};
static struct path route_9 = {
#include "shared/path_enemy_art/route_9.inc"
};
static struct path_point route_10_points[11] = {
#include "shared/path_enemy_art/route_10_points.inc"
};
static struct path route_10 = {
#include "shared/path_enemy_art/route_10.inc"
};
static struct path_point route_11_points[27] = {
#include "shared/path_enemy_art/route_11_points.inc"
};
static struct path route_11 = {
#include "shared/path_enemy_art/route_11.inc"
};
/* clang-format off */
struct collision_shape path_enemy_shapes[6] = {
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_UNTURNED,
     {0.0F, 0.0F, 0.0F}, 6.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE,
     {0.0F, 13.0F, 0.0F}, 10.0F, 14.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE,
     {-6.0F, 12.0F, 13.0F}, 9.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE,
     {-6.0F, 12.0F, -13.0F}, 9.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX2, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE,
     {-15.0F, 23.0F, 19.0F}, 12.0F, 2.0F, 7.0F, 0, {0, EIGHTH_TURN, 0}},
    {0, SHAPE_BOX2, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE,
     {-15.0F, 23.0F, -19.0F}, 12.0F, 2.0F, 7.0F, 0,
     {0, FULL_TURN - EIGHTH_TURN, 0}},
};
/* clang-format on */
NJS_CNK_MODEL *path_gunkumi_pieces[5] = {
#include "shared/path_enemy_art/path_gunkumi_pieces.inc"
};
NJS_CNK_MODEL *path_bombkumi_pieces[4] = {
#include "shared/path_enemy_art/path_bombkumi_pieces.inc"
};
NJS_CNK_MODEL *path_kyoko_pieces[12] = {
#include "shared/path_enemy_art/path_kyoko_pieces.inc"
};
NJS_CNK_MODEL *path_backkyoko_pieces[12] = {
#include "shared/path_enemy_art/path_backkyoko_pieces.inc"
};
struct path *path_enemy_paths[12] = {
#include "shared/path_enemy_art/path_enemy_paths.inc"
};
struct enemy_action path_enemy_actions[5] = {
#include "shared/path_enemy_art/path_enemy_actions.inc"
};
