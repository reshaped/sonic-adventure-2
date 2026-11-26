#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/enemy_emi_art.h"

const char emi_texture_filenames[67] = {
#include "shared/enemy_emi_art/emi_texture_filenames.inc"
};

static NJS_TEXNAME emi_textures[7] = {
#include "shared/enemy_emi_art/emi_textures.inc"
};

NJS_TEXLIST emi_texlist = {
#include "shared/enemy_emi_art/emi_texlist.inc"
};

/* the chunk models, motions, UV scrolls and texture lists that 26 images
 * place, from the disc. its D lies between this file's own D and enemy_emi.c */
static Sint16 emi_piece_0_polygons[165] = {
#include "shared/enemy_emi_art/emi_piece_0_polygons.inc"
};
static Sint32 emi_piece_0_vertices[147] = {
#include "shared/enemy_emi_art/emi_piece_0_vertices.inc"
};
NJS_CNK_MODEL emi_piece_0 = {
#include "shared/enemy_emi_art/emi_piece_0.inc"
};
static NJS_CNK_OBJECT emi_piece_0_joint = {
#include "shared/enemy_emi_art/emi_piece_0_joint.inc"
};
static Sint16 emi_piece_1_polygons[173] = {
#include "shared/enemy_emi_art/emi_piece_1_polygons.inc"
};
static Sint32 emi_piece_1_vertices[135] = {
#include "shared/enemy_emi_art/emi_piece_1_vertices.inc"
};
NJS_CNK_MODEL emi_piece_1 = {
#include "shared/enemy_emi_art/emi_piece_1.inc"
};
static NJS_CNK_OBJECT emi_piece_1_joint = {
#include "shared/enemy_emi_art/emi_piece_1_joint.inc"
};
static Sint16 emi_piece_2_polygons[223] = {
#include "shared/enemy_emi_art/emi_piece_2_polygons.inc"
};
static Sint32 emi_piece_2_vertices[147] = {
#include "shared/enemy_emi_art/emi_piece_2_vertices.inc"
};
NJS_CNK_MODEL emi_piece_2 = {
#include "shared/enemy_emi_art/emi_piece_2.inc"
};
static NJS_CNK_OBJECT emi_piece_2_joint = {
#include "shared/enemy_emi_art/emi_piece_2_joint.inc"
};
static Sint16 emi_piece_3_polygons[203] = {
#include "shared/enemy_emi_art/emi_piece_3_polygons.inc"
};
static Sint32 emi_piece_3_vertices[135] = {
#include "shared/enemy_emi_art/emi_piece_3_vertices.inc"
};
NJS_CNK_MODEL emi_piece_3 = {
#include "shared/enemy_emi_art/emi_piece_3.inc"
};
static NJS_CNK_OBJECT emi_piece_3_joint = {
#include "shared/enemy_emi_art/emi_piece_3_joint.inc"
};
static Sint16 emi_piece_4_polygons[131] = {
#include "shared/enemy_emi_art/emi_piece_4_polygons.inc"
};
static Sint32 emi_piece_4_vertices[99] = {
#include "shared/enemy_emi_art/emi_piece_4_vertices.inc"
};
NJS_CNK_MODEL emi_piece_4 = {
#include "shared/enemy_emi_art/emi_piece_4.inc"
};
static NJS_CNK_OBJECT emi_piece_4_joint = {
#include "shared/enemy_emi_art/emi_piece_4_joint.inc"
};
static Sint16 emi_scroll_model_polygons[183] = {
#include "shared/enemy_emi_art/emi_scroll_model_polygons.inc"
};
static Sint32 emi_scroll_model_vertices[171] = {
#include "shared/enemy_emi_art/emi_scroll_model_vertices.inc"
};
NJS_CNK_MODEL emi_scroll_model = {
#include "shared/enemy_emi_art/emi_scroll_model.inc"
};
NJS_CNK_OBJECT emi_object = {
#include "shared/enemy_emi_art/emi_object.inc"
};
static NJS_MKEY_F emi_motion_joint_1_position[2] = {
#include "shared/enemy_emi_art/emi_motion_joint_1_position.inc"
};
static NJS_MKEY_F emi_motion_joint_3_position[2] = {
#include "shared/enemy_emi_art/emi_motion_joint_3_position.inc"
};
static NJS_MKEY_F emi_motion_joint_4_position[2] = {
#include "shared/enemy_emi_art/emi_motion_joint_4_position.inc"
};
static NJS_MDATA2 emi_motion_tracks[6] = {
#include "shared/enemy_emi_art/emi_motion_tracks.inc"
};
NJS_MOTION emi_motion = {
#include "shared/enemy_emi_art/emi_motion.inc"
};
static Sint16 emi_tread_uv[60] = {
#include "shared/enemy_emi_art/emi_tread_uv.inc"
};
struct texture_anim emi_scroll_uv = {
#include "shared/enemy_emi_art/emi_scroll_uv.inc"
};
static NJS_TEXNAME emi_cannon_textures[7] = {
#include "shared/enemy_emi_art/emi_cannon_textures.inc"
};
NJS_TEXLIST emi_cannon_texlist = {
#include "shared/enemy_emi_art/emi_cannon_texlist.inc"
};
static Sint16 emi_cannon_piece_0_polygons[245] = {
#include "shared/enemy_emi_art/emi_cannon_piece_0_polygons.inc"
};
static Sint32 emi_cannon_piece_0_vertices[153] = {
#include "shared/enemy_emi_art/emi_cannon_piece_0_vertices.inc"
};
NJS_CNK_MODEL emi_cannon_piece_0 = {
#include "shared/enemy_emi_art/emi_cannon_piece_0.inc"
};
static NJS_CNK_OBJECT emi_cannon_piece_0_joint = {
#include "shared/enemy_emi_art/emi_cannon_piece_0_joint.inc"
};
static Sint16 emi_cannon_piece_1_polygons[165] = {
#include "shared/enemy_emi_art/emi_cannon_piece_1_polygons.inc"
};
static Sint32 emi_cannon_piece_1_vertices[147] = {
#include "shared/enemy_emi_art/emi_cannon_piece_1_vertices.inc"
};
NJS_CNK_MODEL emi_cannon_piece_1 = {
#include "shared/enemy_emi_art/emi_cannon_piece_1.inc"
};
static NJS_CNK_OBJECT emi_cannon_piece_1_joint = {
#include "shared/enemy_emi_art/emi_cannon_piece_1_joint.inc"
};
static Sint16 emi_cannon_piece_2_polygons[161] = {
#include "shared/enemy_emi_art/emi_cannon_piece_2_polygons.inc"
};
static Sint32 emi_cannon_piece_2_vertices[135] = {
#include "shared/enemy_emi_art/emi_cannon_piece_2_vertices.inc"
};
NJS_CNK_MODEL emi_cannon_piece_2 = {
#include "shared/enemy_emi_art/emi_cannon_piece_2.inc"
};
static NJS_CNK_OBJECT emi_cannon_piece_2_joint = {
#include "shared/enemy_emi_art/emi_cannon_piece_2_joint.inc"
};
static Sint16 emi_cannon_piece_3_polygons[223] = {
#include "shared/enemy_emi_art/emi_cannon_piece_3_polygons.inc"
};
static Sint32 emi_cannon_piece_3_vertices[147] = {
#include "shared/enemy_emi_art/emi_cannon_piece_3_vertices.inc"
};
NJS_CNK_MODEL emi_cannon_piece_3 = {
#include "shared/enemy_emi_art/emi_cannon_piece_3.inc"
};
static NJS_CNK_OBJECT emi_cannon_piece_3_joint = {
#include "shared/enemy_emi_art/emi_cannon_piece_3_joint.inc"
};
static Sint16 emi_cannon_piece_4_polygons[203] = {
#include "shared/enemy_emi_art/emi_cannon_piece_4_polygons.inc"
};
static Sint32 emi_cannon_piece_4_vertices[135] = {
#include "shared/enemy_emi_art/emi_cannon_piece_4_vertices.inc"
};
NJS_CNK_MODEL emi_cannon_piece_4 = {
#include "shared/enemy_emi_art/emi_cannon_piece_4.inc"
};
static NJS_CNK_OBJECT emi_cannon_piece_4_joint = {
#include "shared/enemy_emi_art/emi_cannon_piece_4_joint.inc"
};
static Sint16 emi_cannon_piece_5_polygons[119] = {
#include "shared/enemy_emi_art/emi_cannon_piece_5_polygons.inc"
};
static Sint32 emi_cannon_piece_5_vertices[87] = {
#include "shared/enemy_emi_art/emi_cannon_piece_5_vertices.inc"
};
NJS_CNK_MODEL emi_cannon_piece_5 = {
#include "shared/enemy_emi_art/emi_cannon_piece_5.inc"
};
static NJS_CNK_OBJECT emi_cannon_piece_5_joint = {
#include "shared/enemy_emi_art/emi_cannon_piece_5_joint.inc"
};
static Sint16 emi_cannon_scroll_model_polygons[183] = {
#include "shared/enemy_emi_art/emi_cannon_scroll_model_polygons.inc"
};
static Sint32 emi_cannon_scroll_model_vertices[171] = {
#include "shared/enemy_emi_art/emi_cannon_scroll_model_vertices.inc"
};
NJS_CNK_MODEL emi_cannon_scroll_model = {
#include "shared/enemy_emi_art/emi_cannon_scroll_model.inc"
};
NJS_CNK_OBJECT emi_cannon_object = {
#include "shared/enemy_emi_art/emi_cannon_object.inc"
};
static NJS_MKEY_F emi_cannon_shot_motion_joint_1_position[5] = {
#include "shared/enemy_emi_art/emi_cannon_shot_motion_joint_1_position.inc"
};
static NJS_MKEY_F emi_cannon_shot_motion_joint_3_position[5] = {
#include "shared/enemy_emi_art/emi_cannon_shot_motion_joint_3_position.inc"
};
static NJS_MKEY_F emi_cannon_shot_motion_joint_4_position[5] = {
#include "shared/enemy_emi_art/emi_cannon_shot_motion_joint_4_position.inc"
};
static NJS_MKEY_F emi_cannon_shot_motion_joint_6_position[5] = {
#include "shared/enemy_emi_art/emi_cannon_shot_motion_joint_6_position.inc"
};
static NJS_MKEY_A emi_cannon_shot_motion_joint_6_angles[5] = {
#include "shared/enemy_emi_art/emi_cannon_shot_motion_joint_6_angles.inc"
};
static NJS_MDATA2 emi_cannon_shot_motion_tracks[7] = {
#include "shared/enemy_emi_art/emi_cannon_shot_motion_tracks.inc"
};
NJS_MOTION emi_cannon_shot_motion = {
#include "shared/enemy_emi_art/emi_cannon_shot_motion.inc"
};
static NJS_MKEY_F emi_cannon_motion_joint_1_position[2] = {
#include "shared/enemy_emi_art/emi_cannon_motion_joint_1_position.inc"
};
static NJS_MKEY_F emi_cannon_motion_joint_3_position[2] = {
#include "shared/enemy_emi_art/emi_cannon_motion_joint_3_position.inc"
};
static NJS_MKEY_F emi_cannon_motion_joint_4_position[2] = {
#include "shared/enemy_emi_art/emi_cannon_motion_joint_4_position.inc"
};
static NJS_MDATA2 emi_cannon_motion_tracks[7] = {
#include "shared/enemy_emi_art/emi_cannon_motion_tracks.inc"
};
NJS_MOTION emi_cannon_motion = {
#include "shared/enemy_emi_art/emi_cannon_motion.inc"
};
static Sint16 emi_cannon_tread_uv[60] = {
#include "shared/enemy_emi_art/emi_cannon_tread_uv.inc"
};
struct texture_anim emi_cannon_scroll_uv = {
#include "shared/enemy_emi_art/emi_cannon_scroll_uv.inc"
};
static NJS_TEXNAME emi_armored_textures[7] = {
#include "shared/enemy_emi_art/emi_armored_textures.inc"
};
NJS_TEXLIST emi_armored_texlist = {
#include "shared/enemy_emi_art/emi_armored_texlist.inc"
};
static Sint16 emi_armored_piece_0_polygons[177] = {
#include "shared/enemy_emi_art/emi_armored_piece_0_polygons.inc"
};
static Sint32 emi_armored_piece_0_vertices[147] = {
#include "shared/enemy_emi_art/emi_armored_piece_0_vertices.inc"
};
NJS_CNK_MODEL emi_armored_piece_0 = {
#include "shared/enemy_emi_art/emi_armored_piece_0.inc"
};
static NJS_CNK_OBJECT emi_armored_piece_0_joint = {
#include "shared/enemy_emi_art/emi_armored_piece_0_joint.inc"
};
static Sint16 emi_armored_piece_1_polygons[153] = {
#include "shared/enemy_emi_art/emi_armored_piece_1_polygons.inc"
};
static Sint32 emi_armored_piece_1_vertices[147] = {
#include "shared/enemy_emi_art/emi_armored_piece_1_vertices.inc"
};
NJS_CNK_MODEL emi_armored_piece_1 = {
#include "shared/enemy_emi_art/emi_armored_piece_1.inc"
};
static NJS_CNK_OBJECT emi_armored_piece_1_joint = {
#include "shared/enemy_emi_art/emi_armored_piece_1_joint.inc"
};
static Sint16 emi_armored_piece_2_polygons[313] = {
#include "shared/enemy_emi_art/emi_armored_piece_2_polygons.inc"
};
static Sint32 emi_armored_piece_2_vertices[219] = {
#include "shared/enemy_emi_art/emi_armored_piece_2_vertices.inc"
};
NJS_CNK_MODEL emi_armored_piece_2 = {
#include "shared/enemy_emi_art/emi_armored_piece_2.inc"
};
static NJS_CNK_OBJECT emi_armored_piece_2_joint = {
#include "shared/enemy_emi_art/emi_armored_piece_2_joint.inc"
};
static Sint16 emi_armored_piece_3_polygons[137] = {
#include "shared/enemy_emi_art/emi_armored_piece_3_polygons.inc"
};
static Sint32 emi_armored_piece_3_vertices[99] = {
#include "shared/enemy_emi_art/emi_armored_piece_3_vertices.inc"
};
NJS_CNK_MODEL emi_armored_piece_3 = {
#include "shared/enemy_emi_art/emi_armored_piece_3.inc"
};
static NJS_CNK_OBJECT emi_armored_piece_3_joint = {
#include "shared/enemy_emi_art/emi_armored_piece_3_joint.inc"
};
static Sint16 emi_armored_piece_4_polygons[131] = {
#include "shared/enemy_emi_art/emi_armored_piece_4_polygons.inc"
};
static Sint32 emi_armored_piece_4_vertices[99] = {
#include "shared/enemy_emi_art/emi_armored_piece_4_vertices.inc"
};
NJS_CNK_MODEL emi_armored_piece_4 = {
#include "shared/enemy_emi_art/emi_armored_piece_4.inc"
};
static NJS_CNK_OBJECT emi_armored_piece_4_joint = {
#include "shared/enemy_emi_art/emi_armored_piece_4_joint.inc"
};
static Sint16 emi_armored_scroll_model_polygons[183] = {
#include "shared/enemy_emi_art/emi_armored_scroll_model_polygons.inc"
};
static Sint32 emi_armored_scroll_model_vertices[171] = {
#include "shared/enemy_emi_art/emi_armored_scroll_model_vertices.inc"
};
NJS_CNK_MODEL emi_armored_scroll_model = {
#include "shared/enemy_emi_art/emi_armored_scroll_model.inc"
};
NJS_CNK_OBJECT emi_armored_object = {
#include "shared/enemy_emi_art/emi_armored_object.inc"
};
static NJS_MKEY_F emi_armored_motion_joint_1_position[2] = {
#include "shared/enemy_emi_art/emi_armored_motion_joint_1_position.inc"
};
static NJS_MKEY_F emi_armored_motion_joint_3_position[2] = {
#include "shared/enemy_emi_art/emi_armored_motion_joint_3_position.inc"
};
static NJS_MDATA2 emi_armored_motion_tracks[6] = {
#include "shared/enemy_emi_art/emi_armored_motion_tracks.inc"
};
NJS_MOTION emi_armored_motion = {
#include "shared/enemy_emi_art/emi_armored_motion.inc"
};
static Sint16 emi_armored_tread_uv[60] = {
#include "shared/enemy_emi_art/emi_armored_tread_uv.inc"
};
struct texture_anim emi_armored_scroll_uv = {
#include "shared/enemy_emi_art/emi_armored_scroll_uv.inc"
};
static NJS_TEXNAME emi_spiked_textures[6] = {
#include "shared/enemy_emi_art/emi_spiked_textures.inc"
};
NJS_TEXLIST emi_spiked_texlist = {
#include "shared/enemy_emi_art/emi_spiked_texlist.inc"
};
static Sint16 emi_spiked_piece_0_polygons[175] = {
#include "shared/enemy_emi_art/emi_spiked_piece_0_polygons.inc"
};
static Sint32 emi_spiked_piece_0_vertices[147] = {
#include "shared/enemy_emi_art/emi_spiked_piece_0_vertices.inc"
};
NJS_CNK_MODEL emi_spiked_piece_0 = {
#include "shared/enemy_emi_art/emi_spiked_piece_0.inc"
};
static NJS_CNK_OBJECT emi_spiked_piece_0_joint = {
#include "shared/enemy_emi_art/emi_spiked_piece_0_joint.inc"
};
static Sint16 emi_spiked_piece_1_polygons[111] = {
#include "shared/enemy_emi_art/emi_spiked_piece_1_polygons.inc"
};
static Sint32 emi_spiked_piece_1_vertices[87] = {
#include "shared/enemy_emi_art/emi_spiked_piece_1_vertices.inc"
};
NJS_CNK_MODEL emi_spiked_piece_1 = {
#include "shared/enemy_emi_art/emi_spiked_piece_1.inc"
};
static NJS_CNK_OBJECT emi_spiked_piece_1_joint = {
#include "shared/enemy_emi_art/emi_spiked_piece_1_joint.inc"
};
static Sint16 emi_spiked_piece_2_polygons[333] = {
#include "shared/enemy_emi_art/emi_spiked_piece_2_polygons.inc"
};
static Sint32 emi_spiked_piece_2_vertices[183] = {
#include "shared/enemy_emi_art/emi_spiked_piece_2_vertices.inc"
};
NJS_CNK_MODEL emi_spiked_piece_2 = {
#include "shared/enemy_emi_art/emi_spiked_piece_2.inc"
};
static NJS_CNK_OBJECT emi_spiked_piece_2_joint = {
#include "shared/enemy_emi_art/emi_spiked_piece_2_joint.inc"
};
static Sint16 emi_spiked_piece_3_polygons[129] = {
#include "shared/enemy_emi_art/emi_spiked_piece_3_polygons.inc"
};
static Sint32 emi_spiked_piece_3_vertices[99] = {
#include "shared/enemy_emi_art/emi_spiked_piece_3_vertices.inc"
};
NJS_CNK_MODEL emi_spiked_piece_3 = {
#include "shared/enemy_emi_art/emi_spiked_piece_3.inc"
};
static NJS_CNK_OBJECT emi_spiked_piece_3_joint = {
#include "shared/enemy_emi_art/emi_spiked_piece_3_joint.inc"
};
static Sint16 emi_spiked_piece_4_polygons[301] = {
#include "shared/enemy_emi_art/emi_spiked_piece_4_polygons.inc"
};
static Sint32 emi_spiked_piece_4_vertices[267] = {
#include "shared/enemy_emi_art/emi_spiked_piece_4_vertices.inc"
};
NJS_CNK_MODEL emi_spiked_piece_4 = {
#include "shared/enemy_emi_art/emi_spiked_piece_4.inc"
};
static NJS_CNK_OBJECT emi_spiked_piece_4_joint = {
#include "shared/enemy_emi_art/emi_spiked_piece_4_joint.inc"
};
static Sint16 emi_spiked_scroll_model_polygons[183] = {
#include "shared/enemy_emi_art/emi_spiked_scroll_model_polygons.inc"
};
static Sint32 emi_spiked_scroll_model_vertices[171] = {
#include "shared/enemy_emi_art/emi_spiked_scroll_model_vertices.inc"
};
NJS_CNK_MODEL emi_spiked_scroll_model = {
#include "shared/enemy_emi_art/emi_spiked_scroll_model.inc"
};
NJS_CNK_OBJECT emi_spiked_object = {
#include "shared/enemy_emi_art/emi_spiked_object.inc"
};
static NJS_MKEY_F emi_spiked_motion_joint_1_position[2] = {
#include "shared/enemy_emi_art/emi_spiked_motion_joint_1_position.inc"
};
static NJS_MKEY_F emi_spiked_motion_joint_2_position[2] = {
#include "shared/enemy_emi_art/emi_spiked_motion_joint_2_position.inc"
};
static NJS_MDATA2 emi_spiked_motion_tracks[6] = {
#include "shared/enemy_emi_art/emi_spiked_motion_tracks.inc"
};
NJS_MOTION emi_spiked_motion = {
#include "shared/enemy_emi_art/emi_spiked_motion.inc"
};
static Sint16 emi_spiked_tread_uv[60] = {
#include "shared/enemy_emi_art/emi_spiked_tread_uv.inc"
};
struct texture_anim emi_spiked_scroll_uv = {
#include "shared/enemy_emi_art/emi_spiked_scroll_uv.inc"
};
