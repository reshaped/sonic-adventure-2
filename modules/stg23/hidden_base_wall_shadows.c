/* STG23.PRS's chunk models and texture lists, from the disc. its D lies
 * between wall.c and block.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/block_models.h"
#include "stg23/masonry_textures.h"

static Sint16 wall_shadow_1_polygons[17] = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_1_polygons.inc"
};
static Sint32 wall_shadow_1_vertices[23] = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_1_vertices.inc"
};
static NJS_CNK_MODEL wall_shadow_1_model = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_1_model.inc"
};
static NJS_CNK_OBJECT wall_shadow_1_object = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_1_object.inc"
};
static Sint16 wall_shadow_2_polygons[17] = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_2_polygons.inc"
};
static Sint32 wall_shadow_2_vertices[23] = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_2_vertices.inc"
};
static NJS_CNK_MODEL wall_shadow_2_model = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_2_model.inc"
};
static NJS_CNK_OBJECT wall_shadow_2_object = {
#include "stg23/hidden_base_wall_shadows/wall_shadow_2_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_block_textures[2] = {
#include "stg23/hidden_base_wall_shadows/hidden_base_masonry_block_textures.inc"
};
NJS_TEXLIST block_whole_texlist = {
#include "stg23/hidden_base_wall_shadows/block_whole_texlist.inc"
};
static Sint16 block_whole_polygons[83] = {
#include "stg23/hidden_base_wall_shadows/block_whole_polygons.inc"
};
static Sint32 block_whole_vertices[79] = {
#include "stg23/hidden_base_wall_shadows/block_whole_vertices.inc"
};
NJS_CNK_MODEL block_whole_model = {
#include "stg23/hidden_base_wall_shadows/block_whole_model.inc"
};
static NJS_CNK_OBJECT block_whole_object = {
#include "stg23/hidden_base_wall_shadows/block_whole_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_cracked_textures[2] = {
#include "stg23/hidden_base_wall_shadows/hidden_base_masonry_cracked_textures.inc"
};
NJS_TEXLIST block_cracked_texlist = {
#include "stg23/hidden_base_wall_shadows/block_cracked_texlist.inc"
};
static Sint16 block_cracked_polygons[83] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_polygons.inc"
};
static Sint32 block_cracked_vertices[79] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_vertices.inc"
};
NJS_CNK_MODEL block_cracked_model = {
#include "stg23/hidden_base_wall_shadows/block_cracked_model.inc"
};
static NJS_CNK_OBJECT block_cracked_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_split_textures[3] = {
#include "stg23/hidden_base_wall_shadows/hidden_base_masonry_split_textures.inc"
};
NJS_TEXLIST block_split_texlist = {
#include "stg23/hidden_base_wall_shadows/block_split_texlist.inc"
};
static Sint16 block_split_polygons[295] = {
#include "stg23/hidden_base_wall_shadows/block_split_polygons.inc"
};
static Sint32 block_split_vertices[291] = {
#include "stg23/hidden_base_wall_shadows/block_split_vertices.inc"
};
NJS_CNK_MODEL block_split_model = {
#include "stg23/hidden_base_wall_shadows/block_split_model.inc"
};
static NJS_CNK_OBJECT block_split_object = {
#include "stg23/hidden_base_wall_shadows/block_split_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_chips_a_textures[3] = {
#include "stg23/hidden_base_wall_shadows/hidden_base_masonry_chips_a_textures.inc"
};
NJS_TEXLIST block_chips_a_texlist = {
#include "stg23/hidden_base_wall_shadows/block_chips_a_texlist.inc"
};
static Sint16 block_cracked_shard_6_polygons[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_6_polygons.inc"
};
static Sint32 block_cracked_shard_6_vertices[35] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_6_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_6 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_6.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_6_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_6_object.inc"
};
static Sint16 block_cracked_shard_5_polygons[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_5_polygons.inc"
};
static Sint32 block_cracked_shard_5_vertices[43] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_5_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_5 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_5.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_5_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_5_object.inc"
};
static Sint16 block_cracked_shard_4_polygons[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_4_polygons.inc"
};
static Sint32 block_cracked_shard_4_vertices[43] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_4_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_4 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_4.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_4_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_4_object.inc"
};
static Sint16 block_cracked_shard_3_polygons[65] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_3_polygons.inc"
};
static Sint32 block_cracked_shard_3_vertices[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_3_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_3 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_3.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_3_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_3_object.inc"
};
static Sint16 block_cracked_shard_2_polygons[65] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_2_polygons.inc"
};
static Sint32 block_cracked_shard_2_vertices[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_2_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_2 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_2.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_2_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_2_object.inc"
};
static Sint16 block_cracked_shard_1_polygons[65] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_1_polygons.inc"
};
static Sint32 block_cracked_shard_1_vertices[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_1_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_1 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_1.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_1_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_1_object.inc"
};
static Sint16 block_cracked_shard_0_polygons[65] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_0_polygons.inc"
};
static Sint32 block_cracked_shard_0_vertices[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_0_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_0 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_0.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_0_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_0_object.inc"
};
static Sint16 block_cracked_shard_7_polygons[59] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_7_polygons.inc"
};
static Sint32 block_cracked_shard_7_vertices[51] = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_7_vertices.inc"
};
NJS_CNK_MODEL block_cracked_shard_7 = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_7.inc"
};
static NJS_CNK_OBJECT block_cracked_shard_7_object = {
#include "stg23/hidden_base_wall_shadows/block_cracked_shard_7_object.inc"
};
static NJS_CNK_OBJECT masonry_chips_a_root = {
#include "stg23/hidden_base_wall_shadows/masonry_chips_a_root.inc"
};
static NJS_TEXNAME hidden_base_masonry_base_textures[2] = {
#include "stg23/hidden_base_wall_shadows/hidden_base_masonry_base_textures.inc"
};
NJS_TEXLIST block_base_texlist = {
#include "stg23/hidden_base_wall_shadows/block_base_texlist.inc"
};
static Sint16 masonry_base_polygons[121] = {
#include "stg23/hidden_base_wall_shadows/masonry_base_polygons.inc"
};
static Sint32 masonry_base_vertices[111] = {
#include "stg23/hidden_base_wall_shadows/masonry_base_vertices.inc"
};
static NJS_CNK_MODEL masonry_base_model = {
#include "stg23/hidden_base_wall_shadows/masonry_base_model.inc"
};
static NJS_CNK_OBJECT masonry_base_object = {
#include "stg23/hidden_base_wall_shadows/masonry_base_object.inc"
};
static NJS_TEXNAME hidden_base_masonry_chips_b_textures[3] = {
#include "stg23/hidden_base_wall_shadows/hidden_base_masonry_chips_b_textures.inc"
};
NJS_TEXLIST block_chips_b_texlist = {
#include "stg23/hidden_base_wall_shadows/block_chips_b_texlist.inc"
};
static Sint16 block_split_shard_8_polygons[101] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_8_polygons.inc"
};
static Sint32 block_split_shard_8_vertices[79] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_8_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_8 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_8.inc"
};
static NJS_CNK_OBJECT block_split_shard_8_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_8_object.inc"
};
static Sint16 block_split_shard_0_polygons[107] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_0_polygons.inc"
};
static Sint32 block_split_shard_0_vertices[99] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_0_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_0 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_0.inc"
};
static NJS_CNK_OBJECT block_split_shard_0_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_0_object.inc"
};
static Sint16 block_split_shard_1_polygons[121] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_1_polygons.inc"
};
static Sint32 block_split_shard_1_vertices[99] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_1_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_1 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_1.inc"
};
static NJS_CNK_OBJECT block_split_shard_1_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_1_object.inc"
};
static Sint16 block_split_shard_2_polygons[169] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_2_polygons.inc"
};
static Sint32 block_split_shard_2_vertices[139] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_2_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_2 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_2.inc"
};
static NJS_CNK_OBJECT block_split_shard_2_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_2_object.inc"
};
static Sint16 block_split_shard_3_polygons[119] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_3_polygons.inc"
};
static Sint32 block_split_shard_3_vertices[107] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_3_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_3 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_3.inc"
};
static NJS_CNK_OBJECT block_split_shard_3_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_3_object.inc"
};
static Sint16 block_split_shard_4_polygons[77] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_4_polygons.inc"
};
static Sint32 block_split_shard_4_vertices[71] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_4_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_4 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_4.inc"
};
static NJS_CNK_OBJECT block_split_shard_4_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_4_object.inc"
};
static Sint16 block_split_shard_7_polygons[139] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_7_polygons.inc"
};
static Sint32 block_split_shard_7_vertices[115] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_7_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_7 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_7.inc"
};
static NJS_CNK_OBJECT block_split_shard_7_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_7_object.inc"
};
static Sint16 block_split_shard_5_polygons[75] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_5_polygons.inc"
};
static Sint32 block_split_shard_5_vertices[67] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_5_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_5 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_5.inc"
};
static NJS_CNK_OBJECT block_split_shard_5_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_5_object.inc"
};
static Sint16 block_split_shard_9_polygons[209] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_9_polygons.inc"
};
static Sint32 block_split_shard_9_vertices[175] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_9_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_9 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_9.inc"
};
static NJS_CNK_OBJECT block_split_shard_9_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_9_object.inc"
};
static Sint16 block_split_shard_10_polygons[77] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_10_polygons.inc"
};
static Sint32 block_split_shard_10_vertices[75] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_10_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_10 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_10.inc"
};
static NJS_CNK_OBJECT block_split_shard_10_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_10_object.inc"
};
static Sint16 block_split_shard_6_polygons[45] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_6_polygons.inc"
};
static Sint32 block_split_shard_6_vertices[35] = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_6_vertices.inc"
};
NJS_CNK_MODEL block_split_shard_6 = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_6.inc"
};
static NJS_CNK_OBJECT block_split_shard_6_object = {
#include "stg23/hidden_base_wall_shadows/block_split_shard_6_object.inc"
};
static NJS_CNK_OBJECT masonry_chips_b_root = {
#include "stg23/hidden_base_wall_shadows/masonry_chips_b_root.inc"
};
