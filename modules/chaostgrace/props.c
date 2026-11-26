/* CHAOSTGRACE.PRS's chunk models and chunk object pointers, from the disc.
 * its D lies between scene_assets.c and scenery_art.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgrace/props.h"

static Sint16 chao_race_neutral_props_low_flower_a_polygons[109] = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_a_polygons.inc"
};
static Sint32 chao_race_neutral_props_low_flower_a_vertices[115] = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_a_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_low_flower_a_model = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_a_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_low_flower_a = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_a.inc"
};
static Sint16 chao_race_neutral_props_low_flower_b_polygons[109] = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_b_polygons.inc"
};
static Sint32 chao_race_neutral_props_low_flower_b_vertices[115] = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_b_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_low_flower_b_model = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_b_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_low_flower_b = {
#include "chaostgrace/props/chao_race_neutral_props_low_flower_b.inc"
};
static Sint16 chao_race_neutral_props_leaf_cluster_polygons[169] = {
#include "chaostgrace/props/chao_race_neutral_props_leaf_cluster_polygons.inc"
};
static Sint32 chao_race_neutral_props_leaf_cluster_vertices[123] = {
#include "chaostgrace/props/chao_race_neutral_props_leaf_cluster_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_leaf_cluster_model = {
#include "chaostgrace/props/chao_race_neutral_props_leaf_cluster_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_leaf_cluster = {
#include "chaostgrace/props/chao_race_neutral_props_leaf_cluster.inc"
};
static Sint16 chao_race_neutral_props_broadleaf_plant_polygons[335] = {
#include "chaostgrace/props/chao_race_neutral_props_broadleaf_plant_polygons.inc"
};
static Sint32 chao_race_neutral_props_broadleaf_plant_vertices[187] = {
#include "chaostgrace/props/chao_race_neutral_props_broadleaf_plant_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_broadleaf_plant_model = {
#include "chaostgrace/props/chao_race_neutral_props_broadleaf_plant_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_broadleaf_plant = {
#include "chaostgrace/props/chao_race_neutral_props_broadleaf_plant.inc"
};
static Sint16 low_ring_polygons[287] = {
#include "chaostgrace/props/low_ring_polygons.inc"
};
static Sint32 low_ring_vertices[163] = {
#include "chaostgrace/props/low_ring_vertices.inc"
};
static NJS_CNK_MODEL low_ring_model = {
#include "chaostgrace/props/low_ring_model.inc"
};
static NJS_CNK_OBJECT low_ring = {
#include "chaostgrace/props/low_ring.inc"
};
static Sint16 chao_race_neutral_props_third_light_polygons[25] = {
#include "chaostgrace/props/chao_race_neutral_props_third_light_polygons.inc"
};
static Sint32 chao_race_neutral_props_third_light_vertices[19] = {
#include "chaostgrace/props/chao_race_neutral_props_third_light_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_third_light_model = {
#include "chaostgrace/props/chao_race_neutral_props_third_light_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_third_light = {
#include "chaostgrace/props/chao_race_neutral_props_third_light.inc"
};
static Sint16 chao_race_neutral_props_second_light_polygons[25] = {
#include "chaostgrace/props/chao_race_neutral_props_second_light_polygons.inc"
};
static Sint32 chao_race_neutral_props_second_light_vertices[19] = {
#include "chaostgrace/props/chao_race_neutral_props_second_light_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_second_light_model = {
#include "chaostgrace/props/chao_race_neutral_props_second_light_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_second_light = {
#include "chaostgrace/props/chao_race_neutral_props_second_light.inc"
};
static Sint16 chao_race_neutral_props_first_light_polygons[25] = {
#include "chaostgrace/props/chao_race_neutral_props_first_light_polygons.inc"
};
static Sint32 chao_race_neutral_props_first_light_vertices[19] = {
#include "chaostgrace/props/chao_race_neutral_props_first_light_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_first_light_model = {
#include "chaostgrace/props/chao_race_neutral_props_first_light_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_first_light = {
#include "chaostgrace/props/chao_race_neutral_props_first_light.inc"
};
static Sint16 chao_race_neutral_props_glowing_stalk_polygons[99] = {
#include "chaostgrace/props/chao_race_neutral_props_glowing_stalk_polygons.inc"
};
static Sint32 chao_race_neutral_props_glowing_stalk_vertices[111] = {
#include "chaostgrace/props/chao_race_neutral_props_glowing_stalk_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_glowing_stalk_model = {
#include "chaostgrace/props/chao_race_neutral_props_glowing_stalk_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_glowing_stalk = {
#include "chaostgrace/props/chao_race_neutral_props_glowing_stalk.inc"
};
static Sint16 chao_race_neutral_props_round_cap_short_polygons[503] = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_short_polygons.inc"
};
static Sint32 chao_race_neutral_props_round_cap_short_vertices[243] = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_short_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_round_cap_short_model = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_short_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_round_cap_short = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_short.inc"
};
static Sint16 chao_race_neutral_props_round_cap_medium_polygons[503] = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_medium_polygons.inc"
};
static Sint32 chao_race_neutral_props_round_cap_medium_vertices[243] = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_medium_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_round_cap_medium_model = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_medium_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_round_cap_medium = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_medium.inc"
};
static Sint16 chao_race_neutral_props_round_cap_tall_polygons[503] = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_tall_polygons.inc"
};
static Sint32 chao_race_neutral_props_round_cap_tall_vertices[243] = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_tall_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_round_cap_tall_model = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_tall_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_round_cap_tall = {
#include "chaostgrace/props/chao_race_neutral_props_round_cap_tall.inc"
};
static Sint16 alternate_cap_short_polygons[493] = {
#include "chaostgrace/props/alternate_cap_short_polygons.inc"
};
static Sint32 alternate_cap_short_vertices[247] = {
#include "chaostgrace/props/alternate_cap_short_vertices.inc"
};
static NJS_CNK_MODEL alternate_cap_short_model = {
#include "chaostgrace/props/alternate_cap_short_model.inc"
};
static NJS_CNK_OBJECT alternate_cap_short = {
#include "chaostgrace/props/alternate_cap_short.inc"
};
static Sint16 alternate_cap_medium_polygons[507] = {
#include "chaostgrace/props/alternate_cap_medium_polygons.inc"
};
static Sint32 alternate_cap_medium_vertices[255] = {
#include "chaostgrace/props/alternate_cap_medium_vertices.inc"
};
static NJS_CNK_MODEL alternate_cap_medium_model = {
#include "chaostgrace/props/alternate_cap_medium_model.inc"
};
static NJS_CNK_OBJECT alternate_cap_medium = {
#include "chaostgrace/props/alternate_cap_medium.inc"
};
static Sint16 alternate_cap_tall_polygons[513] = {
#include "chaostgrace/props/alternate_cap_tall_polygons.inc"
};
static Sint32 alternate_cap_tall_vertices[247] = {
#include "chaostgrace/props/alternate_cap_tall_vertices.inc"
};
static NJS_CNK_MODEL alternate_cap_tall_model = {
#include "chaostgrace/props/alternate_cap_tall_model.inc"
};
static NJS_CNK_OBJECT alternate_cap_tall = {
#include "chaostgrace/props/alternate_cap_tall.inc"
};
static Sint16 chao_race_neutral_props_short_board_polygons[259] = {
#include "chaostgrace/props/chao_race_neutral_props_short_board_polygons.inc"
};
static Sint32 chao_race_neutral_props_short_board_vertices[131] = {
#include "chaostgrace/props/chao_race_neutral_props_short_board_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_short_board_model = {
#include "chaostgrace/props/chao_race_neutral_props_short_board_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_short_board = {
#include "chaostgrace/props/chao_race_neutral_props_short_board.inc"
};
static Sint16 chao_race_neutral_props_tall_board_polygons[281] = {
#include "chaostgrace/props/chao_race_neutral_props_tall_board_polygons.inc"
};
static Sint32 chao_race_neutral_props_tall_board_vertices[131] = {
#include "chaostgrace/props/chao_race_neutral_props_tall_board_vertices.inc"
};
static NJS_CNK_MODEL chao_race_neutral_props_tall_board_model = {
#include "chaostgrace/props/chao_race_neutral_props_tall_board_model.inc"
};
static NJS_CNK_OBJECT chao_race_neutral_props_tall_board = {
#include "chaostgrace/props/chao_race_neutral_props_tall_board.inc"
};
NJS_CNK_OBJECT *race_neutral_props[RACE_NEUTRAL_PROPS] = {
#include "chaostgrace/props/race_neutral_props.inc"
};
static Sint16 chao_race_hero_props_low_flower_a_polygons[109] = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_a_polygons.inc"
};
static Sint32 chao_race_hero_props_low_flower_a_vertices[115] = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_a_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_low_flower_a_model = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_a_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_low_flower_a = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_a.inc"
};
static Sint16 chao_race_hero_props_low_flower_b_polygons[109] = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_b_polygons.inc"
};
static Sint32 chao_race_hero_props_low_flower_b_vertices[115] = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_b_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_low_flower_b_model = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_b_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_low_flower_b = {
#include "chaostgrace/props/chao_race_hero_props_low_flower_b.inc"
};
static Sint16 chao_race_hero_props_leaf_cluster_polygons[169] = {
#include "chaostgrace/props/chao_race_hero_props_leaf_cluster_polygons.inc"
};
static Sint32 chao_race_hero_props_leaf_cluster_vertices[123] = {
#include "chaostgrace/props/chao_race_hero_props_leaf_cluster_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_leaf_cluster_model = {
#include "chaostgrace/props/chao_race_hero_props_leaf_cluster_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_leaf_cluster = {
#include "chaostgrace/props/chao_race_hero_props_leaf_cluster.inc"
};
static Sint16 chao_race_hero_props_broadleaf_plant_polygons[335] = {
#include "chaostgrace/props/chao_race_hero_props_broadleaf_plant_polygons.inc"
};
static Sint32 chao_race_hero_props_broadleaf_plant_vertices[187] = {
#include "chaostgrace/props/chao_race_hero_props_broadleaf_plant_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_broadleaf_plant_model = {
#include "chaostgrace/props/chao_race_hero_props_broadleaf_plant_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_broadleaf_plant = {
#include "chaostgrace/props/chao_race_hero_props_broadleaf_plant.inc"
};
static Sint16 round_bloom_polygons[237] = {
#include "chaostgrace/props/round_bloom_polygons.inc"
};
static Sint32 round_bloom_vertices[135] = {
#include "chaostgrace/props/round_bloom_vertices.inc"
};
static NJS_CNK_MODEL round_bloom_model = {
#include "chaostgrace/props/round_bloom_model.inc"
};
static NJS_CNK_OBJECT round_bloom = {
#include "chaostgrace/props/round_bloom.inc"
};
static Sint16 chao_race_hero_props_third_light_polygons[25] = {
#include "chaostgrace/props/chao_race_hero_props_third_light_polygons.inc"
};
static Sint32 chao_race_hero_props_third_light_vertices[19] = {
#include "chaostgrace/props/chao_race_hero_props_third_light_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_third_light_model = {
#include "chaostgrace/props/chao_race_hero_props_third_light_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_third_light = {
#include "chaostgrace/props/chao_race_hero_props_third_light.inc"
};
static Sint16 chao_race_hero_props_second_light_polygons[25] = {
#include "chaostgrace/props/chao_race_hero_props_second_light_polygons.inc"
};
static Sint32 chao_race_hero_props_second_light_vertices[19] = {
#include "chaostgrace/props/chao_race_hero_props_second_light_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_second_light_model = {
#include "chaostgrace/props/chao_race_hero_props_second_light_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_second_light = {
#include "chaostgrace/props/chao_race_hero_props_second_light.inc"
};
static Sint16 chao_race_hero_props_first_light_polygons[25] = {
#include "chaostgrace/props/chao_race_hero_props_first_light_polygons.inc"
};
static Sint32 chao_race_hero_props_first_light_vertices[19] = {
#include "chaostgrace/props/chao_race_hero_props_first_light_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_first_light_model = {
#include "chaostgrace/props/chao_race_hero_props_first_light_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_first_light = {
#include "chaostgrace/props/chao_race_hero_props_first_light.inc"
};
static Sint16 chao_race_hero_props_glowing_stalk_polygons[99] = {
#include "chaostgrace/props/chao_race_hero_props_glowing_stalk_polygons.inc"
};
static Sint32 chao_race_hero_props_glowing_stalk_vertices[111] = {
#include "chaostgrace/props/chao_race_hero_props_glowing_stalk_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_glowing_stalk_model = {
#include "chaostgrace/props/chao_race_hero_props_glowing_stalk_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_glowing_stalk = {
#include "chaostgrace/props/chao_race_hero_props_glowing_stalk.inc"
};
static Sint16 chao_race_hero_props_round_cap_short_polygons[503] = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_short_polygons.inc"
};
static Sint32 chao_race_hero_props_round_cap_short_vertices[239] = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_short_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_round_cap_short_model = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_short_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_round_cap_short = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_short.inc"
};
static Sint16 chao_race_hero_props_round_cap_medium_polygons[497] = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_medium_polygons.inc"
};
static Sint32 chao_race_hero_props_round_cap_medium_vertices[239] = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_medium_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_round_cap_medium_model = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_medium_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_round_cap_medium = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_medium.inc"
};
static Sint16 chao_race_hero_props_round_cap_tall_polygons[503] = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_tall_polygons.inc"
};
static Sint32 chao_race_hero_props_round_cap_tall_vertices[243] = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_tall_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_round_cap_tall_model = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_tall_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_round_cap_tall = {
#include "chaostgrace/props/chao_race_hero_props_round_cap_tall.inc"
};
static Sint16 chao_race_hero_props_short_board_polygons[259] = {
#include "chaostgrace/props/chao_race_hero_props_short_board_polygons.inc"
};
static Sint32 chao_race_hero_props_short_board_vertices[131] = {
#include "chaostgrace/props/chao_race_hero_props_short_board_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_short_board_model = {
#include "chaostgrace/props/chao_race_hero_props_short_board_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_short_board = {
#include "chaostgrace/props/chao_race_hero_props_short_board.inc"
};
static Sint16 chao_race_hero_props_tall_board_polygons[281] = {
#include "chaostgrace/props/chao_race_hero_props_tall_board_polygons.inc"
};
static Sint32 chao_race_hero_props_tall_board_vertices[131] = {
#include "chaostgrace/props/chao_race_hero_props_tall_board_vertices.inc"
};
static NJS_CNK_MODEL chao_race_hero_props_tall_board_model = {
#include "chaostgrace/props/chao_race_hero_props_tall_board_model.inc"
};
static NJS_CNK_OBJECT chao_race_hero_props_tall_board = {
#include "chaostgrace/props/chao_race_hero_props_tall_board.inc"
};
NJS_CNK_OBJECT *race_hero_props[RACE_HERO_PROPS] = {
#include "chaostgrace/props/race_hero_props.inc"
};
static Sint16 spiked_fence_polygons[397] = {
#include "chaostgrace/props/spiked_fence_polygons.inc"
};
static Sint32 spiked_fence_vertices[271] = {
#include "chaostgrace/props/spiked_fence_vertices.inc"
};
static NJS_CNK_MODEL spiked_fence_model = {
#include "chaostgrace/props/spiked_fence_model.inc"
};
static NJS_CNK_OBJECT spiked_fence = {
#include "chaostgrace/props/spiked_fence.inc"
};
static Sint16 plain_gravestone_polygons[177] = {
#include "chaostgrace/props/plain_gravestone_polygons.inc"
};
static Sint32 plain_gravestone_vertices[179] = {
#include "chaostgrace/props/plain_gravestone_vertices.inc"
};
static NJS_CNK_MODEL plain_gravestone_model = {
#include "chaostgrace/props/plain_gravestone_model.inc"
};
static NJS_CNK_OBJECT plain_gravestone = {
#include "chaostgrace/props/plain_gravestone.inc"
};
static Sint16 cross_gravestone_polygons[349] = {
#include "chaostgrace/props/cross_gravestone_polygons.inc"
};
static Sint32 cross_gravestone_vertices[363] = {
#include "chaostgrace/props/cross_gravestone_vertices.inc"
};
static NJS_CNK_MODEL cross_gravestone_model = {
#include "chaostgrace/props/cross_gravestone_model.inc"
};
static NJS_CNK_OBJECT cross_gravestone = {
#include "chaostgrace/props/cross_gravestone.inc"
};
static Sint16 bare_tree_polygons[367] = {
#include "chaostgrace/props/bare_tree_polygons.inc"
};
static Sint32 bare_tree_vertices[183] = {
#include "chaostgrace/props/bare_tree_vertices.inc"
};
static NJS_CNK_MODEL bare_tree_model = {
#include "chaostgrace/props/bare_tree_model.inc"
};
static NJS_CNK_OBJECT bare_tree = {
#include "chaostgrace/props/bare_tree.inc"
};
static Sint16 broken_wall_polygons[313] = {
#include "chaostgrace/props/broken_wall_polygons.inc"
};
static Sint32 broken_wall_vertices[291] = {
#include "chaostgrace/props/broken_wall_vertices.inc"
};
static NJS_CNK_MODEL broken_wall_model = {
#include "chaostgrace/props/broken_wall_model.inc"
};
static NJS_CNK_OBJECT broken_wall = {
#include "chaostgrace/props/broken_wall.inc"
};
static Sint16 spiked_wall_polygons[781] = {
#include "chaostgrace/props/spiked_wall_polygons.inc"
};
static Sint32 spiked_wall_vertices[639] = {
#include "chaostgrace/props/spiked_wall_vertices.inc"
};
static NJS_CNK_MODEL spiked_wall_model = {
#include "chaostgrace/props/spiked_wall_model.inc"
};
static NJS_CNK_OBJECT spiked_wall = {
#include "chaostgrace/props/spiked_wall.inc"
};
static Sint16 hollow_tree_polygons[593] = {
#include "chaostgrace/props/hollow_tree_polygons.inc"
};
static Sint32 hollow_tree_vertices[359] = {
#include "chaostgrace/props/hollow_tree_vertices.inc"
};
static NJS_CNK_MODEL hollow_tree_model = {
#include "chaostgrace/props/hollow_tree_model.inc"
};
static NJS_CNK_OBJECT hollow_tree = {
#include "chaostgrace/props/hollow_tree.inc"
};
NJS_CNK_OBJECT *race_dark_props[RACE_DARK_PROPS] = {
#include "chaostgrace/props/race_dark_props.inc"
};
