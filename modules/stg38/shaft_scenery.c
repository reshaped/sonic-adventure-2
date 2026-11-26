#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_scenery_art.h"
#include "shared/water_shaft/textures.h"
#include "stg38/core_knuckles_shaft_scenery_filenames.h"
#include "stg38/water_scenery.h"

/* the columns' texture name, which bgtex38's texture list points at too */
const char shaft_column_texture_name[12] = {
#include "stg38/shaft_scenery/shaft_column_texture_name.inc"
};

/* STG38.PRS's texture lists, chunk models and motions, from the disc. its D
 * lies between specks.c and core_shaft_rotor.c */
static NJS_TEXNAME column_textures_slot[1] = {
#include "stg38/shaft_scenery/column_textures_slot.inc"
};
NJS_TEXLIST column_textures = {
#include "stg38/shaft_scenery/column_textures.inc"
};
static Sint16 rising_column_panel_12_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_12_polygons.inc"
};
static Sint32 rising_column_panel_12_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_12_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_12_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_12_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_12 = {
#include "stg38/shaft_scenery/rising_column_panel_12.inc"
};
static Sint16 rising_column_panel_11_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_11_polygons.inc"
};
static Sint32 rising_column_panel_11_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_11_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_11_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_11_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_11 = {
#include "stg38/shaft_scenery/rising_column_panel_11.inc"
};
static Sint16 rising_column_panel_10_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_10_polygons.inc"
};
static Sint32 rising_column_panel_10_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_10_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_10_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_10_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_10 = {
#include "stg38/shaft_scenery/rising_column_panel_10.inc"
};
static Sint16 rising_column_panel_9_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_9_polygons.inc"
};
static Sint32 rising_column_panel_9_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_9_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_9_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_9_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_9 = {
#include "stg38/shaft_scenery/rising_column_panel_9.inc"
};
static Sint16 rising_column_panel_8_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_8_polygons.inc"
};
static Sint32 rising_column_panel_8_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_8_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_8_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_8_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_8 = {
#include "stg38/shaft_scenery/rising_column_panel_8.inc"
};
static Sint16 rising_column_panel_7_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_7_polygons.inc"
};
static Sint32 rising_column_panel_7_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_7_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_7_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_7_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_7 = {
#include "stg38/shaft_scenery/rising_column_panel_7.inc"
};
static Sint16 rising_column_panel_6_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_6_polygons.inc"
};
static Sint32 rising_column_panel_6_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_6_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_6_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_6_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_6 = {
#include "stg38/shaft_scenery/rising_column_panel_6.inc"
};
static Sint16 rising_column_panel_5_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_5_polygons.inc"
};
static Sint32 rising_column_panel_5_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_5_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_5_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_5_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_5 = {
#include "stg38/shaft_scenery/rising_column_panel_5.inc"
};
static Sint16 rising_column_panel_4_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_4_polygons.inc"
};
static Sint32 rising_column_panel_4_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_4_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_4_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_4 = {
#include "stg38/shaft_scenery/rising_column_panel_4.inc"
};
static Sint16 rising_column_panel_3_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_3_polygons.inc"
};
static Sint32 rising_column_panel_3_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_3_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_3_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_3 = {
#include "stg38/shaft_scenery/rising_column_panel_3.inc"
};
static Sint16 rising_column_panel_2_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_2_polygons.inc"
};
static Sint32 rising_column_panel_2_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_2_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_2_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_2 = {
#include "stg38/shaft_scenery/rising_column_panel_2.inc"
};
static Sint16 rising_column_panel_1_polygons[31] = {
#include "stg38/shaft_scenery/rising_column_panel_1_polygons.inc"
};
static Sint32 rising_column_panel_1_vertices[27] = {
#include "stg38/shaft_scenery/rising_column_panel_1_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_1_mesh = {
#include "stg38/shaft_scenery/rising_column_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_1 = {
#include "stg38/shaft_scenery/rising_column_panel_1.inc"
};
NJS_CNK_OBJECT rising_columns = {
#include "stg38/shaft_scenery/rising_columns.inc"
};
static NJS_TEXNAME falling_columns_textures_slot[1] = {
#include "stg38/shaft_scenery/falling_columns_textures_slot.inc"
};
static NJS_TEXLIST falling_columns_textures = {
#include "stg38/shaft_scenery/falling_columns_textures.inc"
};
static Sint16 falling_column_panel_12_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_12_polygons.inc"
};
static Sint32 falling_column_panel_12_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_12_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_12_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_12_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_12 = {
#include "stg38/shaft_scenery/falling_column_panel_12.inc"
};
static Sint16 falling_column_panel_11_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_11_polygons.inc"
};
static Sint32 falling_column_panel_11_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_11_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_11_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_11_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_11 = {
#include "stg38/shaft_scenery/falling_column_panel_11.inc"
};
static Sint16 falling_column_panel_10_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_10_polygons.inc"
};
static Sint32 falling_column_panel_10_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_10_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_10_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_10_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_10 = {
#include "stg38/shaft_scenery/falling_column_panel_10.inc"
};
static Sint16 falling_column_panel_9_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_9_polygons.inc"
};
static Sint32 falling_column_panel_9_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_9_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_9_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_9_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_9 = {
#include "stg38/shaft_scenery/falling_column_panel_9.inc"
};
static Sint16 falling_column_panel_8_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_8_polygons.inc"
};
static Sint32 falling_column_panel_8_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_8_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_8_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_8_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_8 = {
#include "stg38/shaft_scenery/falling_column_panel_8.inc"
};
static Sint16 falling_column_panel_7_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_7_polygons.inc"
};
static Sint32 falling_column_panel_7_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_7_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_7_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_7_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_7 = {
#include "stg38/shaft_scenery/falling_column_panel_7.inc"
};
static Sint16 falling_column_panel_6_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_6_polygons.inc"
};
static Sint32 falling_column_panel_6_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_6_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_6_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_6_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_6 = {
#include "stg38/shaft_scenery/falling_column_panel_6.inc"
};
static Sint16 falling_column_panel_5_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_5_polygons.inc"
};
static Sint32 falling_column_panel_5_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_5_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_5_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_5_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_5 = {
#include "stg38/shaft_scenery/falling_column_panel_5.inc"
};
static Sint16 falling_column_panel_4_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_4_polygons.inc"
};
static Sint32 falling_column_panel_4_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_4_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_4_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_4 = {
#include "stg38/shaft_scenery/falling_column_panel_4.inc"
};
static Sint16 falling_column_panel_3_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_3_polygons.inc"
};
static Sint32 falling_column_panel_3_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_3_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_3_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_3 = {
#include "stg38/shaft_scenery/falling_column_panel_3.inc"
};
static Sint16 falling_column_panel_2_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_2_polygons.inc"
};
static Sint32 falling_column_panel_2_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_2_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_2_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_2 = {
#include "stg38/shaft_scenery/falling_column_panel_2.inc"
};
static Sint16 falling_column_panel_1_polygons[31] = {
#include "stg38/shaft_scenery/falling_column_panel_1_polygons.inc"
};
static Sint32 falling_column_panel_1_vertices[27] = {
#include "stg38/shaft_scenery/falling_column_panel_1_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_1_mesh = {
#include "stg38/shaft_scenery/falling_column_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_1 = {
#include "stg38/shaft_scenery/falling_column_panel_1.inc"
};
NJS_CNK_OBJECT falling_columns = {
#include "stg38/shaft_scenery/falling_columns.inc"
};
static NJS_TEXNAME shaft_textures_slot[1] = {
#include "stg38/shaft_scenery/shaft_textures_slot.inc"
};
NJS_TEXLIST shaft_textures = {
#include "stg38/shaft_scenery/shaft_textures.inc"
};
static Sint16 shaft_wall_panel_2_polygons[273] = {
#include "stg38/shaft_scenery/shaft_wall_panel_2_polygons.inc"
};
static Sint32 shaft_wall_panel_2_vertices[123] = {
#include "stg38/shaft_scenery/shaft_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL shaft_wall_panel_2_mesh = {
#include "stg38/shaft_scenery/shaft_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT shaft_wall_panel_2 = {
#include "stg38/shaft_scenery/shaft_wall_panel_2.inc"
};
static Sint16 shaft_wall_panel_1_polygons[273] = {
#include "stg38/shaft_scenery/shaft_wall_panel_1_polygons.inc"
};
static Sint32 shaft_wall_panel_1_vertices[123] = {
#include "stg38/shaft_scenery/shaft_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_wall_panel_1_mesh = {
#include "stg38/shaft_scenery/shaft_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT shaft_wall_panel_1 = {
#include "stg38/shaft_scenery/shaft_wall_panel_1.inc"
};
NJS_CNK_OBJECT shaft_walls = {
#include "stg38/shaft_scenery/shaft_walls.inc"
};
static NJS_TEXNAME shaft_entry_walls_textures_slot[1] = {
#include "stg38/shaft_scenery/shaft_entry_walls_textures_slot.inc"
};
static NJS_TEXLIST shaft_entry_walls_textures = {
#include "stg38/shaft_scenery/shaft_entry_walls_textures.inc"
};
static Sint16 shaft_entry_wall_panel_2_polygons[273] = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_2_polygons.inc"
};
static Sint32 shaft_entry_wall_panel_2_vertices[123] = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL shaft_entry_wall_panel_2_mesh = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT shaft_entry_wall_panel_2 = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_2.inc"
};
static Sint16 shaft_entry_wall_panel_1_polygons[273] = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_1_polygons.inc"
};
static Sint32 shaft_entry_wall_panel_1_vertices[123] = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_entry_wall_panel_1_mesh = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT shaft_entry_wall_panel_1 = {
#include "stg38/shaft_scenery/shaft_entry_wall_panel_1.inc"
};
NJS_CNK_OBJECT shaft_entry_walls = {
#include "stg38/shaft_scenery/shaft_entry_walls.inc"
};
static NJS_TEXNAME surface_tiles_textures_slot[1] = {
#include "stg38/shaft_scenery/surface_tiles_textures_slot.inc"
};
static NJS_TEXLIST surface_tiles_textures = {
#include "stg38/shaft_scenery/surface_tiles_textures.inc"
};
static Sint16 surface_tile_panel_1_polygons[317] = {
#include "stg38/shaft_scenery/surface_tile_panel_1_polygons.inc"
};
static Sint32 surface_tile_panel_1_vertices[195] = {
#include "stg38/shaft_scenery/surface_tile_panel_1_vertices.inc"
};
static NJS_CNK_MODEL surface_tile_panel_1_mesh = {
#include "stg38/shaft_scenery/surface_tile_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT surface_tile_panel_1 = {
#include "stg38/shaft_scenery/surface_tile_panel_1.inc"
};
NJS_CNK_OBJECT surface_tiles = {
#include "stg38/shaft_scenery/surface_tiles.inc"
};
static NJS_TEXNAME fence_textures_slot[1] = {
#include "stg38/shaft_scenery/fence_textures_slot.inc"
};
NJS_TEXLIST fence_textures = {
#include "stg38/shaft_scenery/fence_textures.inc"
};
static Sint16 beam_fence_panel_1_polygons[77] = {
#include "stg38/shaft_scenery/beam_fence_panel_1_polygons.inc"
};
static Sint32 beam_fence_panel_1_vertices[83] = {
#include "stg38/shaft_scenery/beam_fence_panel_1_vertices.inc"
};
static NJS_CNK_MODEL beam_fence_panel_1_mesh = {
#include "stg38/shaft_scenery/beam_fence_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT beam_fence_panel_1 = {
#include "stg38/shaft_scenery/beam_fence_panel_1.inc"
};
NJS_CNK_OBJECT beam_fence = {
#include "stg38/shaft_scenery/beam_fence.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_0_position[2] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_0_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_0_angles[2] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_0_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_1_position[90] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_1_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_1_angles[48] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_1_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_2_position[134] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_2_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_2_angles[2] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_2_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_3_position[135] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_3_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_3_angles[32] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_3_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_4_position[134] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_4_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_4_angles[2] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_4_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_5_position[90] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_5_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_5_angles[48] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_5_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_6_position[93] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_6_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_6_angles[2] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_6_angles.inc"
};
static NJS_MKEY_F shaft_rotor_motion_joint_7_position[159] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_7_position.inc"
};
static NJS_MKEY_A shaft_rotor_motion_joint_7_angles[2] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_joint_7_angles.inc"
};
static NJS_MDATA3 shaft_rotor_motion_tracks[8] = {
#include "stg38/shaft_scenery/shaft_rotor_motion_tracks.inc"
};
NJS_MOTION shaft_rotor_motion = {
#include "stg38/shaft_scenery/shaft_rotor_motion.inc"
};
