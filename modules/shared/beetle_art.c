#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "enemy/enemy.h"
#include "shared/beetle_art.h"

const char beetle_texture_filenames[131] = {
#include "shared/beetle_art/beetle_texture_filenames.inc"
};

/* the texture lists, chunk models, motions, UV scrolls, chunk model
 * pointers, texture slices and enemy actions that 36 images place, from the
 * disc. its D precedes beetle.c */
static NJS_TEXNAME beetle_skins[3] = {
#include "shared/beetle_art/beetle_skins.inc"
};
NJS_TEXLIST beetle_texlist = {
#include "shared/beetle_art/beetle_texlist.inc"
};
Sint16 beetle_canopy_polygons[365] = {
#include "shared/beetle_art/beetle_canopy_polygons.inc"
};
static Sint32 beetle_canopy_vertices[255] = {
#include "shared/beetle_art/beetle_canopy_vertices.inc"
};
static NJS_CNK_MODEL beetle_canopy_model = {
#include "shared/beetle_art/beetle_canopy_model.inc"
};
static NJS_CNK_OBJECT beetle_canopy = {
#include "shared/beetle_art/beetle_canopy.inc"
};
Sint16 beetle_lower_fin_polygons[171] = {
#include "shared/beetle_art/beetle_lower_fin_polygons.inc"
};
static Sint32 beetle_lower_fin_vertices[99] = {
#include "shared/beetle_art/beetle_lower_fin_vertices.inc"
};
static NJS_CNK_MODEL beetle_lower_fin_model = {
#include "shared/beetle_art/beetle_lower_fin_model.inc"
};
static NJS_CNK_OBJECT beetle_lower_fin = {
#include "shared/beetle_art/beetle_lower_fin.inc"
};
Sint16 beetle_rotor_polygons[275] = {
#include "shared/beetle_art/beetle_rotor_polygons.inc"
};
static Sint32 beetle_rotor_vertices[147] = {
#include "shared/beetle_art/beetle_rotor_vertices.inc"
};
static NJS_CNK_MODEL beetle_rotor_model = {
#include "shared/beetle_art/beetle_rotor_model.inc"
};
static NJS_CNK_OBJECT beetle_rotor = {
#include "shared/beetle_art/beetle_rotor.inc"
};
Sint16 beetle_body_polygons[535] = {
#include "shared/beetle_art/beetle_body_polygons.inc"
};
static Sint32 beetle_body_vertices[297] = {
#include "shared/beetle_art/beetle_body_vertices.inc"
};
static NJS_CNK_MODEL beetle_body_model = {
#include "shared/beetle_art/beetle_body_model.inc"
};
static NJS_CNK_OBJECT beetle_body = {
#include "shared/beetle_art/beetle_body.inc"
};
Sint16 beetle_rotor_disc_polygons[93] = {
#include "shared/beetle_art/beetle_rotor_disc_polygons.inc"
};
static Sint32 beetle_rotor_disc_vertices[57] = {
#include "shared/beetle_art/beetle_rotor_disc_vertices.inc"
};
static NJS_CNK_MODEL beetle_rotor_disc_model = {
#include "shared/beetle_art/beetle_rotor_disc_model.inc"
};
static NJS_CNK_OBJECT beetle_rotor_disc = {
#include "shared/beetle_art/beetle_rotor_disc.inc"
};
NJS_CNK_OBJECT beetle_object[1] = {
#include "shared/beetle_art/beetle_object.inc"
};
static NJS_MKEY_A beetle_hover_motion_joint_1_angles[3] = {
#include "shared/beetle_art/beetle_hover_motion_joint_1_angles.inc"
};
static NJS_MKEY_F beetle_hover_motion_joint_5_position[3] = {
#include "shared/beetle_art/beetle_hover_motion_joint_5_position.inc"
};
static NJS_MDATA2 beetle_hover_motion_tracks[6] = {
#include "shared/beetle_art/beetle_hover_motion_tracks.inc"
};
static NJS_MOTION beetle_hover_motion = {
#include "shared/beetle_art/beetle_hover_motion.inc"
};
static NJS_TEXNAME gunner_skins[4] = {
#include "shared/beetle_art/gunner_skins.inc"
};
NJS_TEXLIST gunner_texlist = {
#include "shared/beetle_art/gunner_texlist.inc"
};
static Sint16 gunner_canopy_polygons[365] = {
#include "shared/beetle_art/gunner_canopy_polygons.inc"
};
static Sint32 gunner_canopy_vertices[255] = {
#include "shared/beetle_art/gunner_canopy_vertices.inc"
};
static NJS_CNK_MODEL gunner_canopy_model = {
#include "shared/beetle_art/gunner_canopy_model.inc"
};
static NJS_CNK_OBJECT gunner_canopy = {
#include "shared/beetle_art/gunner_canopy.inc"
};
static Sint16 gunner_gun_mount_polygons[457] = {
#include "shared/beetle_art/gunner_gun_mount_polygons.inc"
};
static Sint32 gunner_gun_mount_vertices[309] = {
#include "shared/beetle_art/gunner_gun_mount_vertices.inc"
};
static NJS_CNK_MODEL gunner_gun_mount_model = {
#include "shared/beetle_art/gunner_gun_mount_model.inc"
};
static NJS_CNK_OBJECT gunner_gun_mount = {
#include "shared/beetle_art/gunner_gun_mount.inc"
};
static Sint16 gunner_barrel_polygons[217] = {
#include "shared/beetle_art/gunner_barrel_polygons.inc"
};
static Sint32 gunner_barrel_vertices[153] = {
#include "shared/beetle_art/gunner_barrel_vertices.inc"
};
static NJS_CNK_MODEL gunner_barrel_model = {
#include "shared/beetle_art/gunner_barrel_model.inc"
};
static NJS_CNK_OBJECT gunner_barrel = {
#include "shared/beetle_art/gunner_barrel.inc"
};
static Sint16 gunner_rotor_polygons[275] = {
#include "shared/beetle_art/gunner_rotor_polygons.inc"
};
static Sint32 gunner_rotor_vertices[147] = {
#include "shared/beetle_art/gunner_rotor_vertices.inc"
};
static NJS_CNK_MODEL gunner_rotor_model = {
#include "shared/beetle_art/gunner_rotor_model.inc"
};
static NJS_CNK_OBJECT gunner_rotor = {
#include "shared/beetle_art/gunner_rotor.inc"
};
static Sint16 gunner_body_polygons[535] = {
#include "shared/beetle_art/gunner_body_polygons.inc"
};
static Sint32 gunner_body_vertices[297] = {
#include "shared/beetle_art/gunner_body_vertices.inc"
};
static NJS_CNK_MODEL gunner_body_model = {
#include "shared/beetle_art/gunner_body_model.inc"
};
static NJS_CNK_OBJECT gunner_body = {
#include "shared/beetle_art/gunner_body.inc"
};
static Sint16 gunner_rotor_disc_polygons[93] = {
#include "shared/beetle_art/gunner_rotor_disc_polygons.inc"
};
static Sint32 gunner_rotor_disc_vertices[57] = {
#include "shared/beetle_art/gunner_rotor_disc_vertices.inc"
};
static NJS_CNK_MODEL gunner_rotor_disc_model = {
#include "shared/beetle_art/gunner_rotor_disc_model.inc"
};
static NJS_CNK_OBJECT gunner_rotor_disc = {
#include "shared/beetle_art/gunner_rotor_disc.inc"
};
NJS_CNK_OBJECT gunner_object[1] = {
#include "shared/beetle_art/gunner_object.inc"
};
static NJS_MKEY_A gunner_hover_motion_joint_1_angles[3] = {
#include "shared/beetle_art/gunner_hover_motion_joint_1_angles.inc"
};
static NJS_MKEY_F gunner_hover_motion_joint_6_position[3] = {
#include "shared/beetle_art/gunner_hover_motion_joint_6_position.inc"
};
static NJS_MDATA2 gunner_hover_motion_tracks[7] = {
#include "shared/beetle_art/gunner_hover_motion_tracks.inc"
};
static NJS_MOTION gunner_hover_motion = {
#include "shared/beetle_art/gunner_hover_motion.inc"
};
static NJS_MKEY_F gunner_fire_motion_joint_4_position[5] = {
#include "shared/beetle_art/gunner_fire_motion_joint_4_position.inc"
};
static NJS_MKEY_F gunner_fire_motion_joint_5_position[5] = {
#include "shared/beetle_art/gunner_fire_motion_joint_5_position.inc"
};
static NJS_MDATA2 gunner_fire_motion_tracks[7] = {
#include "shared/beetle_art/gunner_fire_motion_tracks.inc"
};
static NJS_MOTION gunner_fire_motion = {
#include "shared/beetle_art/gunner_fire_motion.inc"
};
static NJS_TEXNAME bomber_skins[4] = {
#include "shared/beetle_art/bomber_skins.inc"
};
NJS_TEXLIST bomber_texlist = {
#include "shared/beetle_art/bomber_texlist.inc"
};
static Sint16 bomber_canopy_polygons[365] = {
#include "shared/beetle_art/bomber_canopy_polygons.inc"
};
static Sint32 bomber_canopy_vertices[255] = {
#include "shared/beetle_art/bomber_canopy_vertices.inc"
};
static NJS_CNK_MODEL bomber_canopy_model = {
#include "shared/beetle_art/bomber_canopy_model.inc"
};
static NJS_CNK_OBJECT bomber_canopy = {
#include "shared/beetle_art/bomber_canopy.inc"
};
static Sint16 bomber_bomb_rack_polygons[401] = {
#include "shared/beetle_art/bomber_bomb_rack_polygons.inc"
};
static Sint32 bomber_bomb_rack_vertices[225] = {
#include "shared/beetle_art/bomber_bomb_rack_vertices.inc"
};
static NJS_CNK_MODEL bomber_bomb_rack_model = {
#include "shared/beetle_art/bomber_bomb_rack_model.inc"
};
static NJS_CNK_OBJECT bomber_bomb_rack = {
#include "shared/beetle_art/bomber_bomb_rack.inc"
};
static NJS_CNK_OBJECT bomber_attachment = {
#include "shared/beetle_art/bomber_attachment.inc"
};
static Sint16 bomber_rotor_disc_polygons[93] = {
#include "shared/beetle_art/bomber_rotor_disc_polygons.inc"
};
static Sint32 bomber_rotor_disc_vertices[57] = {
#include "shared/beetle_art/bomber_rotor_disc_vertices.inc"
};
static NJS_CNK_MODEL bomber_rotor_disc_model = {
#include "shared/beetle_art/bomber_rotor_disc_model.inc"
};
static NJS_CNK_OBJECT bomber_rotor_disc = {
#include "shared/beetle_art/bomber_rotor_disc.inc"
};
static Sint16 bomber_rotor_polygons[275] = {
#include "shared/beetle_art/bomber_rotor_polygons.inc"
};
static Sint32 bomber_rotor_vertices[147] = {
#include "shared/beetle_art/bomber_rotor_vertices.inc"
};
static NJS_CNK_MODEL bomber_rotor_model = {
#include "shared/beetle_art/bomber_rotor_model.inc"
};
static NJS_CNK_OBJECT bomber_rotor = {
#include "shared/beetle_art/bomber_rotor.inc"
};
static Sint16 bomber_body_polygons[597] = {
#include "shared/beetle_art/bomber_body_polygons.inc"
};
static Sint32 bomber_body_vertices[339] = {
#include "shared/beetle_art/bomber_body_vertices.inc"
};
static NJS_CNK_MODEL bomber_body_model = {
#include "shared/beetle_art/bomber_body_model.inc"
};
static NJS_CNK_OBJECT bomber_body = {
#include "shared/beetle_art/bomber_body.inc"
};
NJS_CNK_OBJECT bomber_object[1] = {
#include "shared/beetle_art/bomber_object.inc"
};
static NJS_MKEY_A bomber_hover_motion_joint_3_angles[3] = {
#include "shared/beetle_art/bomber_hover_motion_joint_3_angles.inc"
};
static NJS_MKEY_F bomber_hover_motion_joint_6_position[3] = {
#include "shared/beetle_art/bomber_hover_motion_joint_6_position.inc"
};
static NJS_MDATA2 bomber_hover_motion_tracks[7] = {
#include "shared/beetle_art/bomber_hover_motion_tracks.inc"
};
static NJS_MOTION bomber_hover_motion = {
#include "shared/beetle_art/bomber_hover_motion.inc"
};
static NJS_TEXNAME bumper_skins[2] = {
#include "shared/beetle_art/bumper_skins.inc"
};
NJS_TEXLIST bumper_texlist = {
#include "shared/beetle_art/bumper_texlist.inc"
};
static Sint16 bumper_pad_polygons[167] = {
#include "shared/beetle_art/bumper_pad_polygons.inc"
};
static Sint32 bumper_pad_vertices[105] = {
#include "shared/beetle_art/bumper_pad_vertices.inc"
};
static NJS_CNK_MODEL bumper_pad_model = {
#include "shared/beetle_art/bumper_pad_model.inc"
};
static NJS_CNK_OBJECT bumper_pad = {
#include "shared/beetle_art/bumper_pad.inc"
};
static Sint16 bumper_polygons[649] = {
#include "shared/beetle_art/bumper_polygons.inc"
};
static Sint32 bumper_vertices[501] = {
#include "shared/beetle_art/bumper_vertices.inc"
};
static NJS_CNK_MODEL bumper_model = {
#include "shared/beetle_art/bumper_model.inc"
};
NJS_CNK_OBJECT bumper_object[1] = {
#include "shared/beetle_art/bumper_object.inc"
};
static NJS_MKEY_F bumper_rebound_motion_joint_1_position[3] = {
#include "shared/beetle_art/bumper_rebound_motion_joint_1_position.inc"
};
static NJS_MDATA2 bumper_rebound_motion_tracks[2] = {
#include "shared/beetle_art/bumper_rebound_motion_tracks.inc"
};
static NJS_MOTION bumper_rebound_motion = {
#include "shared/beetle_art/bumper_rebound_motion.inc"
};
static NJS_TEXNAME electric_skins[9] = {
#include "shared/beetle_art/electric_skins.inc"
};
NJS_TEXLIST electric_texlist = {
#include "shared/beetle_art/electric_texlist.inc"
};
static NJS_TEXNAME discharge_surface_name[1] = {
#include "shared/beetle_art/discharge_surface_name.inc"
};
NJS_TEXLIST discharge_surface_texlist = {
#include "shared/beetle_art/discharge_surface_texlist.inc"
};
static Sint16 discharge_polygons[497] = {
#include "shared/beetle_art/discharge_polygons.inc"
};
static Sint32 discharge_vertices[399] = {
#include "shared/beetle_art/discharge_vertices.inc"
};
NJS_CNK_MODEL discharge_model = {
#include "shared/beetle_art/discharge_model.inc"
};
NJS_CNK_OBJECT discharge_object = {
#include "shared/beetle_art/discharge_object.inc"
};
static Sint16 discharge_uvs[342] = {
#include "shared/beetle_art/discharge_uvs.inc"
};
struct texture_anim discharge_anim = {
#include "shared/beetle_art/discharge_anim.inc"
};
/* clang-format off */
struct collision_shape beetle_shapes[4] = {
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_UNTURNED,
     {0.0F, 0.0F, 0.0F}, 6.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_PLAYERS,
     SHAPE_OFF | SHAPE_UNTURNED,
     {0.0F, 0.0F, 0.0F}, 15.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_UNTURNED,
     {0.0F, -3.0F, 0.0F}, 6.0F, 3.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(2) | SHAPE_HURTS_ALL,
     0,
     {0.0F, 5.0F, 0.0F}, 12.0F, 5.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
NJS_CNK_MODEL *beetle_pieces[6] = {
#include "shared/beetle_art/beetle_pieces.inc"
};
NJS_CNK_MODEL *gunner_pieces[7] = {
#include "shared/beetle_art/gunner_pieces.inc"
};
NJS_CNK_MODEL *bomber_pieces[6] = {
#include "shared/beetle_art/bomber_pieces.inc"
};
NJS_CNK_MODEL *bumper_pieces[3] = {
#include "shared/beetle_art/bumper_pieces.inc"
};
NJS_TEXLIST discharge_texlists[9] = {
#include "shared/beetle_art/discharge_texlists.inc"
};
struct enemy_action beetle_actions[BEETLE_ACTION_COUNT] = {
#include "shared/beetle_art/beetle_actions.inc"
};
