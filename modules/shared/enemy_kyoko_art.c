/* the kyoko's art, drawn with E_KYOKOTEX's textures. it holds the body, the
 * spinner, the three guns, the pieces and the motions. kyoko_body_anim animates
 * the body's texture */
#include <ninja.h>
#include <sg_xpt.h>
#include "draw/texture_anim.h"
#include "shared/enemy_kyoko_art.h"

static const char kyoko_texture_filenames[59] = {
#include "shared/enemy_kyoko_art/kyoko_texture_filenames.inc"
};

static Sint16 kyoko_piece_0_polygons[303] = {
#include "shared/enemy_kyoko_art/kyoko_piece_0_polygons.inc"
};

static Sint32 kyoko_piece_0_vertices[243] = {
#include "shared/enemy_kyoko_art/kyoko_piece_0_vertices.inc"
};

NJS_CNK_MODEL kyoko_piece_0 = {
#include "shared/enemy_kyoko_art/kyoko_piece_0.inc"
};

static NJS_CNK_OBJECT kyoko_piece_0_object = {
#include "shared/enemy_kyoko_art/kyoko_piece_0_object.inc"
};

static Sint16 kyoko_capture_gun_polygons[145] = {
#include "shared/enemy_kyoko_art/kyoko_capture_gun_polygons.inc"
};

static Sint32 kyoko_capture_gun_vertices[81] = {
#include "shared/enemy_kyoko_art/kyoko_capture_gun_vertices.inc"
};

NJS_CNK_MODEL kyoko_capture_gun_model = {
#include "shared/enemy_kyoko_art/kyoko_capture_gun_model.inc"
};

NJS_CNK_OBJECT kyoko_capture_gun[1] = {
#include "shared/enemy_kyoko_art/kyoko_capture_gun.inc"
};

static Sint16 kyoko_laser_gun_polygons[157] = {
#include "shared/enemy_kyoko_art/kyoko_laser_gun_polygons.inc"
};

static Sint32 kyoko_laser_gun_vertices[117] = {
#include "shared/enemy_kyoko_art/kyoko_laser_gun_vertices.inc"
};

NJS_CNK_MODEL kyoko_laser_gun_model = {
#include "shared/enemy_kyoko_art/kyoko_laser_gun_model.inc"
};

NJS_CNK_OBJECT kyoko_laser_gun[1] = {
#include "shared/enemy_kyoko_art/kyoko_laser_gun.inc"
};

static Sint16 kyoko_bullet_gun_polygons[163] = {
#include "shared/enemy_kyoko_art/kyoko_bullet_gun_polygons.inc"
};

static Sint32 kyoko_bullet_gun_vertices[117] = {
#include "shared/enemy_kyoko_art/kyoko_bullet_gun_vertices.inc"
};

NJS_CNK_MODEL kyoko_bullet_gun_model = {
#include "shared/enemy_kyoko_art/kyoko_bullet_gun_model.inc"
};

NJS_CNK_OBJECT kyoko_bullet_gun[1] = {
#include "shared/enemy_kyoko_art/kyoko_bullet_gun.inc"
};

static Sint16 kyoko_piece_1_polygons[57] = {
#include "shared/enemy_kyoko_art/kyoko_piece_1_polygons.inc"
};

static Sint32 kyoko_piece_1_vertices[39] = {
#include "shared/enemy_kyoko_art/kyoko_piece_1_vertices.inc"
};

NJS_CNK_MODEL kyoko_piece_1 = {
#include "shared/enemy_kyoko_art/kyoko_piece_1.inc"
};

static NJS_CNK_OBJECT kyoko_piece_1_object = {
#include "shared/enemy_kyoko_art/kyoko_piece_1_object.inc"
};

static Sint16 kyoko_piece_2_polygons[57] = {
#include "shared/enemy_kyoko_art/kyoko_piece_2_polygons.inc"
};

static Sint32 kyoko_piece_2_vertices[39] = {
#include "shared/enemy_kyoko_art/kyoko_piece_2_vertices.inc"
};

NJS_CNK_MODEL kyoko_piece_2 = {
#include "shared/enemy_kyoko_art/kyoko_piece_2.inc"
};

static NJS_CNK_OBJECT kyoko_piece_2_object = {
#include "shared/enemy_kyoko_art/kyoko_piece_2_object.inc"
};

static Sint16 kyoko_piece_3_polygons[353] = {
#include "shared/enemy_kyoko_art/kyoko_piece_3_polygons.inc"
};

static Sint32 kyoko_piece_3_vertices[267] = {
#include "shared/enemy_kyoko_art/kyoko_piece_3_vertices.inc"
};

NJS_CNK_MODEL kyoko_piece_3 = {
#include "shared/enemy_kyoko_art/kyoko_piece_3.inc"
};

static NJS_CNK_OBJECT kyoko_piece_3_object = {
#include "shared/enemy_kyoko_art/kyoko_piece_3_object.inc"
};

static Sint16 kyoko_piece_4_polygons[353] = {
#include "shared/enemy_kyoko_art/kyoko_piece_4_polygons.inc"
};

static Sint32 kyoko_piece_4_vertices[267] = {
#include "shared/enemy_kyoko_art/kyoko_piece_4_vertices.inc"
};

NJS_CNK_MODEL kyoko_piece_4 = {
#include "shared/enemy_kyoko_art/kyoko_piece_4.inc"
};

static NJS_CNK_OBJECT kyoko_piece_4_object = {
#include "shared/enemy_kyoko_art/kyoko_piece_4_object.inc"
};

static Sint16 kyoko_piece_5_polygons[65] = {
#include "shared/enemy_kyoko_art/kyoko_piece_5_polygons.inc"
};

static Sint32 kyoko_piece_5_vertices[75] = {
#include "shared/enemy_kyoko_art/kyoko_piece_5_vertices.inc"
};

NJS_CNK_MODEL kyoko_piece_5 = {
#include "shared/enemy_kyoko_art/kyoko_piece_5.inc"
};

static NJS_CNK_OBJECT kyoko_piece_5_object = {
#include "shared/enemy_kyoko_art/kyoko_piece_5_object.inc"
};

static Sint16 kyoko_piece_6_polygons[213] = {
#include "shared/enemy_kyoko_art/kyoko_piece_6_polygons.inc"
};

static Sint32 kyoko_piece_6_vertices[129] = {
#include "shared/enemy_kyoko_art/kyoko_piece_6_vertices.inc"
};

NJS_CNK_MODEL kyoko_piece_6 = {
#include "shared/enemy_kyoko_art/kyoko_piece_6.inc"
};

static NJS_CNK_OBJECT kyoko_piece_6_object = {
#include "shared/enemy_kyoko_art/kyoko_piece_6_object.inc"
};

static Sint16 kyoko_spinner_polygons[79] = {
#include "shared/enemy_kyoko_art/kyoko_spinner_polygons.inc"
};

static Sint32 kyoko_spinner_vertices[87] = {
#include "shared/enemy_kyoko_art/kyoko_spinner_vertices.inc"
};

NJS_CNK_MODEL kyoko_spinner_model = {
#include "shared/enemy_kyoko_art/kyoko_spinner_model.inc"
};

NJS_CNK_OBJECT kyoko_spinner[1] = {
#include "shared/enemy_kyoko_art/kyoko_spinner.inc"
};

static Sint16 kyoko_body_polygons[277] = {
#include "shared/enemy_kyoko_art/kyoko_body_polygons.inc"
};

static Sint32 kyoko_body_vertices[255] = {
#include "shared/enemy_kyoko_art/kyoko_body_vertices.inc"
};

NJS_CNK_MODEL kyoko_body_model = {
#include "shared/enemy_kyoko_art/kyoko_body_model.inc"
};

NJS_CNK_OBJECT kyoko_object = {
#include "shared/enemy_kyoko_art/kyoko_object.inc"
};

static Sint16 scroll_uvs[32] = {
#include "shared/enemy_kyoko_art/scroll_uvs.inc"
};

struct texture_anim kyoko_body_anim = {
#include "shared/enemy_kyoko_art/kyoko_body_anim.inc"
};

static NJS_TEXNAME textures[6] = {
#include "shared/enemy_kyoko_art/textures.inc"
};

NJS_TEXLIST kyoko_texlist = {
#include "shared/enemy_kyoko_art/kyoko_texlist.inc"
};

static NJS_MKEY_F kyoko_joint_0_position_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_joint_0_position_keys.inc"
};

static NJS_MKEY_F kyoko_joint_3_position_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_joint_3_position_keys.inc"
};

static NJS_MDATA2 kyoko_motion_tracks[12] = {
#include "shared/enemy_kyoko_art/kyoko_motion_tracks.inc"
};

NJS_MOTION kyoko_motion = {
#include "shared/enemy_kyoko_art/kyoko_motion.inc"
};

static NJS_MKEY_F unused_joint_0_position_keys[4] = {
#include "shared/enemy_kyoko_art/unused_joint_0_position_keys.inc"
};

static NJS_MKEY_F unused_joint_3_position_keys[4] = {
#include "shared/enemy_kyoko_art/unused_joint_3_position_keys.inc"
};

static NJS_MKEY_A unused_joint_4_rotation_keys[4] = {
#include "shared/enemy_kyoko_art/unused_joint_4_rotation_keys.inc"
};

static NJS_MKEY_A unused_joint_5_rotation_keys[4] = {
#include "shared/enemy_kyoko_art/unused_joint_5_rotation_keys.inc"
};

static NJS_MKEY_A unused_joint_6_rotation_keys[4] = {
#include "shared/enemy_kyoko_art/unused_joint_6_rotation_keys.inc"
};

static NJS_MKEY_A unused_joint_7_rotation_keys[4] = {
#include "shared/enemy_kyoko_art/unused_joint_7_rotation_keys.inc"
};

static NJS_MDATA2 unused_tracks[12] = {
#include "shared/enemy_kyoko_art/unused_tracks.inc"
};

NJS_MOTION kyoko_unused_motion = {
#include "shared/enemy_kyoko_art/kyoko_unused_motion.inc"
};

static NJS_MKEY_F kyoko_jets_on_joint_0_position_keys[3] = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_joint_0_position_keys.inc"
};

static NJS_MKEY_F kyoko_jets_on_joint_3_position_keys[3] = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_joint_3_position_keys.inc"
};

static NJS_MKEY_A kyoko_jets_on_joint_4_rotation_keys[3] = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_joint_4_rotation_keys.inc"
};

static NJS_MKEY_A kyoko_jets_on_joint_5_rotation_keys[3] = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_joint_5_rotation_keys.inc"
};

static NJS_MKEY_A kyoko_jets_on_joint_6_rotation_keys[3] = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_joint_6_rotation_keys.inc"
};

static NJS_MKEY_A kyoko_jets_on_joint_7_rotation_keys[3] = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_joint_7_rotation_keys.inc"
};

static NJS_MDATA2 kyoko_jets_on_motion_tracks[12] = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_motion_tracks.inc"
};

NJS_MOTION kyoko_jets_on_motion = {
#include "shared/enemy_kyoko_art/kyoko_jets_on_motion.inc"
};

static NJS_MKEY_F kyoko_jets_joint_0_position_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_jets_joint_0_position_keys.inc"
};

static NJS_MKEY_A kyoko_jets_joint_1_rotation_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_jets_joint_1_rotation_keys.inc"
};

static NJS_MKEY_F kyoko_jets_joint_3_position_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_jets_joint_3_position_keys.inc"
};

static NJS_MKEY_A kyoko_jets_joint_4_rotation_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_jets_joint_4_rotation_keys.inc"
};

static NJS_MKEY_A kyoko_jets_joint_5_rotation_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_jets_joint_5_rotation_keys.inc"
};

static NJS_MKEY_A kyoko_jets_joint_6_rotation_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_jets_joint_6_rotation_keys.inc"
};

static NJS_MKEY_A kyoko_jets_joint_7_rotation_keys[2] = {
#include "shared/enemy_kyoko_art/kyoko_jets_joint_7_rotation_keys.inc"
};

static NJS_MDATA2 kyoko_jets_motion_tracks[12] = {
#include "shared/enemy_kyoko_art/kyoko_jets_motion_tracks.inc"
};

NJS_MOTION kyoko_jets_motion = {
#include "shared/enemy_kyoko_art/kyoko_jets_motion.inc"
};
