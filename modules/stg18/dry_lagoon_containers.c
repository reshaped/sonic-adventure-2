/* STG18.PRS's texture lists, chunk models, Basic models, chunk model
 * pointers, texture list pointers, text pointers, container pieces, words,
 * model builds, collision shapes, floats, motions and their registry, from
 * the disc. its D lies between shelf.c and global_light_switch.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/motions.h"
#include "stg18/container_art.h"

static NJS_TEXNAME dark_container_shard_textures_names[3] = {
#include "stg18/dry_lagoon_containers/dark_container_shard_textures_names.inc"
};
static NJS_TEXLIST dark_container_shard_textures = {
#include "stg18/dry_lagoon_containers/dark_container_shard_textures.inc"
};
static Sint16 dark_container_piece_24_polygons[47] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_24_polygons.inc"
};
static Sint32 dark_container_piece_24_vertices[39] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_24_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_24 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_24.inc"
};
static NJS_CNK_OBJECT dark_container_piece_24_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_24_object.inc"
};
static Sint16 dark_container_piece_12_polygons[29] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_12_polygons.inc"
};
static Sint32 dark_container_piece_12_vertices[23] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_12_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_12 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_12.inc"
};
static NJS_CNK_OBJECT dark_container_piece_12_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_12_object.inc"
};
static Sint16 dark_container_piece_1_polygons[221] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_1_polygons.inc"
};
static Sint32 dark_container_piece_1_vertices[207] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_1_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_1 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_1.inc"
};
static NJS_CNK_OBJECT dark_container_piece_1_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_1_object.inc"
};
static Sint16 dark_container_piece_23_polygons[61] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_23_polygons.inc"
};
static Sint32 dark_container_piece_23_vertices[39] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_23_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_23 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_23.inc"
};
static NJS_CNK_OBJECT dark_container_piece_23_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_23_object.inc"
};
static Sint16 dark_container_piece_22_polygons[47] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_22_polygons.inc"
};
static Sint32 dark_container_piece_22_vertices[39] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_22_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_22 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_22.inc"
};
static NJS_CNK_OBJECT dark_container_piece_22_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_22_object.inc"
};
static Sint16 dark_container_piece_21_polygons[47] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_21_polygons.inc"
};
static Sint32 dark_container_piece_21_vertices[39] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_21_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_21 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_21.inc"
};
static NJS_CNK_OBJECT dark_container_piece_21_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_21_object.inc"
};
static Sint16 dark_container_piece_13_polygons[31] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_13_polygons.inc"
};
static Sint32 dark_container_piece_13_vertices[27] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_13_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_13 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_13.inc"
};
static NJS_CNK_OBJECT dark_container_piece_13_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_13_object.inc"
};
static Sint16 dark_container_piece_20_polygons[45] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_20_polygons.inc"
};
static Sint32 dark_container_piece_20_vertices[35] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_20_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_20 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_20.inc"
};
static NJS_CNK_OBJECT dark_container_piece_20_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_20_object.inc"
};
static Sint16 dark_container_piece_14_polygons[31] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_14_polygons.inc"
};
static Sint32 dark_container_piece_14_vertices[27] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_14_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_14 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_14.inc"
};
static NJS_CNK_OBJECT dark_container_piece_14_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_14_object.inc"
};
static Sint16 dark_container_piece_15_polygons[45] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_15_polygons.inc"
};
static Sint32 dark_container_piece_15_vertices[35] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_15_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_15 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_15.inc"
};
static NJS_CNK_OBJECT dark_container_piece_15_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_15_object.inc"
};
static Sint16 dark_container_piece_16_polygons[47] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_16_polygons.inc"
};
static Sint32 dark_container_piece_16_vertices[39] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_16_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_16 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_16.inc"
};
static NJS_CNK_OBJECT dark_container_piece_16_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_16_object.inc"
};
static Sint16 dark_container_piece_2_polygons[115] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_2_polygons.inc"
};
static Sint32 dark_container_piece_2_vertices[103] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_2_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_2 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_2.inc"
};
static NJS_CNK_OBJECT dark_container_piece_2_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_2_object.inc"
};
static Sint16 dark_container_piece_19_polygons[45] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_19_polygons.inc"
};
static Sint32 dark_container_piece_19_vertices[35] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_19_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_19 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_19.inc"
};
static NJS_CNK_OBJECT dark_container_piece_19_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_19_object.inc"
};
static Sint16 dark_container_piece_18_polygons[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_18_polygons.inc"
};
static Sint32 dark_container_piece_18_vertices[39] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_18_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_18 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_18.inc"
};
static NJS_CNK_OBJECT dark_container_piece_18_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_18_object.inc"
};
static Sint16 dark_container_piece_8_polygons[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_8_polygons.inc"
};
static Sint32 dark_container_piece_8_vertices[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_8_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_8 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_8.inc"
};
static NJS_CNK_OBJECT dark_container_piece_8_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_8_object.inc"
};
static Sint16 dark_container_piece_9_polygons[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_9_polygons.inc"
};
static Sint32 dark_container_piece_9_vertices[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_9_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_9 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_9.inc"
};
static NJS_CNK_OBJECT dark_container_piece_9_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_9_object.inc"
};
static Sint16 dark_container_piece_10_polygons[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_10_polygons.inc"
};
static Sint32 dark_container_piece_10_vertices[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_10_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_10 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_10.inc"
};
static NJS_CNK_OBJECT dark_container_piece_10_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_10_object.inc"
};
static Sint16 dark_container_piece_11_polygons[29] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_11_polygons.inc"
};
static Sint32 dark_container_piece_11_vertices[23] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_11_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_11 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_11.inc"
};
static NJS_CNK_OBJECT dark_container_piece_11_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_11_object.inc"
};
static Sint16 dark_container_piece_7_polygons[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_7_polygons.inc"
};
static Sint32 dark_container_piece_7_vertices[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_7_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_7 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_7.inc"
};
static NJS_CNK_OBJECT dark_container_piece_7_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_7_object.inc"
};
static Sint16 dark_container_piece_6_polygons[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_6_polygons.inc"
};
static Sint32 dark_container_piece_6_vertices[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_6_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_6 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_6.inc"
};
static NJS_CNK_OBJECT dark_container_piece_6_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_6_object.inc"
};
static Sint16 dark_container_piece_5_polygons[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_5_polygons.inc"
};
static Sint32 dark_container_piece_5_vertices[51] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_5_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_5 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_5.inc"
};
static NJS_CNK_OBJECT dark_container_piece_5_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_5_object.inc"
};
static Sint16 dark_container_piece_4_polygons[109] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_4_polygons.inc"
};
static Sint32 dark_container_piece_4_vertices[99] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_4_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_4 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_4.inc"
};
static NJS_CNK_OBJECT dark_container_piece_4_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_4_object.inc"
};
static Sint16 dark_container_piece_3_polygons[109] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_3_polygons.inc"
};
static Sint32 dark_container_piece_3_vertices[99] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_3_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_3 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_3.inc"
};
static NJS_CNK_OBJECT dark_container_piece_3_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_3_object.inc"
};
static Sint16 dark_container_piece_17_polygons[47] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_17_polygons.inc"
};
static Sint32 dark_container_piece_17_vertices[39] = {
#include "stg18/dry_lagoon_containers/dark_container_piece_17_vertices.inc"
};
static NJS_CNK_MODEL dark_container_piece_17 = {
#include "stg18/dry_lagoon_containers/dark_container_piece_17.inc"
};
static NJS_CNK_OBJECT dark_container_piece_17_object = {
#include "stg18/dry_lagoon_containers/dark_container_piece_17_object.inc"
};
static NJS_CNK_OBJECT dark_container_shards = {
#include "stg18/dry_lagoon_containers/dark_container_shards.inc"
};
static NJS_TEXNAME bright_container_shard_textures_names[3] = {
#include "stg18/dry_lagoon_containers/bright_container_shard_textures_names.inc"
};
static NJS_TEXLIST bright_container_shard_textures = {
#include "stg18/dry_lagoon_containers/bright_container_shard_textures.inc"
};
static Sint16 bright_container_piece_1_polygons[221] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_1_polygons.inc"
};
static Sint32 bright_container_piece_1_vertices[211] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_1_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_1 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_1.inc"
};
static NJS_CNK_OBJECT bright_container_piece_1_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_1_object.inc"
};
static Sint16 bright_container_piece_24_polygons[47] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_24_polygons.inc"
};
static Sint32 bright_container_piece_24_vertices[39] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_24_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_24 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_24.inc"
};
static NJS_CNK_OBJECT bright_container_piece_24_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_24_object.inc"
};
static Sint16 bright_container_piece_23_polygons[61] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_23_polygons.inc"
};
static Sint32 bright_container_piece_23_vertices[39] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_23_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_23 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_23.inc"
};
static NJS_CNK_OBJECT bright_container_piece_23_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_23_object.inc"
};
static Sint16 bright_container_piece_22_polygons[47] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_22_polygons.inc"
};
static Sint32 bright_container_piece_22_vertices[39] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_22_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_22 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_22.inc"
};
static NJS_CNK_OBJECT bright_container_piece_22_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_22_object.inc"
};
static Sint16 bright_container_piece_21_polygons[47] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_21_polygons.inc"
};
static Sint32 bright_container_piece_21_vertices[39] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_21_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_21 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_21.inc"
};
static NJS_CNK_OBJECT bright_container_piece_21_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_21_object.inc"
};
static Sint16 bright_container_piece_20_polygons[45] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_20_polygons.inc"
};
static Sint32 bright_container_piece_20_vertices[35] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_20_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_20 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_20.inc"
};
static NJS_CNK_OBJECT bright_container_piece_20_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_20_object.inc"
};
static Sint16 bright_container_piece_19_polygons[45] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_19_polygons.inc"
};
static Sint32 bright_container_piece_19_vertices[35] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_19_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_19 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_19.inc"
};
static NJS_CNK_OBJECT bright_container_piece_19_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_19_object.inc"
};
static Sint16 bright_container_piece_18_polygons[45] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_18_polygons.inc"
};
static Sint32 bright_container_piece_18_vertices[35] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_18_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_18 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_18.inc"
};
static NJS_CNK_OBJECT bright_container_piece_18_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_18_object.inc"
};
static Sint16 bright_container_piece_17_polygons[47] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_17_polygons.inc"
};
static Sint32 bright_container_piece_17_vertices[39] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_17_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_17 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_17.inc"
};
static NJS_CNK_OBJECT bright_container_piece_17_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_17_object.inc"
};
static Sint16 bright_container_piece_16_polygons[47] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_16_polygons.inc"
};
static Sint32 bright_container_piece_16_vertices[39] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_16_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_16 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_16.inc"
};
static NJS_CNK_OBJECT bright_container_piece_16_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_16_object.inc"
};
static Sint16 bright_container_piece_15_polygons[45] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_15_polygons.inc"
};
static Sint32 bright_container_piece_15_vertices[35] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_15_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_15 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_15.inc"
};
static NJS_CNK_OBJECT bright_container_piece_15_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_15_object.inc"
};
static Sint16 bright_container_piece_2_polygons[109] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_2_polygons.inc"
};
static Sint32 bright_container_piece_2_vertices[99] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_2_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_2 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_2.inc"
};
static NJS_CNK_OBJECT bright_container_piece_2_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_2_object.inc"
};
static Sint16 bright_container_piece_13_polygons[31] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_13_polygons.inc"
};
static Sint32 bright_container_piece_13_vertices[27] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_13_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_13 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_13.inc"
};
static NJS_CNK_OBJECT bright_container_piece_13_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_13_object.inc"
};
static Sint16 bright_container_piece_12_polygons[29] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_12_polygons.inc"
};
static Sint32 bright_container_piece_12_vertices[23] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_12_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_12 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_12.inc"
};
static NJS_CNK_OBJECT bright_container_piece_12_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_12_object.inc"
};
static Sint16 bright_container_piece_11_polygons[29] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_11_polygons.inc"
};
static Sint32 bright_container_piece_11_vertices[23] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_11_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_11 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_11.inc"
};
static NJS_CNK_OBJECT bright_container_piece_11_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_11_object.inc"
};
static Sint16 bright_container_piece_10_polygons[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_10_polygons.inc"
};
static Sint32 bright_container_piece_10_vertices[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_10_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_10 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_10.inc"
};
static NJS_CNK_OBJECT bright_container_piece_10_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_10_object.inc"
};
static Sint16 bright_container_piece_9_polygons[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_9_polygons.inc"
};
static Sint32 bright_container_piece_9_vertices[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_9_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_9 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_9.inc"
};
static NJS_CNK_OBJECT bright_container_piece_9_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_9_object.inc"
};
static Sint16 bright_container_piece_8_polygons[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_8_polygons.inc"
};
static Sint32 bright_container_piece_8_vertices[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_8_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_8 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_8.inc"
};
static NJS_CNK_OBJECT bright_container_piece_8_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_8_object.inc"
};
static Sint16 bright_container_piece_7_polygons[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_7_polygons.inc"
};
static Sint32 bright_container_piece_7_vertices[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_7_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_7 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_7.inc"
};
static NJS_CNK_OBJECT bright_container_piece_7_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_7_object.inc"
};
static Sint16 bright_container_piece_6_polygons[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_6_polygons.inc"
};
static Sint32 bright_container_piece_6_vertices[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_6_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_6 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_6.inc"
};
static NJS_CNK_OBJECT bright_container_piece_6_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_6_object.inc"
};
static Sint16 bright_container_piece_5_polygons[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_5_polygons.inc"
};
static Sint32 bright_container_piece_5_vertices[51] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_5_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_5 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_5.inc"
};
static NJS_CNK_OBJECT bright_container_piece_5_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_5_object.inc"
};
static Sint16 bright_container_piece_4_polygons[109] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_4_polygons.inc"
};
static Sint32 bright_container_piece_4_vertices[99] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_4_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_4 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_4.inc"
};
static NJS_CNK_OBJECT bright_container_piece_4_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_4_object.inc"
};
static Sint16 bright_container_piece_3_polygons[109] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_3_polygons.inc"
};
static Sint32 bright_container_piece_3_vertices[99] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_3_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_3 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_3.inc"
};
static NJS_CNK_OBJECT bright_container_piece_3_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_3_object.inc"
};
static Sint16 bright_container_piece_14_polygons[31] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_14_polygons.inc"
};
static Sint32 bright_container_piece_14_vertices[27] = {
#include "stg18/dry_lagoon_containers/bright_container_piece_14_vertices.inc"
};
static NJS_CNK_MODEL bright_container_piece_14 = {
#include "stg18/dry_lagoon_containers/bright_container_piece_14.inc"
};
static NJS_CNK_OBJECT bright_container_piece_14_object = {
#include "stg18/dry_lagoon_containers/bright_container_piece_14_object.inc"
};
static NJS_CNK_OBJECT bright_container_shards = {
#include "stg18/dry_lagoon_containers/bright_container_shards.inc"
};
static NJS_TEXNAME dark_container_textures_names[3] = {
#include "stg18/dry_lagoon_containers/dark_container_textures_names.inc"
};
static NJS_TEXLIST dark_container_textures = {
#include "stg18/dry_lagoon_containers/dark_container_textures.inc"
};
static Sint16 dark_container_mesh_polygons[349] = {
#include "stg18/dry_lagoon_containers/dark_container_mesh_polygons.inc"
};
static Sint32 dark_container_mesh_vertices[235] = {
#include "stg18/dry_lagoon_containers/dark_container_mesh_vertices.inc"
};
static NJS_CNK_MODEL dark_container_mesh = {
#include "stg18/dry_lagoon_containers/dark_container_mesh.inc"
};
static NJS_CNK_OBJECT dark_container_mesh_object = {
#include "stg18/dry_lagoon_containers/dark_container_mesh_object.inc"
};
static NJS_TEXNAME bright_container_textures_names[3] = {
#include "stg18/dry_lagoon_containers/bright_container_textures_names.inc"
};
static NJS_TEXLIST bright_container_textures = {
#include "stg18/dry_lagoon_containers/bright_container_textures.inc"
};
static Sint16 bright_container_mesh_polygons[349] = {
#include "stg18/dry_lagoon_containers/bright_container_mesh_polygons.inc"
};
static Sint32 bright_container_mesh_vertices[219] = {
#include "stg18/dry_lagoon_containers/bright_container_mesh_vertices.inc"
};
static NJS_CNK_MODEL bright_container_mesh = {
#include "stg18/dry_lagoon_containers/bright_container_mesh.inc"
};
static NJS_CNK_OBJECT bright_container_mesh_object = {
#include "stg18/dry_lagoon_containers/bright_container_mesh_object.inc"
};
static NJS_MATERIAL container_collision_materials[1] = {
#include "stg18/dry_lagoon_containers/container_collision_materials.inc"
};
static Sint16 container_collision_strips[19] = {
#include "stg18/dry_lagoon_containers/container_collision_strips.inc"
};
static NJS_MESHSET container_collision_faces[1] = {
#include "stg18/dry_lagoon_containers/container_collision_faces.inc"
};
static NJS_POINT3 container_collision_points[8] = {
#include "stg18/dry_lagoon_containers/container_collision_points.inc"
};
static NJS_POINT3 container_collision_normals[8] = {
#include "stg18/dry_lagoon_containers/container_collision_normals.inc"
};
static NJS_MODEL container_collision_model = {
#include "stg18/dry_lagoon_containers/container_collision_model.inc"
};
static NJS_OBJECT container_collision = {
#include "stg18/dry_lagoon_containers/container_collision.inc"
};
NJS_CNK_MODEL *container_models[CONTAINER_VARIANTS] = {
#include "stg18/dry_lagoon_containers/container_models.inc"
};
NJS_TEXLIST *container_texlists[5] = {
#include "stg18/dry_lagoon_containers/container_texlists.inc"
};
const char *container_variants[CONTAINER_VARIANTS] = {
#include "stg18/dry_lagoon_containers/container_variants.inc"
};
struct container_piece bright_container_pieces[CONTAINER_PIECES] = {
#include "stg18/dry_lagoon_containers/bright_container_pieces.inc"
};
struct container_piece dark_container_pieces[CONTAINER_PIECES] = {
#include "stg18/dry_lagoon_containers/dark_container_pieces.inc"
};
Bool container_built = {
#include "stg18/dry_lagoon_containers/container_built.inc"
};
struct model_build bright_container_builds[CONTAINER_BUILDS] = {
#include "stg18/dry_lagoon_containers/bright_container_builds.inc"
};
struct model_build dark_container_builds[CONTAINER_BUILDS] = {
#include "stg18/dry_lagoon_containers/dark_container_builds.inc"
};
struct collision_shape container_shape[CONTAINER_SHAPES] = {
#include "stg18/dry_lagoon_containers/container_shape.inc"
};
Float container_piece_spread = {
#include "stg18/dry_lagoon_containers/container_piece_spread.inc"
};
Float container_piece_speed = {
#include "stg18/dry_lagoon_containers/container_piece_speed.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_4_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_4_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_7_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_7_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_8_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_8_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_9_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_9_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_13_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_13_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_14_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_14_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_17_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_17_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_18_angles[14] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_18_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_19_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_19_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_23_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_23_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_27_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_27_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_28_angles[13] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_28_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_29_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_29_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_32_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_32_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_33_angles[13] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_33_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_37_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_37_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_38_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_38_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_39_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_39_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_41_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_41_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_44_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_44_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_51_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_51_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_53_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_53_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_56_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_56_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_57_angles[13] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_57_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_58_angles[13] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_58_angles.inc"
};
static NJS_MKEY_A upgrade_green_forest_motion_joint_60_angles[16] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_joint_60_angles.inc"
};
static NJS_MDATA2 upgrade_green_forest_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion_tracks.inc"
};
static NJS_MOTION upgrade_green_forest_motion = {
#include "stg18/dry_lagoon_containers/upgrade_green_forest_motion.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_7_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_7_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_8_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_8_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_9_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_9_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_13_angles[7] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_13_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_17_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_17_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_18_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_18_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_19_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_19_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_23_angles[5] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_23_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_27_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_27_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_28_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_28_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_29_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_29_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_32_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_32_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_33_angles[5] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_33_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_37_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_37_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_38_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_38_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_39_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_39_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_41_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_41_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_44_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_44_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_51_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_51_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_53_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_53_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_56_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_56_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_57_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_57_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_58_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_58_angles.inc"
};
static NJS_MKEY_A turtle_stand_motion_joint_60_angles[8] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_joint_60_angles.inc"
};
static NJS_MDATA2 turtle_stand_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion_tracks.inc"
};
static NJS_MOTION turtle_stand_motion = {
#include "stg18/dry_lagoon_containers/turtle_stand_motion.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_4_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_7_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_8_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_9_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_13_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_14_angles[26] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_17_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_18_angles[27] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_19_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_23_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_24_angles[13] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_27_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_31_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_32_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_33_angles[12] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_37_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_38_angles[27] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_39_angles[27] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_41_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_44_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_51_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_53_angles[32] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_56_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_57_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_58_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_stopped_action_motion_joint_60_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_stopped_action_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion_tracks.inc"
};
static NJS_MOTION rouge_stopped_action_motion = {
#include "stg18/dry_lagoon_containers/rouge_stopped_action_motion.inc"
};
static NJS_MKEY_F turtle_hop_motion_joint_2_position[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_2_position.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_4_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_4_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_7_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_7_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_8_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_8_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_9_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_9_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_13_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_13_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_17_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_17_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_18_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_18_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_19_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_19_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_23_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_23_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_24_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_24_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_27_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_27_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_31_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_31_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_32_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_32_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_33_angles[11] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_33_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_37_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_37_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_38_angles[13] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_38_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_39_angles[13] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_39_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_41_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_41_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_44_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_44_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_51_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_51_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_53_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_53_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_56_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_56_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_57_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_57_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_58_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_58_angles.inc"
};
static NJS_MKEY_A turtle_hop_motion_joint_60_angles[16] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_joint_60_angles.inc"
};
static NJS_MDATA2 turtle_hop_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion_tracks.inc"
};
static NJS_MOTION turtle_hop_motion = {
#include "stg18/dry_lagoon_containers/turtle_hop_motion.inc"
};
static NJS_MKEY_F rouge_float_motion_joint_2_position[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_4_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_7_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_8_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_9_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_13_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_14_angles[7] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_17_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_18_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_19_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_23_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_24_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_27_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_28_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_29_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_31_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_32_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_33_angles[6] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_37_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_38_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_39_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_40_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_41_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_44_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_51_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_53_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_56_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_57_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_58_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_59_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_float_motion_joint_60_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_float_motion_joint_60_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_float_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/rouge_float_motion_tracks.inc"
};
static NJS_MOTION rouge_float_motion = {
#include "stg18/dry_lagoon_containers/rouge_float_motion.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_2_position[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_4_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_7_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_8_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_9_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_13_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_14_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_17_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_18_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_19_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_23_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_24_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_27_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_28_angles[3] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_29_angles[3] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_31_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_32_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_33_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_33_angles.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_35_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_35_position.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_36_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_36_position.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_37_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_38_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_39_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_40_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_41_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_44_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_51_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_53_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_56_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_57_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_58_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_59_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_dive_start_motion_joint_60_position[3] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_dive_start_motion_joint_60_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_dive_start_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion_tracks.inc"
};
static NJS_MOTION rouge_dive_start_motion = {
#include "stg18/dry_lagoon_containers/rouge_dive_start_motion.inc"
};
static NJS_MKEY_F rouge_dive_motion_joint_2_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_4_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_7_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_8_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_9_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_13_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_14_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_17_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_18_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_19_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_23_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_24_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_27_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_28_angles[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_29_angles[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_31_angles[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_32_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_33_angles[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_37_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_38_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_39_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_40_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_41_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_44_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_51_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_53_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_56_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_57_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_58_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_59_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_dive_motion_joint_60_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_dive_motion_joint_60_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_dive_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion_tracks.inc"
};
static NJS_MOTION rouge_dive_motion = {
#include "stg18/dry_lagoon_containers/rouge_dive_motion.inc"
};
static NJS_MKEY_F rouge_surface_swim_motion_joint_2_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_4_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_7_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_8_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_9_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_13_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_14_angles[15] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_17_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_18_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_19_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_23_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_24_angles[15] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_27_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_28_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_29_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_31_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_32_angles[16] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_33_angles[7] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_37_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_38_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_39_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_40_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_41_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_44_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_51_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_53_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_56_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_57_angles[14] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_58_angles[14] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_59_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_surface_swim_motion_joint_60_position[9] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_surface_swim_motion_joint_60_angles[17] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_surface_swim_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion_tracks.inc"
};
static NJS_MOTION rouge_surface_swim_motion = {
#include "stg18/dry_lagoon_containers/rouge_surface_swim_motion.inc"
};
static NJS_MKEY_F rouge_surface_start_motion_joint_2_position[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_4_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_7_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_8_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_9_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_13_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_14_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_17_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_18_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_19_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_23_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_24_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_27_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_28_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_29_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_31_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_32_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_33_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_37_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_38_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_39_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_40_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_41_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_44_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_51_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_53_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_56_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_57_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_58_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_59_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_surface_start_motion_joint_60_position[3] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_surface_start_motion_joint_60_angles[8] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_surface_start_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion_tracks.inc"
};
static NJS_MOTION rouge_surface_start_motion = {
#include "stg18/dry_lagoon_containers/rouge_surface_start_motion.inc"
};
static NJS_MKEY_F rouge_surface_motion_joint_2_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_2_position.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_4_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_4_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_7_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_7_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_8_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_8_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_9_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_9_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_13_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_13_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_14_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_14_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_17_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_17_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_18_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_18_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_19_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_19_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_23_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_23_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_24_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_24_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_27_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_27_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_28_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_28_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_29_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_29_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_31_angles[2] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_31_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_32_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_32_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_33_angles[3] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_33_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_37_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_37_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_38_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_38_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_39_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_39_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_40_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_40_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_41_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_41_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_44_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_44_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_51_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_51_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_53_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_53_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_56_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_56_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_57_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_57_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_58_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_58_angles.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_59_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_59_angles.inc"
};
static NJS_MKEY_F rouge_surface_motion_joint_60_position[2] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_60_position.inc"
};
static NJS_MKEY_A rouge_surface_motion_joint_60_angles[4] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_joint_60_angles.inc"
};
static NJS_MDATA2 rouge_surface_motion_tracks[62] = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion_tracks.inc"
};
static NJS_MOTION rouge_surface_motion = {
#include "stg18/dry_lagoon_containers/rouge_surface_motion.inc"
};
struct motion_entry module_motions[11] = {
#include "stg18/dry_lagoon_containers/module_motions.inc"
};
