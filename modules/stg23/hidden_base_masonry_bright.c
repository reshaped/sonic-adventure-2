/* STG23.PRS's texture lists, chunk models and Basic models, from the disc.
 * its D lies between plant.c and wall.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/masonry_textures.h"
#include "stg23/wall_models.h"

static NJS_TEXNAME hidden_base_masonry_bright_textures[1] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_bright_textures.inc"
};
NJS_TEXLIST wall_bright_whole_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_bright_whole_texlist.inc"
};
static Sint16 wall_bright_whole_polygons[51] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_whole_polygons.inc"
};
static Sint32 wall_bright_whole_vertices[51] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_whole_vertices.inc"
};
NJS_CNK_MODEL wall_bright_whole_model = {
#include "stg23/hidden_base_masonry_bright/wall_bright_whole_model.inc"
};
static NJS_CNK_OBJECT wall_bright_whole_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_whole_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_bright_crack_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_bright_crack_textures.inc"
};
NJS_TEXLIST wall_bright_cracked_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_bright_cracked_texlist.inc"
};
static Sint16 wall_bright_cracked_polygons[153] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_cracked_polygons.inc"
};
static Sint32 wall_bright_cracked_vertices[147] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_cracked_vertices.inc"
};
NJS_CNK_MODEL wall_bright_cracked_model = {
#include "stg23/hidden_base_masonry_bright/wall_bright_cracked_model.inc"
};
static NJS_CNK_OBJECT wall_bright_cracked_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_cracked_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_bright_split_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_bright_split_textures.inc"
};
NJS_TEXLIST wall_bright_split_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_bright_split_texlist.inc"
};
static Sint16 wall_bright_split_polygons[167] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_split_polygons.inc"
};
static Sint32 wall_bright_split_vertices[151] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_split_vertices.inc"
};
NJS_CNK_MODEL wall_bright_split_model = {
#include "stg23/hidden_base_masonry_bright/wall_bright_split_model.inc"
};
static NJS_CNK_OBJECT wall_bright_split_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_split_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_bright_frame_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_bright_frame_textures.inc"
};
NJS_TEXLIST wall_bright_frame_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_bright_frame_texlist.inc"
};
static Sint16 wall_bright_frame_polygons[257] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_frame_polygons.inc"
};
static Sint32 wall_bright_frame_vertices[203] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_frame_vertices.inc"
};
NJS_CNK_MODEL wall_bright_frame_model = {
#include "stg23/hidden_base_masonry_bright/wall_bright_frame_model.inc"
};
static NJS_CNK_OBJECT wall_bright_frame_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_frame_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_bright_chip_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_bright_chip_textures.inc"
};
NJS_TEXLIST wall_bright_chips_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_bright_chips_texlist.inc"
};
static Sint16 wall_bright_shard_0_polygons[95] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_0_polygons.inc"
};
static Sint32 wall_bright_shard_0_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_0_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_0 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_0.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_0_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_0_object.inc"
};
static Sint16 wall_bright_shard_1_polygons[89] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_1_polygons.inc"
};
static Sint32 wall_bright_shard_1_vertices[83] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_1_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_1 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_1.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_1_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_1_object.inc"
};
static Sint16 wall_bright_shard_2_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_2_polygons.inc"
};
static Sint32 wall_bright_shard_2_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_2_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_2 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_2.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_2_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_2_object.inc"
};
static Sint16 wall_bright_shard_3_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_3_polygons.inc"
};
static Sint32 wall_bright_shard_3_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_3_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_3 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_3.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_3_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_3_object.inc"
};
static Sint16 wall_bright_shard_4_polygons[71] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_4_polygons.inc"
};
static Sint32 wall_bright_shard_4_vertices[67] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_4_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_4 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_4.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_4_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_4_object.inc"
};
static Sint16 wall_bright_shard_5_polygons[115] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_5_polygons.inc"
};
static Sint32 wall_bright_shard_5_vertices[119] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_5_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_5 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_5.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_5_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_5_object.inc"
};
static Sint16 wall_bright_shard_6_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_6_polygons.inc"
};
static Sint32 wall_bright_shard_6_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_6_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_6 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_6.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_6_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_6_object.inc"
};
static Sint16 wall_bright_shard_7_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_7_polygons.inc"
};
static Sint32 wall_bright_shard_7_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_7_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_7 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_7.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_7_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_7_object.inc"
};
static Sint16 wall_bright_shard_8_polygons[77] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_8_polygons.inc"
};
static Sint32 wall_bright_shard_8_vertices[75] = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_8_vertices.inc"
};
NJS_CNK_MODEL wall_bright_shard_8 = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_8.inc"
};
static NJS_CNK_OBJECT wall_bright_shard_8_object = {
#include "stg23/hidden_base_masonry_bright/wall_bright_shard_8_object.inc"
};
static NJS_CNK_OBJECT masonry_bright_chip_root = {
#include "stg23/hidden_base_masonry_bright/masonry_bright_chip_root.inc"
};
static NJS_TEXNAME hidden_base_masonry_dark_textures[1] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_dark_textures.inc"
};
NJS_TEXLIST wall_dark_whole_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_dark_whole_texlist.inc"
};
static Sint16 wall_dark_whole_polygons[51] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_whole_polygons.inc"
};
static Sint32 wall_dark_whole_vertices[51] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_whole_vertices.inc"
};
NJS_CNK_MODEL wall_dark_whole_model = {
#include "stg23/hidden_base_masonry_bright/wall_dark_whole_model.inc"
};
static NJS_CNK_OBJECT wall_dark_whole_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_whole_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_dark_crack_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_dark_crack_textures.inc"
};
NJS_TEXLIST wall_dark_cracked_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_dark_cracked_texlist.inc"
};
static Sint16 wall_dark_cracked_polygons[153] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_cracked_polygons.inc"
};
static Sint32 wall_dark_cracked_vertices[123] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_cracked_vertices.inc"
};
NJS_CNK_MODEL wall_dark_cracked_model = {
#include "stg23/hidden_base_masonry_bright/wall_dark_cracked_model.inc"
};
static NJS_CNK_OBJECT wall_dark_cracked_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_cracked_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_dark_split_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_dark_split_textures.inc"
};
NJS_TEXLIST wall_dark_split_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_dark_split_texlist.inc"
};
static Sint16 wall_dark_split_polygons[167] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_split_polygons.inc"
};
static Sint32 wall_dark_split_vertices[123] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_split_vertices.inc"
};
NJS_CNK_MODEL wall_dark_split_model = {
#include "stg23/hidden_base_masonry_bright/wall_dark_split_model.inc"
};
static NJS_CNK_OBJECT wall_dark_split_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_split_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_dark_frame_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_dark_frame_textures.inc"
};
NJS_TEXLIST wall_dark_frame_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_dark_frame_texlist.inc"
};
static Sint16 wall_dark_frame_polygons[257] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_frame_polygons.inc"
};
static Sint32 wall_dark_frame_vertices[179] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_frame_vertices.inc"
};
NJS_CNK_MODEL wall_dark_frame_model = {
#include "stg23/hidden_base_masonry_bright/wall_dark_frame_model.inc"
};
static NJS_CNK_OBJECT wall_dark_frame_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_frame_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_dark_chip_textures[2] = {
#include "stg23/hidden_base_masonry_bright/hidden_base_masonry_dark_chip_textures.inc"
};
NJS_TEXLIST wall_dark_chips_texlist = {
#include "stg23/hidden_base_masonry_bright/wall_dark_chips_texlist.inc"
};
static Sint16 wall_dark_shard_0_polygons[95] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_0_polygons.inc"
};
static Sint32 wall_dark_shard_0_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_0_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_0 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_0.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_0_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_0_object.inc"
};
static Sint16 wall_dark_shard_1_polygons[89] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_1_polygons.inc"
};
static Sint32 wall_dark_shard_1_vertices[67] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_1_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_1 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_1.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_1_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_1_object.inc"
};
static Sint16 wall_dark_shard_2_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_2_polygons.inc"
};
static Sint32 wall_dark_shard_2_vertices[95] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_2_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_2 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_2.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_2_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_2_object.inc"
};
static Sint16 wall_dark_shard_3_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_3_polygons.inc"
};
static Sint32 wall_dark_shard_3_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_3_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_3 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_3.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_3_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_3_object.inc"
};
static Sint16 wall_dark_shard_4_polygons[71] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_4_polygons.inc"
};
static Sint32 wall_dark_shard_4_vertices[67] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_4_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_4 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_4.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_4_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_4_object.inc"
};
static Sint16 wall_dark_shard_5_polygons[115] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_5_polygons.inc"
};
static Sint32 wall_dark_shard_5_vertices[107] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_5_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_5 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_5.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_5_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_5_object.inc"
};
static Sint16 wall_dark_shard_6_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_6_polygons.inc"
};
static Sint32 wall_dark_shard_6_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_6_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_6 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_6.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_6_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_6_object.inc"
};
static Sint16 wall_dark_shard_7_polygons[97] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_7_polygons.inc"
};
static Sint32 wall_dark_shard_7_vertices[99] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_7_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_7 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_7.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_7_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_7_object.inc"
};
static Sint16 wall_dark_shard_8_polygons[77] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_8_polygons.inc"
};
static Sint32 wall_dark_shard_8_vertices[67] = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_8_vertices.inc"
};
NJS_CNK_MODEL wall_dark_shard_8 = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_8.inc"
};
static NJS_CNK_OBJECT wall_dark_shard_8_object = {
#include "stg23/hidden_base_masonry_bright/wall_dark_shard_8_object.inc"
};
static NJS_CNK_OBJECT masonry_dark_chip_root = {
#include "stg23/hidden_base_masonry_bright/masonry_dark_chip_root.inc"
};
static NJS_MATERIAL base_wall_collision_materials[1] = {
#include "stg23/hidden_base_masonry_bright/base_wall_collision_materials.inc"
};
static Sint16 base_wall_collision_faces[19] = {
#include "stg23/hidden_base_masonry_bright/base_wall_collision_faces.inc"
};
static NJS_MESHSET base_wall_collision_meshsets[1] = {
#include "stg23/hidden_base_masonry_bright/base_wall_collision_meshsets.inc"
};
static NJS_POINT3 base_wall_collision_points[8] = {
#include "stg23/hidden_base_masonry_bright/base_wall_collision_points.inc"
};
static NJS_POINT3 base_wall_collision_normals[8] = {
#include "stg23/hidden_base_masonry_bright/base_wall_collision_normals.inc"
};
static NJS_MODEL base_wall_collision_model = {
#include "stg23/hidden_base_masonry_bright/base_wall_collision_model.inc"
};
NJS_OBJECT base_wall_collision = {
#include "stg23/hidden_base_masonry_bright/base_wall_collision.inc"
};
