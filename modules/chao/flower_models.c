/* CHAO.PRS's chunk models and chunk object pointers, from the disc. its D
 * lies between drive.c and dat_0c6191a4.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/flower_models.h"
#include "chao/garden_decor.h"
#include "chao/orchard_art.h"

static Sint16 mating_flower_a_polygons[109] = {
#include "chao/flower_models/mating_flower_a_polygons.inc"
};
static Sint32 mating_flower_a_vertices[115] = {
#include "chao/flower_models/mating_flower_a_vertices.inc"
};
static NJS_CNK_MODEL mating_flower_a_model = {
#include "chao/flower_models/mating_flower_a_model.inc"
};
NJS_CNK_OBJECT mating_flower_a = {
#include "chao/flower_models/mating_flower_a.inc"
};
static Sint16 mating_flower_b_polygons[109] = {
#include "chao/flower_models/mating_flower_b_polygons.inc"
};
static Sint32 mating_flower_b_vertices[115] = {
#include "chao/flower_models/mating_flower_b_vertices.inc"
};
static NJS_CNK_MODEL mating_flower_b_model = {
#include "chao/flower_models/mating_flower_b_model.inc"
};
NJS_CNK_OBJECT mating_flower_b = {
#include "chao/flower_models/mating_flower_b.inc"
};
static Sint16 garden_oval_polygons[163] = {
#include "chao/flower_models/garden_oval_polygons.inc"
};
static Sint32 garden_oval_vertices[143] = {
#include "chao/flower_models/garden_oval_vertices.inc"
};
static NJS_CNK_MODEL garden_oval_mesh = {
#include "chao/flower_models/garden_oval_mesh.inc"
};
NJS_CNK_OBJECT garden_oval_model = {
#include "chao/flower_models/garden_oval_model.inc"
};
static Sint16 fruit_chao_polygons[281] = {
#include "chao/flower_models/fruit_chao_polygons.inc"
};
static Sint32 fruit_chao_vertices[201] = {
#include "chao/flower_models/fruit_chao_vertices.inc"
};
static NJS_CNK_MODEL fruit_chao_model = {
#include "chao/flower_models/fruit_chao_model.inc"
};
static NJS_CNK_OBJECT fruit_chao = {
#include "chao/flower_models/fruit_chao.inc"
};
static Sint16 fruit_dark_polygons[249] = {
#include "chao/flower_models/fruit_dark_polygons.inc"
};
static Sint32 fruit_dark_vertices[153] = {
#include "chao/flower_models/fruit_dark_vertices.inc"
};
static NJS_CNK_MODEL fruit_dark_model = {
#include "chao/flower_models/fruit_dark_model.inc"
};
static NJS_CNK_OBJECT fruit_dark = {
#include "chao/flower_models/fruit_dark.inc"
};
static Sint16 fruit_kind_16_polygons[221] = {
#include "chao/flower_models/fruit_kind_16_polygons.inc"
};
static Sint32 fruit_kind_16_vertices[219] = {
#include "chao/flower_models/fruit_kind_16_vertices.inc"
};
static NJS_CNK_MODEL fruit_kind_16_model = {
#include "chao/flower_models/fruit_kind_16_model.inc"
};
static NJS_CNK_OBJECT fruit_kind_16 = {
#include "chao/flower_models/fruit_kind_16.inc"
};
static Sint16 fruit_heart_polygons[245] = {
#include "chao/flower_models/fruit_heart_polygons.inc"
};
static Sint32 fruit_heart_vertices[207] = {
#include "chao/flower_models/fruit_heart_vertices.inc"
};
static NJS_CNK_MODEL fruit_heart_model = {
#include "chao/flower_models/fruit_heart_model.inc"
};
static NJS_CNK_OBJECT fruit_heart = {
#include "chao/flower_models/fruit_heart.inc"
};
static Sint16 fruit_hero_polygons[215] = {
#include "chao/flower_models/fruit_hero_polygons.inc"
};
static Sint32 fruit_hero_vertices[135] = {
#include "chao/flower_models/fruit_hero_vertices.inc"
};
static NJS_CNK_MODEL fruit_hero_model = {
#include "chao/flower_models/fruit_hero_model.inc"
};
static NJS_CNK_OBJECT fruit_hero = {
#include "chao/flower_models/fruit_hero.inc"
};
static Sint16 fruit_round_polygons[213] = {
#include "chao/flower_models/fruit_round_polygons.inc"
};
static Sint32 fruit_round_vertices[171] = {
#include "chao/flower_models/fruit_round_vertices.inc"
};
static NJS_CNK_MODEL fruit_round_model = {
#include "chao/flower_models/fruit_round_model.inc"
};
static NJS_CNK_OBJECT fruit_round = {
#include "chao/flower_models/fruit_round.inc"
};
static Sint16 fruit_kind_18_polygons[207] = {
#include "chao/flower_models/fruit_kind_18_polygons.inc"
};
static Sint32 fruit_kind_18_vertices[195] = {
#include "chao/flower_models/fruit_kind_18_vertices.inc"
};
static NJS_CNK_MODEL fruit_kind_18_model = {
#include "chao/flower_models/fruit_kind_18_model.inc"
};
static NJS_CNK_OBJECT fruit_kind_18 = {
#include "chao/flower_models/fruit_kind_18.inc"
};
static Sint16 fruit_kind_17_polygons[247] = {
#include "chao/flower_models/fruit_kind_17_polygons.inc"
};
static Sint32 fruit_kind_17_vertices[213] = {
#include "chao/flower_models/fruit_kind_17_vertices.inc"
};
static NJS_CNK_MODEL fruit_kind_17_model = {
#include "chao/flower_models/fruit_kind_17_model.inc"
};
static NJS_CNK_OBJECT fruit_kind_17 = {
#include "chao/flower_models/fruit_kind_17.inc"
};
static Sint16 fruit_triangle_polygons[189] = {
#include "chao/flower_models/fruit_triangle_polygons.inc"
};
static Sint32 fruit_triangle_vertices[147] = {
#include "chao/flower_models/fruit_triangle_vertices.inc"
};
static NJS_CNK_MODEL fruit_triangle_model = {
#include "chao/flower_models/fruit_triangle_model.inc"
};
static NJS_CNK_OBJECT fruit_triangle = {
#include "chao/flower_models/fruit_triangle.inc"
};
static Sint16 fruit_square_polygons[293] = {
#include "chao/flower_models/fruit_square_polygons.inc"
};
static Sint32 fruit_square_vertices[195] = {
#include "chao/flower_models/fruit_square_vertices.inc"
};
static NJS_CNK_MODEL fruit_square_model = {
#include "chao/flower_models/fruit_square_model.inc"
};
static NJS_CNK_OBJECT fruit_square = {
#include "chao/flower_models/fruit_square.inc"
};
static Sint16 fruit_neutral_1_polygons[169] = {
#include "chao/flower_models/fruit_neutral_1_polygons.inc"
};
static Sint32 fruit_neutral_1_vertices[123] = {
#include "chao/flower_models/fruit_neutral_1_vertices.inc"
};
static NJS_CNK_MODEL fruit_neutral_1_model = {
#include "chao/flower_models/fruit_neutral_1_model.inc"
};
static NJS_CNK_OBJECT fruit_neutral_1 = {
#include "chao/flower_models/fruit_neutral_1.inc"
};
static Sint16 fruit_neutral_2_polygons[219] = {
#include "chao/flower_models/fruit_neutral_2_polygons.inc"
};
static Sint32 fruit_neutral_2_vertices[123] = {
#include "chao/flower_models/fruit_neutral_2_vertices.inc"
};
static NJS_CNK_MODEL fruit_neutral_2_model = {
#include "chao/flower_models/fruit_neutral_2_model.inc"
};
static NJS_CNK_OBJECT fruit_neutral_2 = {
#include "chao/flower_models/fruit_neutral_2.inc"
};
static Sint16 fruit_neutral_3_polygons[203] = {
#include "chao/flower_models/fruit_neutral_3_polygons.inc"
};
static Sint32 fruit_neutral_3_vertices[135] = {
#include "chao/flower_models/fruit_neutral_3_vertices.inc"
};
static NJS_CNK_MODEL fruit_neutral_3_model = {
#include "chao/flower_models/fruit_neutral_3_model.inc"
};
static NJS_CNK_OBJECT fruit_neutral_3 = {
#include "chao/flower_models/fruit_neutral_3.inc"
};
static Sint16 fruit_hero_1_polygons[203] = {
#include "chao/flower_models/fruit_hero_1_polygons.inc"
};
static Sint32 fruit_hero_1_vertices[135] = {
#include "chao/flower_models/fruit_hero_1_vertices.inc"
};
static NJS_CNK_MODEL fruit_hero_1_model = {
#include "chao/flower_models/fruit_hero_1_model.inc"
};
static NJS_CNK_OBJECT fruit_hero_1 = {
#include "chao/flower_models/fruit_hero_1.inc"
};
static Sint16 fruit_hero_2_polygons[203] = {
#include "chao/flower_models/fruit_hero_2_polygons.inc"
};
static Sint32 fruit_hero_2_vertices[135] = {
#include "chao/flower_models/fruit_hero_2_vertices.inc"
};
static NJS_CNK_MODEL fruit_hero_2_model = {
#include "chao/flower_models/fruit_hero_2_model.inc"
};
static NJS_CNK_OBJECT fruit_hero_2 = {
#include "chao/flower_models/fruit_hero_2.inc"
};
static Sint16 fruit_hero_3_polygons[203] = {
#include "chao/flower_models/fruit_hero_3_polygons.inc"
};
static Sint32 fruit_hero_3_vertices[135] = {
#include "chao/flower_models/fruit_hero_3_vertices.inc"
};
static NJS_CNK_MODEL fruit_hero_3_model = {
#include "chao/flower_models/fruit_hero_3_model.inc"
};
static NJS_CNK_OBJECT fruit_hero_3 = {
#include "chao/flower_models/fruit_hero_3.inc"
};
static Sint16 fruit_dark_1_polygons[205] = {
#include "chao/flower_models/fruit_dark_1_polygons.inc"
};
static Sint32 fruit_dark_1_vertices[123] = {
#include "chao/flower_models/fruit_dark_1_vertices.inc"
};
static NJS_CNK_MODEL fruit_dark_1_model = {
#include "chao/flower_models/fruit_dark_1_model.inc"
};
static NJS_CNK_OBJECT fruit_dark_1 = {
#include "chao/flower_models/fruit_dark_1.inc"
};
static Sint16 fruit_dark_2_polygons[187] = {
#include "chao/flower_models/fruit_dark_2_polygons.inc"
};
static Sint32 fruit_dark_2_vertices[105] = {
#include "chao/flower_models/fruit_dark_2_vertices.inc"
};
static NJS_CNK_MODEL fruit_dark_2_model = {
#include "chao/flower_models/fruit_dark_2_model.inc"
};
static NJS_CNK_OBJECT fruit_dark_2 = {
#include "chao/flower_models/fruit_dark_2.inc"
};
static Sint16 fruit_dark_3_polygons[163] = {
#include "chao/flower_models/fruit_dark_3_polygons.inc"
};
static Sint32 fruit_dark_3_vertices[123] = {
#include "chao/flower_models/fruit_dark_3_vertices.inc"
};
static NJS_CNK_MODEL fruit_dark_3_model = {
#include "chao/flower_models/fruit_dark_3_model.inc"
};
static NJS_CNK_OBJECT fruit_dark_3 = {
#include "chao/flower_models/fruit_dark_3.inc"
};
static Sint16 fruit_neutral_garden_polygons[163] = {
#include "chao/flower_models/fruit_neutral_garden_polygons.inc"
};
static Sint32 fruit_neutral_garden_vertices[123] = {
#include "chao/flower_models/fruit_neutral_garden_vertices.inc"
};
static NJS_CNK_MODEL fruit_neutral_garden_model = {
#include "chao/flower_models/fruit_neutral_garden_model.inc"
};
static NJS_CNK_OBJECT fruit_neutral_garden = {
#include "chao/flower_models/fruit_neutral_garden.inc"
};
static Sint16 fruit_hero_garden_polygons[219] = {
#include "chao/flower_models/fruit_hero_garden_polygons.inc"
};
static Sint32 fruit_hero_garden_vertices[123] = {
#include "chao/flower_models/fruit_hero_garden_vertices.inc"
};
static NJS_CNK_MODEL fruit_hero_garden_model = {
#include "chao/flower_models/fruit_hero_garden_model.inc"
};
static NJS_CNK_OBJECT fruit_hero_garden = {
#include "chao/flower_models/fruit_hero_garden.inc"
};
static Sint16 fruit_dark_garden_polygons[163] = {
#include "chao/flower_models/fruit_dark_garden_polygons.inc"
};
static Sint32 fruit_dark_garden_vertices[123] = {
#include "chao/flower_models/fruit_dark_garden_vertices.inc"
};
static NJS_CNK_MODEL fruit_dark_garden_model = {
#include "chao/flower_models/fruit_dark_garden_model.inc"
};
static NJS_CNK_OBJECT fruit_dark_garden = {
#include "chao/flower_models/fruit_dark_garden.inc"
};
NJS_CNK_OBJECT *garden_fruit_models[GARDEN_FRUIT_TYPES] = {
#include "chao/flower_models/garden_fruit_models.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_sprout_node_6 = {
#include "chao/flower_models/tree_neutral_1_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_sprout_node_5 = {
#include "chao/flower_models/tree_neutral_1_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_sprout_node_4 = {
#include "chao/flower_models/tree_neutral_1_sprout_node_4.inc"
};
static Sint16 tree_neutral_1_sprout_node_3_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_sprout_node_3_polygons.inc"
};
static Sint32 tree_neutral_1_sprout_node_3_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_sprout_node_3_model = {
#include "chao/flower_models/tree_neutral_1_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_sprout_node_3 = {
#include "chao/flower_models/tree_neutral_1_sprout_node_3.inc"
};
static Sint16 tree_neutral_1_sprout_node_2_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_sprout_node_2_polygons.inc"
};
static Sint32 tree_neutral_1_sprout_node_2_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_sprout_node_2_model = {
#include "chao/flower_models/tree_neutral_1_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_sprout_node_2 = {
#include "chao/flower_models/tree_neutral_1_sprout_node_2.inc"
};
static Sint16 tree_neutral_1_sprout_node_1_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_sprout_node_1_polygons.inc"
};
static Sint32 tree_neutral_1_sprout_node_1_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_sprout_node_1_model = {
#include "chao/flower_models/tree_neutral_1_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_sprout_node_1 = {
#include "chao/flower_models/tree_neutral_1_sprout_node_1.inc"
};
static Sint16 tree_neutral_1_sprout_polygons[239] = {
#include "chao/flower_models/tree_neutral_1_sprout_polygons.inc"
};
static Sint32 tree_neutral_1_sprout_vertices[147] = {
#include "chao/flower_models/tree_neutral_1_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_sprout_model = {
#include "chao/flower_models/tree_neutral_1_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_sprout = {
#include "chao/flower_models/tree_neutral_1_sprout.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_mature_node_6 = {
#include "chao/flower_models/tree_neutral_1_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_mature_node_5 = {
#include "chao/flower_models/tree_neutral_1_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_mature_node_4 = {
#include "chao/flower_models/tree_neutral_1_mature_node_4.inc"
};
static Sint16 tree_neutral_1_mature_node_3_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_mature_node_3_polygons.inc"
};
static Sint32 tree_neutral_1_mature_node_3_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_mature_node_3_model = {
#include "chao/flower_models/tree_neutral_1_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_mature_node_3 = {
#include "chao/flower_models/tree_neutral_1_mature_node_3.inc"
};
static Sint16 tree_neutral_1_mature_node_2_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_mature_node_2_polygons.inc"
};
static Sint32 tree_neutral_1_mature_node_2_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_mature_node_2_model = {
#include "chao/flower_models/tree_neutral_1_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_mature_node_2 = {
#include "chao/flower_models/tree_neutral_1_mature_node_2.inc"
};
static Sint16 tree_neutral_1_mature_node_1_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_mature_node_1_polygons.inc"
};
static Sint32 tree_neutral_1_mature_node_1_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_mature_node_1_model = {
#include "chao/flower_models/tree_neutral_1_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_mature_node_1 = {
#include "chao/flower_models/tree_neutral_1_mature_node_1.inc"
};
static Sint16 tree_neutral_1_mature_polygons[239] = {
#include "chao/flower_models/tree_neutral_1_mature_polygons.inc"
};
static Sint32 tree_neutral_1_mature_vertices[147] = {
#include "chao/flower_models/tree_neutral_1_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_mature_model = {
#include "chao/flower_models/tree_neutral_1_mature_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_mature = {
#include "chao/flower_models/tree_neutral_1_mature.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_withered_node_6 = {
#include "chao/flower_models/tree_neutral_1_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_withered_node_5 = {
#include "chao/flower_models/tree_neutral_1_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_withered_node_4 = {
#include "chao/flower_models/tree_neutral_1_withered_node_4.inc"
};
static Sint16 tree_neutral_1_withered_node_3_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_withered_node_3_polygons.inc"
};
static Sint32 tree_neutral_1_withered_node_3_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_withered_node_3_model = {
#include "chao/flower_models/tree_neutral_1_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_withered_node_3 = {
#include "chao/flower_models/tree_neutral_1_withered_node_3.inc"
};
static Sint16 tree_neutral_1_withered_node_2_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_withered_node_2_polygons.inc"
};
static Sint32 tree_neutral_1_withered_node_2_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_withered_node_2_model = {
#include "chao/flower_models/tree_neutral_1_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_withered_node_2 = {
#include "chao/flower_models/tree_neutral_1_withered_node_2.inc"
};
static Sint16 tree_neutral_1_withered_node_1_polygons[83] = {
#include "chao/flower_models/tree_neutral_1_withered_node_1_polygons.inc"
};
static Sint32 tree_neutral_1_withered_node_1_vertices[55] = {
#include "chao/flower_models/tree_neutral_1_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_withered_node_1_model = {
#include "chao/flower_models/tree_neutral_1_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_withered_node_1 = {
#include "chao/flower_models/tree_neutral_1_withered_node_1.inc"
};
static Sint16 tree_neutral_1_withered_polygons[239] = {
#include "chao/flower_models/tree_neutral_1_withered_polygons.inc"
};
static Sint32 tree_neutral_1_withered_vertices[147] = {
#include "chao/flower_models/tree_neutral_1_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_1_withered_model = {
#include "chao/flower_models/tree_neutral_1_withered_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_1_withered = {
#include "chao/flower_models/tree_neutral_1_withered.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_sprout_node_6 = {
#include "chao/flower_models/tree_neutral_2_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_sprout_node_5 = {
#include "chao/flower_models/tree_neutral_2_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_sprout_node_4 = {
#include "chao/flower_models/tree_neutral_2_sprout_node_4.inc"
};
static Sint16 tree_neutral_2_sprout_node_3_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_sprout_node_3_polygons.inc"
};
static Sint32 tree_neutral_2_sprout_node_3_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_sprout_node_3_model = {
#include "chao/flower_models/tree_neutral_2_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_sprout_node_3 = {
#include "chao/flower_models/tree_neutral_2_sprout_node_3.inc"
};
static Sint16 tree_neutral_2_sprout_node_2_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_sprout_node_2_polygons.inc"
};
static Sint32 tree_neutral_2_sprout_node_2_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_sprout_node_2_model = {
#include "chao/flower_models/tree_neutral_2_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_sprout_node_2 = {
#include "chao/flower_models/tree_neutral_2_sprout_node_2.inc"
};
static Sint16 tree_neutral_2_sprout_node_1_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_sprout_node_1_polygons.inc"
};
static Sint32 tree_neutral_2_sprout_node_1_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_sprout_node_1_model = {
#include "chao/flower_models/tree_neutral_2_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_sprout_node_1 = {
#include "chao/flower_models/tree_neutral_2_sprout_node_1.inc"
};
static Sint16 tree_neutral_2_sprout_polygons[129] = {
#include "chao/flower_models/tree_neutral_2_sprout_polygons.inc"
};
static Sint32 tree_neutral_2_sprout_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_sprout_model = {
#include "chao/flower_models/tree_neutral_2_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_sprout = {
#include "chao/flower_models/tree_neutral_2_sprout.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_mature_node_6 = {
#include "chao/flower_models/tree_neutral_2_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_mature_node_5 = {
#include "chao/flower_models/tree_neutral_2_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_mature_node_4 = {
#include "chao/flower_models/tree_neutral_2_mature_node_4.inc"
};
static Sint16 tree_neutral_2_mature_node_3_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_mature_node_3_polygons.inc"
};
static Sint32 tree_neutral_2_mature_node_3_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_mature_node_3_model = {
#include "chao/flower_models/tree_neutral_2_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_mature_node_3 = {
#include "chao/flower_models/tree_neutral_2_mature_node_3.inc"
};
static Sint16 tree_neutral_2_mature_node_2_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_mature_node_2_polygons.inc"
};
static Sint32 tree_neutral_2_mature_node_2_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_mature_node_2_model = {
#include "chao/flower_models/tree_neutral_2_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_mature_node_2 = {
#include "chao/flower_models/tree_neutral_2_mature_node_2.inc"
};
static Sint16 tree_neutral_2_mature_node_1_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_mature_node_1_polygons.inc"
};
static Sint32 tree_neutral_2_mature_node_1_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_mature_node_1_model = {
#include "chao/flower_models/tree_neutral_2_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_mature_node_1 = {
#include "chao/flower_models/tree_neutral_2_mature_node_1.inc"
};
static Sint16 tree_neutral_2_mature_polygons[129] = {
#include "chao/flower_models/tree_neutral_2_mature_polygons.inc"
};
static Sint32 tree_neutral_2_mature_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_mature_model = {
#include "chao/flower_models/tree_neutral_2_mature_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_mature = {
#include "chao/flower_models/tree_neutral_2_mature.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_withered_node_6 = {
#include "chao/flower_models/tree_neutral_2_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_withered_node_5 = {
#include "chao/flower_models/tree_neutral_2_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_withered_node_4 = {
#include "chao/flower_models/tree_neutral_2_withered_node_4.inc"
};
static Sint16 tree_neutral_2_withered_node_3_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_withered_node_3_polygons.inc"
};
static Sint32 tree_neutral_2_withered_node_3_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_withered_node_3_model = {
#include "chao/flower_models/tree_neutral_2_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_withered_node_3 = {
#include "chao/flower_models/tree_neutral_2_withered_node_3.inc"
};
static Sint16 tree_neutral_2_withered_node_2_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_withered_node_2_polygons.inc"
};
static Sint32 tree_neutral_2_withered_node_2_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_withered_node_2_model = {
#include "chao/flower_models/tree_neutral_2_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_withered_node_2 = {
#include "chao/flower_models/tree_neutral_2_withered_node_2.inc"
};
static Sint16 tree_neutral_2_withered_node_1_polygons[69] = {
#include "chao/flower_models/tree_neutral_2_withered_node_1_polygons.inc"
};
static Sint32 tree_neutral_2_withered_node_1_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_withered_node_1_model = {
#include "chao/flower_models/tree_neutral_2_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_withered_node_1 = {
#include "chao/flower_models/tree_neutral_2_withered_node_1.inc"
};
static Sint16 tree_neutral_2_withered_polygons[123] = {
#include "chao/flower_models/tree_neutral_2_withered_polygons.inc"
};
static Sint32 tree_neutral_2_withered_vertices[67] = {
#include "chao/flower_models/tree_neutral_2_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_2_withered_model = {
#include "chao/flower_models/tree_neutral_2_withered_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_2_withered = {
#include "chao/flower_models/tree_neutral_2_withered.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_sprout_node_6 = {
#include "chao/flower_models/tree_neutral_3_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_sprout_node_5 = {
#include "chao/flower_models/tree_neutral_3_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_sprout_node_4 = {
#include "chao/flower_models/tree_neutral_3_sprout_node_4.inc"
};
static Sint16 tree_neutral_3_sprout_node_3_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_sprout_node_3_polygons.inc"
};
static Sint32 tree_neutral_3_sprout_node_3_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_sprout_node_3_model = {
#include "chao/flower_models/tree_neutral_3_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_sprout_node_3 = {
#include "chao/flower_models/tree_neutral_3_sprout_node_3.inc"
};
static Sint16 tree_neutral_3_sprout_node_2_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_sprout_node_2_polygons.inc"
};
static Sint32 tree_neutral_3_sprout_node_2_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_sprout_node_2_model = {
#include "chao/flower_models/tree_neutral_3_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_sprout_node_2 = {
#include "chao/flower_models/tree_neutral_3_sprout_node_2.inc"
};
static Sint16 tree_neutral_3_sprout_node_1_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_sprout_node_1_polygons.inc"
};
static Sint32 tree_neutral_3_sprout_node_1_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_sprout_node_1_model = {
#include "chao/flower_models/tree_neutral_3_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_sprout_node_1 = {
#include "chao/flower_models/tree_neutral_3_sprout_node_1.inc"
};
static Sint16 tree_neutral_3_sprout_polygons[123] = {
#include "chao/flower_models/tree_neutral_3_sprout_polygons.inc"
};
static Sint32 tree_neutral_3_sprout_vertices[67] = {
#include "chao/flower_models/tree_neutral_3_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_sprout_model = {
#include "chao/flower_models/tree_neutral_3_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_sprout = {
#include "chao/flower_models/tree_neutral_3_sprout.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_mature_node_6 = {
#include "chao/flower_models/tree_neutral_3_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_mature_node_5 = {
#include "chao/flower_models/tree_neutral_3_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_mature_node_4 = {
#include "chao/flower_models/tree_neutral_3_mature_node_4.inc"
};
static Sint16 tree_neutral_3_mature_node_3_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_mature_node_3_polygons.inc"
};
static Sint32 tree_neutral_3_mature_node_3_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_mature_node_3_model = {
#include "chao/flower_models/tree_neutral_3_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_mature_node_3 = {
#include "chao/flower_models/tree_neutral_3_mature_node_3.inc"
};
static Sint16 tree_neutral_3_mature_node_2_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_mature_node_2_polygons.inc"
};
static Sint32 tree_neutral_3_mature_node_2_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_mature_node_2_model = {
#include "chao/flower_models/tree_neutral_3_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_mature_node_2 = {
#include "chao/flower_models/tree_neutral_3_mature_node_2.inc"
};
static Sint16 tree_neutral_3_mature_node_1_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_mature_node_1_polygons.inc"
};
static Sint32 tree_neutral_3_mature_node_1_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_mature_node_1_model = {
#include "chao/flower_models/tree_neutral_3_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_mature_node_1 = {
#include "chao/flower_models/tree_neutral_3_mature_node_1.inc"
};
static Sint16 tree_neutral_3_mature_polygons[137] = {
#include "chao/flower_models/tree_neutral_3_mature_polygons.inc"
};
static Sint32 tree_neutral_3_mature_vertices[67] = {
#include "chao/flower_models/tree_neutral_3_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_mature_model = {
#include "chao/flower_models/tree_neutral_3_mature_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_mature = {
#include "chao/flower_models/tree_neutral_3_mature.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_withered_node_6 = {
#include "chao/flower_models/tree_neutral_3_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_withered_node_5 = {
#include "chao/flower_models/tree_neutral_3_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_withered_node_4 = {
#include "chao/flower_models/tree_neutral_3_withered_node_4.inc"
};
static Sint16 tree_neutral_3_withered_node_3_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_withered_node_3_polygons.inc"
};
static Sint32 tree_neutral_3_withered_node_3_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_withered_node_3_model = {
#include "chao/flower_models/tree_neutral_3_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_withered_node_3 = {
#include "chao/flower_models/tree_neutral_3_withered_node_3.inc"
};
static Sint16 tree_neutral_3_withered_node_2_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_withered_node_2_polygons.inc"
};
static Sint32 tree_neutral_3_withered_node_2_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_withered_node_2_model = {
#include "chao/flower_models/tree_neutral_3_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_withered_node_2 = {
#include "chao/flower_models/tree_neutral_3_withered_node_2.inc"
};
static Sint16 tree_neutral_3_withered_node_1_polygons[69] = {
#include "chao/flower_models/tree_neutral_3_withered_node_1_polygons.inc"
};
static Sint32 tree_neutral_3_withered_node_1_vertices[51] = {
#include "chao/flower_models/tree_neutral_3_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_withered_node_1_model = {
#include "chao/flower_models/tree_neutral_3_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_withered_node_1 = {
#include "chao/flower_models/tree_neutral_3_withered_node_1.inc"
};
static Sint16 tree_neutral_3_withered_polygons[123] = {
#include "chao/flower_models/tree_neutral_3_withered_polygons.inc"
};
static Sint32 tree_neutral_3_withered_vertices[67] = {
#include "chao/flower_models/tree_neutral_3_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_neutral_3_withered_model = {
#include "chao/flower_models/tree_neutral_3_withered_model.inc"
};
static NJS_CNK_OBJECT tree_neutral_3_withered = {
#include "chao/flower_models/tree_neutral_3_withered.inc"
};
static NJS_CNK_OBJECT tree_hero_1_sprout_node_6 = {
#include "chao/flower_models/tree_hero_1_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_1_sprout_node_5 = {
#include "chao/flower_models/tree_hero_1_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_1_sprout_node_4 = {
#include "chao/flower_models/tree_hero_1_sprout_node_4.inc"
};
static Sint16 tree_hero_1_sprout_node_3_polygons[69] = {
#include "chao/flower_models/tree_hero_1_sprout_node_3_polygons.inc"
};
static Sint32 tree_hero_1_sprout_node_3_vertices[67] = {
#include "chao/flower_models/tree_hero_1_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_sprout_node_3_model = {
#include "chao/flower_models/tree_hero_1_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_sprout_node_3 = {
#include "chao/flower_models/tree_hero_1_sprout_node_3.inc"
};
static Sint16 tree_hero_1_sprout_node_2_polygons[69] = {
#include "chao/flower_models/tree_hero_1_sprout_node_2_polygons.inc"
};
static Sint32 tree_hero_1_sprout_node_2_vertices[67] = {
#include "chao/flower_models/tree_hero_1_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_sprout_node_2_model = {
#include "chao/flower_models/tree_hero_1_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_sprout_node_2 = {
#include "chao/flower_models/tree_hero_1_sprout_node_2.inc"
};
static Sint16 tree_hero_1_sprout_node_1_polygons[69] = {
#include "chao/flower_models/tree_hero_1_sprout_node_1_polygons.inc"
};
static Sint32 tree_hero_1_sprout_node_1_vertices[67] = {
#include "chao/flower_models/tree_hero_1_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_sprout_node_1_model = {
#include "chao/flower_models/tree_hero_1_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_sprout_node_1 = {
#include "chao/flower_models/tree_hero_1_sprout_node_1.inc"
};
static Sint16 tree_hero_1_sprout_polygons[139] = {
#include "chao/flower_models/tree_hero_1_sprout_polygons.inc"
};
static Sint32 tree_hero_1_sprout_vertices[71] = {
#include "chao/flower_models/tree_hero_1_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_sprout_model = {
#include "chao/flower_models/tree_hero_1_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_sprout = {
#include "chao/flower_models/tree_hero_1_sprout.inc"
};
static NJS_CNK_OBJECT tree_hero_1_mature_node_6 = {
#include "chao/flower_models/tree_hero_1_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_1_mature_node_5 = {
#include "chao/flower_models/tree_hero_1_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_1_mature_node_4 = {
#include "chao/flower_models/tree_hero_1_mature_node_4.inc"
};
static Sint16 tree_hero_1_mature_node_3_polygons[69] = {
#include "chao/flower_models/tree_hero_1_mature_node_3_polygons.inc"
};
static Sint32 tree_hero_1_mature_node_3_vertices[67] = {
#include "chao/flower_models/tree_hero_1_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_mature_node_3_model = {
#include "chao/flower_models/tree_hero_1_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_mature_node_3 = {
#include "chao/flower_models/tree_hero_1_mature_node_3.inc"
};
static Sint16 tree_hero_1_mature_node_2_polygons[69] = {
#include "chao/flower_models/tree_hero_1_mature_node_2_polygons.inc"
};
static Sint32 tree_hero_1_mature_node_2_vertices[67] = {
#include "chao/flower_models/tree_hero_1_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_mature_node_2_model = {
#include "chao/flower_models/tree_hero_1_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_mature_node_2 = {
#include "chao/flower_models/tree_hero_1_mature_node_2.inc"
};
static Sint16 tree_hero_1_mature_node_1_polygons[69] = {
#include "chao/flower_models/tree_hero_1_mature_node_1_polygons.inc"
};
static Sint32 tree_hero_1_mature_node_1_vertices[67] = {
#include "chao/flower_models/tree_hero_1_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_mature_node_1_model = {
#include "chao/flower_models/tree_hero_1_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_mature_node_1 = {
#include "chao/flower_models/tree_hero_1_mature_node_1.inc"
};
static Sint16 tree_hero_1_mature_polygons[139] = {
#include "chao/flower_models/tree_hero_1_mature_polygons.inc"
};
static Sint32 tree_hero_1_mature_vertices[71] = {
#include "chao/flower_models/tree_hero_1_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_mature_model = {
#include "chao/flower_models/tree_hero_1_mature_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_mature = {
#include "chao/flower_models/tree_hero_1_mature.inc"
};
static NJS_CNK_OBJECT tree_hero_1_withered_node_6 = {
#include "chao/flower_models/tree_hero_1_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_1_withered_node_5 = {
#include "chao/flower_models/tree_hero_1_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_1_withered_node_4 = {
#include "chao/flower_models/tree_hero_1_withered_node_4.inc"
};
static Sint16 tree_hero_1_withered_node_3_polygons[69] = {
#include "chao/flower_models/tree_hero_1_withered_node_3_polygons.inc"
};
static Sint32 tree_hero_1_withered_node_3_vertices[67] = {
#include "chao/flower_models/tree_hero_1_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_withered_node_3_model = {
#include "chao/flower_models/tree_hero_1_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_withered_node_3 = {
#include "chao/flower_models/tree_hero_1_withered_node_3.inc"
};
static Sint16 tree_hero_1_withered_node_2_polygons[69] = {
#include "chao/flower_models/tree_hero_1_withered_node_2_polygons.inc"
};
static Sint32 tree_hero_1_withered_node_2_vertices[67] = {
#include "chao/flower_models/tree_hero_1_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_withered_node_2_model = {
#include "chao/flower_models/tree_hero_1_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_withered_node_2 = {
#include "chao/flower_models/tree_hero_1_withered_node_2.inc"
};
static Sint16 tree_hero_1_withered_node_1_polygons[69] = {
#include "chao/flower_models/tree_hero_1_withered_node_1_polygons.inc"
};
static Sint32 tree_hero_1_withered_node_1_vertices[67] = {
#include "chao/flower_models/tree_hero_1_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_withered_node_1_model = {
#include "chao/flower_models/tree_hero_1_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_withered_node_1 = {
#include "chao/flower_models/tree_hero_1_withered_node_1.inc"
};
static Sint16 tree_hero_1_withered_polygons[131] = {
#include "chao/flower_models/tree_hero_1_withered_polygons.inc"
};
static Sint32 tree_hero_1_withered_vertices[71] = {
#include "chao/flower_models/tree_hero_1_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_1_withered_model = {
#include "chao/flower_models/tree_hero_1_withered_model.inc"
};
static NJS_CNK_OBJECT tree_hero_1_withered = {
#include "chao/flower_models/tree_hero_1_withered.inc"
};
static NJS_CNK_OBJECT tree_hero_2_sprout_node_6 = {
#include "chao/flower_models/tree_hero_2_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_2_sprout_node_5 = {
#include "chao/flower_models/tree_hero_2_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_2_sprout_node_4 = {
#include "chao/flower_models/tree_hero_2_sprout_node_4.inc"
};
static Sint16 tree_hero_2_sprout_node_3_polygons[57] = {
#include "chao/flower_models/tree_hero_2_sprout_node_3_polygons.inc"
};
static Sint32 tree_hero_2_sprout_node_3_vertices[43] = {
#include "chao/flower_models/tree_hero_2_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_sprout_node_3_model = {
#include "chao/flower_models/tree_hero_2_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_sprout_node_3 = {
#include "chao/flower_models/tree_hero_2_sprout_node_3.inc"
};
static Sint16 tree_hero_2_sprout_node_2_polygons[57] = {
#include "chao/flower_models/tree_hero_2_sprout_node_2_polygons.inc"
};
static Sint32 tree_hero_2_sprout_node_2_vertices[43] = {
#include "chao/flower_models/tree_hero_2_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_sprout_node_2_model = {
#include "chao/flower_models/tree_hero_2_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_sprout_node_2 = {
#include "chao/flower_models/tree_hero_2_sprout_node_2.inc"
};
static Sint16 tree_hero_2_sprout_node_1_polygons[57] = {
#include "chao/flower_models/tree_hero_2_sprout_node_1_polygons.inc"
};
static Sint32 tree_hero_2_sprout_node_1_vertices[43] = {
#include "chao/flower_models/tree_hero_2_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_sprout_node_1_model = {
#include "chao/flower_models/tree_hero_2_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_sprout_node_1 = {
#include "chao/flower_models/tree_hero_2_sprout_node_1.inc"
};
static Sint16 tree_hero_2_sprout_polygons[201] = {
#include "chao/flower_models/tree_hero_2_sprout_polygons.inc"
};
static Sint32 tree_hero_2_sprout_vertices[135] = {
#include "chao/flower_models/tree_hero_2_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_sprout_model = {
#include "chao/flower_models/tree_hero_2_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_sprout = {
#include "chao/flower_models/tree_hero_2_sprout.inc"
};
static NJS_CNK_OBJECT tree_hero_2_mature_node_6 = {
#include "chao/flower_models/tree_hero_2_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_2_mature_node_5 = {
#include "chao/flower_models/tree_hero_2_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_2_mature_node_4 = {
#include "chao/flower_models/tree_hero_2_mature_node_4.inc"
};
static Sint16 tree_hero_2_mature_node_3_polygons[57] = {
#include "chao/flower_models/tree_hero_2_mature_node_3_polygons.inc"
};
static Sint32 tree_hero_2_mature_node_3_vertices[43] = {
#include "chao/flower_models/tree_hero_2_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_mature_node_3_model = {
#include "chao/flower_models/tree_hero_2_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_mature_node_3 = {
#include "chao/flower_models/tree_hero_2_mature_node_3.inc"
};
static Sint16 tree_hero_2_mature_node_2_polygons[57] = {
#include "chao/flower_models/tree_hero_2_mature_node_2_polygons.inc"
};
static Sint32 tree_hero_2_mature_node_2_vertices[43] = {
#include "chao/flower_models/tree_hero_2_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_mature_node_2_model = {
#include "chao/flower_models/tree_hero_2_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_mature_node_2 = {
#include "chao/flower_models/tree_hero_2_mature_node_2.inc"
};
static Sint16 tree_hero_2_mature_node_1_polygons[57] = {
#include "chao/flower_models/tree_hero_2_mature_node_1_polygons.inc"
};
static Sint32 tree_hero_2_mature_node_1_vertices[43] = {
#include "chao/flower_models/tree_hero_2_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_mature_node_1_model = {
#include "chao/flower_models/tree_hero_2_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_mature_node_1 = {
#include "chao/flower_models/tree_hero_2_mature_node_1.inc"
};
static Sint16 tree_hero_2_mature_polygons[201] = {
#include "chao/flower_models/tree_hero_2_mature_polygons.inc"
};
static Sint32 tree_hero_2_mature_vertices[135] = {
#include "chao/flower_models/tree_hero_2_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_mature_model = {
#include "chao/flower_models/tree_hero_2_mature_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_mature = {
#include "chao/flower_models/tree_hero_2_mature.inc"
};
static NJS_CNK_OBJECT tree_hero_2_withered_node_6 = {
#include "chao/flower_models/tree_hero_2_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_2_withered_node_5 = {
#include "chao/flower_models/tree_hero_2_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_2_withered_node_4 = {
#include "chao/flower_models/tree_hero_2_withered_node_4.inc"
};
static Sint16 tree_hero_2_withered_node_3_polygons[57] = {
#include "chao/flower_models/tree_hero_2_withered_node_3_polygons.inc"
};
static Sint32 tree_hero_2_withered_node_3_vertices[43] = {
#include "chao/flower_models/tree_hero_2_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_withered_node_3_model = {
#include "chao/flower_models/tree_hero_2_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_withered_node_3 = {
#include "chao/flower_models/tree_hero_2_withered_node_3.inc"
};
static Sint16 tree_hero_2_withered_node_2_polygons[57] = {
#include "chao/flower_models/tree_hero_2_withered_node_2_polygons.inc"
};
static Sint32 tree_hero_2_withered_node_2_vertices[43] = {
#include "chao/flower_models/tree_hero_2_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_withered_node_2_model = {
#include "chao/flower_models/tree_hero_2_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_withered_node_2 = {
#include "chao/flower_models/tree_hero_2_withered_node_2.inc"
};
static Sint16 tree_hero_2_withered_node_1_polygons[57] = {
#include "chao/flower_models/tree_hero_2_withered_node_1_polygons.inc"
};
static Sint32 tree_hero_2_withered_node_1_vertices[43] = {
#include "chao/flower_models/tree_hero_2_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_withered_node_1_model = {
#include "chao/flower_models/tree_hero_2_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_withered_node_1 = {
#include "chao/flower_models/tree_hero_2_withered_node_1.inc"
};
static Sint16 tree_hero_2_withered_polygons[201] = {
#include "chao/flower_models/tree_hero_2_withered_polygons.inc"
};
static Sint32 tree_hero_2_withered_vertices[135] = {
#include "chao/flower_models/tree_hero_2_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_2_withered_model = {
#include "chao/flower_models/tree_hero_2_withered_model.inc"
};
static NJS_CNK_OBJECT tree_hero_2_withered = {
#include "chao/flower_models/tree_hero_2_withered.inc"
};
static NJS_CNK_OBJECT tree_hero_3_sprout_node_6 = {
#include "chao/flower_models/tree_hero_3_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_3_sprout_node_5 = {
#include "chao/flower_models/tree_hero_3_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_3_sprout_node_4 = {
#include "chao/flower_models/tree_hero_3_sprout_node_4.inc"
};
static Sint16 tree_hero_3_sprout_node_3_polygons[83] = {
#include "chao/flower_models/tree_hero_3_sprout_node_3_polygons.inc"
};
static Sint32 tree_hero_3_sprout_node_3_vertices[55] = {
#include "chao/flower_models/tree_hero_3_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_sprout_node_3_model = {
#include "chao/flower_models/tree_hero_3_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_sprout_node_3 = {
#include "chao/flower_models/tree_hero_3_sprout_node_3.inc"
};
static Sint16 tree_hero_3_sprout_node_2_polygons[83] = {
#include "chao/flower_models/tree_hero_3_sprout_node_2_polygons.inc"
};
static Sint32 tree_hero_3_sprout_node_2_vertices[55] = {
#include "chao/flower_models/tree_hero_3_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_sprout_node_2_model = {
#include "chao/flower_models/tree_hero_3_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_sprout_node_2 = {
#include "chao/flower_models/tree_hero_3_sprout_node_2.inc"
};
static Sint16 tree_hero_3_sprout_node_1_polygons[83] = {
#include "chao/flower_models/tree_hero_3_sprout_node_1_polygons.inc"
};
static Sint32 tree_hero_3_sprout_node_1_vertices[55] = {
#include "chao/flower_models/tree_hero_3_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_sprout_node_1_model = {
#include "chao/flower_models/tree_hero_3_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_sprout_node_1 = {
#include "chao/flower_models/tree_hero_3_sprout_node_1.inc"
};
static Sint16 tree_hero_3_sprout_polygons[197] = {
#include "chao/flower_models/tree_hero_3_sprout_polygons.inc"
};
static Sint32 tree_hero_3_sprout_vertices[111] = {
#include "chao/flower_models/tree_hero_3_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_sprout_model = {
#include "chao/flower_models/tree_hero_3_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_sprout = {
#include "chao/flower_models/tree_hero_3_sprout.inc"
};
static NJS_CNK_OBJECT tree_hero_3_mature_node_6 = {
#include "chao/flower_models/tree_hero_3_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_3_mature_node_5 = {
#include "chao/flower_models/tree_hero_3_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_3_mature_node_4 = {
#include "chao/flower_models/tree_hero_3_mature_node_4.inc"
};
static Sint16 tree_hero_3_mature_node_3_polygons[83] = {
#include "chao/flower_models/tree_hero_3_mature_node_3_polygons.inc"
};
static Sint32 tree_hero_3_mature_node_3_vertices[55] = {
#include "chao/flower_models/tree_hero_3_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_mature_node_3_model = {
#include "chao/flower_models/tree_hero_3_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_mature_node_3 = {
#include "chao/flower_models/tree_hero_3_mature_node_3.inc"
};
static Sint16 tree_hero_3_mature_node_2_polygons[83] = {
#include "chao/flower_models/tree_hero_3_mature_node_2_polygons.inc"
};
static Sint32 tree_hero_3_mature_node_2_vertices[55] = {
#include "chao/flower_models/tree_hero_3_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_mature_node_2_model = {
#include "chao/flower_models/tree_hero_3_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_mature_node_2 = {
#include "chao/flower_models/tree_hero_3_mature_node_2.inc"
};
static Sint16 tree_hero_3_mature_node_1_polygons[83] = {
#include "chao/flower_models/tree_hero_3_mature_node_1_polygons.inc"
};
static Sint32 tree_hero_3_mature_node_1_vertices[55] = {
#include "chao/flower_models/tree_hero_3_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_mature_node_1_model = {
#include "chao/flower_models/tree_hero_3_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_mature_node_1 = {
#include "chao/flower_models/tree_hero_3_mature_node_1.inc"
};
static Sint16 tree_hero_3_mature_polygons[189] = {
#include "chao/flower_models/tree_hero_3_mature_polygons.inc"
};
static Sint32 tree_hero_3_mature_vertices[111] = {
#include "chao/flower_models/tree_hero_3_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_mature_model = {
#include "chao/flower_models/tree_hero_3_mature_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_mature = {
#include "chao/flower_models/tree_hero_3_mature.inc"
};
static NJS_CNK_OBJECT tree_hero_3_withered_node_6 = {
#include "chao/flower_models/tree_hero_3_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_hero_3_withered_node_5 = {
#include "chao/flower_models/tree_hero_3_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_hero_3_withered_node_4 = {
#include "chao/flower_models/tree_hero_3_withered_node_4.inc"
};
static Sint16 tree_hero_3_withered_node_3_polygons[83] = {
#include "chao/flower_models/tree_hero_3_withered_node_3_polygons.inc"
};
static Sint32 tree_hero_3_withered_node_3_vertices[55] = {
#include "chao/flower_models/tree_hero_3_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_withered_node_3_model = {
#include "chao/flower_models/tree_hero_3_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_withered_node_3 = {
#include "chao/flower_models/tree_hero_3_withered_node_3.inc"
};
static Sint16 tree_hero_3_withered_node_2_polygons[83] = {
#include "chao/flower_models/tree_hero_3_withered_node_2_polygons.inc"
};
static Sint32 tree_hero_3_withered_node_2_vertices[55] = {
#include "chao/flower_models/tree_hero_3_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_withered_node_2_model = {
#include "chao/flower_models/tree_hero_3_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_withered_node_2 = {
#include "chao/flower_models/tree_hero_3_withered_node_2.inc"
};
static Sint16 tree_hero_3_withered_node_1_polygons[83] = {
#include "chao/flower_models/tree_hero_3_withered_node_1_polygons.inc"
};
static Sint32 tree_hero_3_withered_node_1_vertices[55] = {
#include "chao/flower_models/tree_hero_3_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_withered_node_1_model = {
#include "chao/flower_models/tree_hero_3_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_withered_node_1 = {
#include "chao/flower_models/tree_hero_3_withered_node_1.inc"
};
static Sint16 tree_hero_3_withered_polygons[197] = {
#include "chao/flower_models/tree_hero_3_withered_polygons.inc"
};
static Sint32 tree_hero_3_withered_vertices[111] = {
#include "chao/flower_models/tree_hero_3_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_hero_3_withered_model = {
#include "chao/flower_models/tree_hero_3_withered_model.inc"
};
static NJS_CNK_OBJECT tree_hero_3_withered = {
#include "chao/flower_models/tree_hero_3_withered.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout_node_7 = {
#include "chao/flower_models/tree_dark_1_sprout_node_7.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout_node_6 = {
#include "chao/flower_models/tree_dark_1_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout_node_5 = {
#include "chao/flower_models/tree_dark_1_sprout_node_5.inc"
};
static Sint16 tree_dark_1_sprout_node_4_polygons[53] = {
#include "chao/flower_models/tree_dark_1_sprout_node_4_polygons.inc"
};
static Sint32 tree_dark_1_sprout_node_4_vertices[39] = {
#include "chao/flower_models/tree_dark_1_sprout_node_4_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_sprout_node_4_model = {
#include "chao/flower_models/tree_dark_1_sprout_node_4_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout_node_4 = {
#include "chao/flower_models/tree_dark_1_sprout_node_4.inc"
};
static Sint16 tree_dark_1_sprout_node_3_polygons[53] = {
#include "chao/flower_models/tree_dark_1_sprout_node_3_polygons.inc"
};
static Sint32 tree_dark_1_sprout_node_3_vertices[39] = {
#include "chao/flower_models/tree_dark_1_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_sprout_node_3_model = {
#include "chao/flower_models/tree_dark_1_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout_node_3 = {
#include "chao/flower_models/tree_dark_1_sprout_node_3.inc"
};
static Sint16 tree_dark_1_sprout_node_2_polygons[53] = {
#include "chao/flower_models/tree_dark_1_sprout_node_2_polygons.inc"
};
static Sint32 tree_dark_1_sprout_node_2_vertices[39] = {
#include "chao/flower_models/tree_dark_1_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_sprout_node_2_model = {
#include "chao/flower_models/tree_dark_1_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout_node_2 = {
#include "chao/flower_models/tree_dark_1_sprout_node_2.inc"
};
static Sint16 tree_dark_1_sprout_node_1_polygons[53] = {
#include "chao/flower_models/tree_dark_1_sprout_node_1_polygons.inc"
};
static Sint32 tree_dark_1_sprout_node_1_vertices[39] = {
#include "chao/flower_models/tree_dark_1_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_sprout_node_1_model = {
#include "chao/flower_models/tree_dark_1_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout_node_1 = {
#include "chao/flower_models/tree_dark_1_sprout_node_1.inc"
};
static Sint16 tree_dark_1_sprout_polygons[139] = {
#include "chao/flower_models/tree_dark_1_sprout_polygons.inc"
};
static Sint32 tree_dark_1_sprout_vertices[71] = {
#include "chao/flower_models/tree_dark_1_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_sprout_model = {
#include "chao/flower_models/tree_dark_1_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_sprout = {
#include "chao/flower_models/tree_dark_1_sprout.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature_node_7 = {
#include "chao/flower_models/tree_dark_1_mature_node_7.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature_node_6 = {
#include "chao/flower_models/tree_dark_1_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature_node_5 = {
#include "chao/flower_models/tree_dark_1_mature_node_5.inc"
};
static Sint16 tree_dark_1_mature_node_4_polygons[53] = {
#include "chao/flower_models/tree_dark_1_mature_node_4_polygons.inc"
};
static Sint32 tree_dark_1_mature_node_4_vertices[39] = {
#include "chao/flower_models/tree_dark_1_mature_node_4_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_mature_node_4_model = {
#include "chao/flower_models/tree_dark_1_mature_node_4_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature_node_4 = {
#include "chao/flower_models/tree_dark_1_mature_node_4.inc"
};
static Sint16 tree_dark_1_mature_node_3_polygons[53] = {
#include "chao/flower_models/tree_dark_1_mature_node_3_polygons.inc"
};
static Sint32 tree_dark_1_mature_node_3_vertices[39] = {
#include "chao/flower_models/tree_dark_1_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_mature_node_3_model = {
#include "chao/flower_models/tree_dark_1_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature_node_3 = {
#include "chao/flower_models/tree_dark_1_mature_node_3.inc"
};
static Sint16 tree_dark_1_mature_node_2_polygons[53] = {
#include "chao/flower_models/tree_dark_1_mature_node_2_polygons.inc"
};
static Sint32 tree_dark_1_mature_node_2_vertices[39] = {
#include "chao/flower_models/tree_dark_1_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_mature_node_2_model = {
#include "chao/flower_models/tree_dark_1_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature_node_2 = {
#include "chao/flower_models/tree_dark_1_mature_node_2.inc"
};
static Sint16 tree_dark_1_mature_node_1_polygons[53] = {
#include "chao/flower_models/tree_dark_1_mature_node_1_polygons.inc"
};
static Sint32 tree_dark_1_mature_node_1_vertices[39] = {
#include "chao/flower_models/tree_dark_1_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_mature_node_1_model = {
#include "chao/flower_models/tree_dark_1_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature_node_1 = {
#include "chao/flower_models/tree_dark_1_mature_node_1.inc"
};
static Sint16 tree_dark_1_mature_polygons[139] = {
#include "chao/flower_models/tree_dark_1_mature_polygons.inc"
};
static Sint32 tree_dark_1_mature_vertices[71] = {
#include "chao/flower_models/tree_dark_1_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_mature_model = {
#include "chao/flower_models/tree_dark_1_mature_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_mature = {
#include "chao/flower_models/tree_dark_1_mature.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered_node_7 = {
#include "chao/flower_models/tree_dark_1_withered_node_7.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered_node_6 = {
#include "chao/flower_models/tree_dark_1_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered_node_5 = {
#include "chao/flower_models/tree_dark_1_withered_node_5.inc"
};
static Sint16 tree_dark_1_withered_node_4_polygons[53] = {
#include "chao/flower_models/tree_dark_1_withered_node_4_polygons.inc"
};
static Sint32 tree_dark_1_withered_node_4_vertices[39] = {
#include "chao/flower_models/tree_dark_1_withered_node_4_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_withered_node_4_model = {
#include "chao/flower_models/tree_dark_1_withered_node_4_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered_node_4 = {
#include "chao/flower_models/tree_dark_1_withered_node_4.inc"
};
static Sint16 tree_dark_1_withered_node_3_polygons[53] = {
#include "chao/flower_models/tree_dark_1_withered_node_3_polygons.inc"
};
static Sint32 tree_dark_1_withered_node_3_vertices[39] = {
#include "chao/flower_models/tree_dark_1_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_withered_node_3_model = {
#include "chao/flower_models/tree_dark_1_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered_node_3 = {
#include "chao/flower_models/tree_dark_1_withered_node_3.inc"
};
static Sint16 tree_dark_1_withered_node_2_polygons[53] = {
#include "chao/flower_models/tree_dark_1_withered_node_2_polygons.inc"
};
static Sint32 tree_dark_1_withered_node_2_vertices[39] = {
#include "chao/flower_models/tree_dark_1_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_withered_node_2_model = {
#include "chao/flower_models/tree_dark_1_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered_node_2 = {
#include "chao/flower_models/tree_dark_1_withered_node_2.inc"
};
static Sint16 tree_dark_1_withered_node_1_polygons[53] = {
#include "chao/flower_models/tree_dark_1_withered_node_1_polygons.inc"
};
static Sint32 tree_dark_1_withered_node_1_vertices[39] = {
#include "chao/flower_models/tree_dark_1_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_withered_node_1_model = {
#include "chao/flower_models/tree_dark_1_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered_node_1 = {
#include "chao/flower_models/tree_dark_1_withered_node_1.inc"
};
static Sint16 tree_dark_1_withered_polygons[145] = {
#include "chao/flower_models/tree_dark_1_withered_polygons.inc"
};
static Sint32 tree_dark_1_withered_vertices[71] = {
#include "chao/flower_models/tree_dark_1_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_1_withered_model = {
#include "chao/flower_models/tree_dark_1_withered_model.inc"
};
static NJS_CNK_OBJECT tree_dark_1_withered = {
#include "chao/flower_models/tree_dark_1_withered.inc"
};
static NJS_CNK_OBJECT tree_dark_2_sprout_node_6 = {
#include "chao/flower_models/tree_dark_2_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_2_sprout_node_5 = {
#include "chao/flower_models/tree_dark_2_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_dark_2_sprout_node_4 = {
#include "chao/flower_models/tree_dark_2_sprout_node_4.inc"
};
static Sint16 tree_dark_2_sprout_node_3_polygons[69] = {
#include "chao/flower_models/tree_dark_2_sprout_node_3_polygons.inc"
};
static Sint32 tree_dark_2_sprout_node_3_vertices[67] = {
#include "chao/flower_models/tree_dark_2_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_sprout_node_3_model = {
#include "chao/flower_models/tree_dark_2_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_sprout_node_3 = {
#include "chao/flower_models/tree_dark_2_sprout_node_3.inc"
};
static Sint16 tree_dark_2_sprout_node_2_polygons[69] = {
#include "chao/flower_models/tree_dark_2_sprout_node_2_polygons.inc"
};
static Sint32 tree_dark_2_sprout_node_2_vertices[67] = {
#include "chao/flower_models/tree_dark_2_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_sprout_node_2_model = {
#include "chao/flower_models/tree_dark_2_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_sprout_node_2 = {
#include "chao/flower_models/tree_dark_2_sprout_node_2.inc"
};
static Sint16 tree_dark_2_sprout_node_1_polygons[69] = {
#include "chao/flower_models/tree_dark_2_sprout_node_1_polygons.inc"
};
static Sint32 tree_dark_2_sprout_node_1_vertices[67] = {
#include "chao/flower_models/tree_dark_2_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_sprout_node_1_model = {
#include "chao/flower_models/tree_dark_2_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_sprout_node_1 = {
#include "chao/flower_models/tree_dark_2_sprout_node_1.inc"
};
static Sint16 tree_dark_2_sprout_polygons[107] = {
#include "chao/flower_models/tree_dark_2_sprout_polygons.inc"
};
static Sint32 tree_dark_2_sprout_vertices[55] = {
#include "chao/flower_models/tree_dark_2_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_sprout_model = {
#include "chao/flower_models/tree_dark_2_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_sprout = {
#include "chao/flower_models/tree_dark_2_sprout.inc"
};
static NJS_CNK_OBJECT tree_dark_2_mature_node_6 = {
#include "chao/flower_models/tree_dark_2_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_2_mature_node_5 = {
#include "chao/flower_models/tree_dark_2_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_dark_2_mature_node_4 = {
#include "chao/flower_models/tree_dark_2_mature_node_4.inc"
};
static Sint16 tree_dark_2_mature_node_3_polygons[69] = {
#include "chao/flower_models/tree_dark_2_mature_node_3_polygons.inc"
};
static Sint32 tree_dark_2_mature_node_3_vertices[67] = {
#include "chao/flower_models/tree_dark_2_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_mature_node_3_model = {
#include "chao/flower_models/tree_dark_2_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_mature_node_3 = {
#include "chao/flower_models/tree_dark_2_mature_node_3.inc"
};
static Sint16 tree_dark_2_mature_node_2_polygons[69] = {
#include "chao/flower_models/tree_dark_2_mature_node_2_polygons.inc"
};
static Sint32 tree_dark_2_mature_node_2_vertices[67] = {
#include "chao/flower_models/tree_dark_2_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_mature_node_2_model = {
#include "chao/flower_models/tree_dark_2_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_mature_node_2 = {
#include "chao/flower_models/tree_dark_2_mature_node_2.inc"
};
static Sint16 tree_dark_2_mature_node_1_polygons[69] = {
#include "chao/flower_models/tree_dark_2_mature_node_1_polygons.inc"
};
static Sint32 tree_dark_2_mature_node_1_vertices[67] = {
#include "chao/flower_models/tree_dark_2_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_mature_node_1_model = {
#include "chao/flower_models/tree_dark_2_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_mature_node_1 = {
#include "chao/flower_models/tree_dark_2_mature_node_1.inc"
};
static Sint16 tree_dark_2_mature_polygons[115] = {
#include "chao/flower_models/tree_dark_2_mature_polygons.inc"
};
static Sint32 tree_dark_2_mature_vertices[55] = {
#include "chao/flower_models/tree_dark_2_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_mature_model = {
#include "chao/flower_models/tree_dark_2_mature_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_mature = {
#include "chao/flower_models/tree_dark_2_mature.inc"
};
static NJS_CNK_OBJECT tree_dark_2_withered_node_6 = {
#include "chao/flower_models/tree_dark_2_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_2_withered_node_5 = {
#include "chao/flower_models/tree_dark_2_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_dark_2_withered_node_4 = {
#include "chao/flower_models/tree_dark_2_withered_node_4.inc"
};
static Sint16 tree_dark_2_withered_node_3_polygons[69] = {
#include "chao/flower_models/tree_dark_2_withered_node_3_polygons.inc"
};
static Sint32 tree_dark_2_withered_node_3_vertices[67] = {
#include "chao/flower_models/tree_dark_2_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_withered_node_3_model = {
#include "chao/flower_models/tree_dark_2_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_withered_node_3 = {
#include "chao/flower_models/tree_dark_2_withered_node_3.inc"
};
static Sint16 tree_dark_2_withered_node_2_polygons[69] = {
#include "chao/flower_models/tree_dark_2_withered_node_2_polygons.inc"
};
static Sint32 tree_dark_2_withered_node_2_vertices[67] = {
#include "chao/flower_models/tree_dark_2_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_withered_node_2_model = {
#include "chao/flower_models/tree_dark_2_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_withered_node_2 = {
#include "chao/flower_models/tree_dark_2_withered_node_2.inc"
};
static Sint16 tree_dark_2_withered_node_1_polygons[69] = {
#include "chao/flower_models/tree_dark_2_withered_node_1_polygons.inc"
};
static Sint32 tree_dark_2_withered_node_1_vertices[67] = {
#include "chao/flower_models/tree_dark_2_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_withered_node_1_model = {
#include "chao/flower_models/tree_dark_2_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_withered_node_1 = {
#include "chao/flower_models/tree_dark_2_withered_node_1.inc"
};
static Sint16 tree_dark_2_withered_polygons[115] = {
#include "chao/flower_models/tree_dark_2_withered_polygons.inc"
};
static Sint32 tree_dark_2_withered_vertices[55] = {
#include "chao/flower_models/tree_dark_2_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_2_withered_model = {
#include "chao/flower_models/tree_dark_2_withered_model.inc"
};
static NJS_CNK_OBJECT tree_dark_2_withered = {
#include "chao/flower_models/tree_dark_2_withered.inc"
};
static NJS_CNK_OBJECT tree_dark_3_sprout_node_6 = {
#include "chao/flower_models/tree_dark_3_sprout_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_3_sprout_node_5 = {
#include "chao/flower_models/tree_dark_3_sprout_node_5.inc"
};
static NJS_CNK_OBJECT tree_dark_3_sprout_node_4 = {
#include "chao/flower_models/tree_dark_3_sprout_node_4.inc"
};
static Sint16 tree_dark_3_sprout_node_3_polygons[83] = {
#include "chao/flower_models/tree_dark_3_sprout_node_3_polygons.inc"
};
static Sint32 tree_dark_3_sprout_node_3_vertices[55] = {
#include "chao/flower_models/tree_dark_3_sprout_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_sprout_node_3_model = {
#include "chao/flower_models/tree_dark_3_sprout_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_sprout_node_3 = {
#include "chao/flower_models/tree_dark_3_sprout_node_3.inc"
};
static Sint16 tree_dark_3_sprout_node_2_polygons[83] = {
#include "chao/flower_models/tree_dark_3_sprout_node_2_polygons.inc"
};
static Sint32 tree_dark_3_sprout_node_2_vertices[55] = {
#include "chao/flower_models/tree_dark_3_sprout_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_sprout_node_2_model = {
#include "chao/flower_models/tree_dark_3_sprout_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_sprout_node_2 = {
#include "chao/flower_models/tree_dark_3_sprout_node_2.inc"
};
static Sint16 tree_dark_3_sprout_node_1_polygons[83] = {
#include "chao/flower_models/tree_dark_3_sprout_node_1_polygons.inc"
};
static Sint32 tree_dark_3_sprout_node_1_vertices[55] = {
#include "chao/flower_models/tree_dark_3_sprout_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_sprout_node_1_model = {
#include "chao/flower_models/tree_dark_3_sprout_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_sprout_node_1 = {
#include "chao/flower_models/tree_dark_3_sprout_node_1.inc"
};
static Sint16 tree_dark_3_sprout_polygons[131] = {
#include "chao/flower_models/tree_dark_3_sprout_polygons.inc"
};
static Sint32 tree_dark_3_sprout_vertices[71] = {
#include "chao/flower_models/tree_dark_3_sprout_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_sprout_model = {
#include "chao/flower_models/tree_dark_3_sprout_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_sprout = {
#include "chao/flower_models/tree_dark_3_sprout.inc"
};
static NJS_CNK_OBJECT tree_dark_3_mature_node_6 = {
#include "chao/flower_models/tree_dark_3_mature_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_3_mature_node_5 = {
#include "chao/flower_models/tree_dark_3_mature_node_5.inc"
};
static NJS_CNK_OBJECT tree_dark_3_mature_node_4 = {
#include "chao/flower_models/tree_dark_3_mature_node_4.inc"
};
static Sint16 tree_dark_3_mature_node_3_polygons[83] = {
#include "chao/flower_models/tree_dark_3_mature_node_3_polygons.inc"
};
static Sint32 tree_dark_3_mature_node_3_vertices[55] = {
#include "chao/flower_models/tree_dark_3_mature_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_mature_node_3_model = {
#include "chao/flower_models/tree_dark_3_mature_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_mature_node_3 = {
#include "chao/flower_models/tree_dark_3_mature_node_3.inc"
};
static Sint16 tree_dark_3_mature_node_2_polygons[83] = {
#include "chao/flower_models/tree_dark_3_mature_node_2_polygons.inc"
};
static Sint32 tree_dark_3_mature_node_2_vertices[55] = {
#include "chao/flower_models/tree_dark_3_mature_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_mature_node_2_model = {
#include "chao/flower_models/tree_dark_3_mature_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_mature_node_2 = {
#include "chao/flower_models/tree_dark_3_mature_node_2.inc"
};
static Sint16 tree_dark_3_mature_node_1_polygons[83] = {
#include "chao/flower_models/tree_dark_3_mature_node_1_polygons.inc"
};
static Sint32 tree_dark_3_mature_node_1_vertices[55] = {
#include "chao/flower_models/tree_dark_3_mature_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_mature_node_1_model = {
#include "chao/flower_models/tree_dark_3_mature_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_mature_node_1 = {
#include "chao/flower_models/tree_dark_3_mature_node_1.inc"
};
static Sint16 tree_dark_3_mature_polygons[145] = {
#include "chao/flower_models/tree_dark_3_mature_polygons.inc"
};
static Sint32 tree_dark_3_mature_vertices[71] = {
#include "chao/flower_models/tree_dark_3_mature_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_mature_model = {
#include "chao/flower_models/tree_dark_3_mature_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_mature = {
#include "chao/flower_models/tree_dark_3_mature.inc"
};
static NJS_CNK_OBJECT tree_dark_3_withered_node_6 = {
#include "chao/flower_models/tree_dark_3_withered_node_6.inc"
};
static NJS_CNK_OBJECT tree_dark_3_withered_node_5 = {
#include "chao/flower_models/tree_dark_3_withered_node_5.inc"
};
static NJS_CNK_OBJECT tree_dark_3_withered_node_4 = {
#include "chao/flower_models/tree_dark_3_withered_node_4.inc"
};
static Sint16 tree_dark_3_withered_node_3_polygons[83] = {
#include "chao/flower_models/tree_dark_3_withered_node_3_polygons.inc"
};
static Sint32 tree_dark_3_withered_node_3_vertices[55] = {
#include "chao/flower_models/tree_dark_3_withered_node_3_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_withered_node_3_model = {
#include "chao/flower_models/tree_dark_3_withered_node_3_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_withered_node_3 = {
#include "chao/flower_models/tree_dark_3_withered_node_3.inc"
};
static Sint16 tree_dark_3_withered_node_2_polygons[83] = {
#include "chao/flower_models/tree_dark_3_withered_node_2_polygons.inc"
};
static Sint32 tree_dark_3_withered_node_2_vertices[55] = {
#include "chao/flower_models/tree_dark_3_withered_node_2_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_withered_node_2_model = {
#include "chao/flower_models/tree_dark_3_withered_node_2_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_withered_node_2 = {
#include "chao/flower_models/tree_dark_3_withered_node_2.inc"
};
static Sint16 tree_dark_3_withered_node_1_polygons[83] = {
#include "chao/flower_models/tree_dark_3_withered_node_1_polygons.inc"
};
static Sint32 tree_dark_3_withered_node_1_vertices[55] = {
#include "chao/flower_models/tree_dark_3_withered_node_1_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_withered_node_1_model = {
#include "chao/flower_models/tree_dark_3_withered_node_1_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_withered_node_1 = {
#include "chao/flower_models/tree_dark_3_withered_node_1.inc"
};
static Sint16 tree_dark_3_withered_polygons[131] = {
#include "chao/flower_models/tree_dark_3_withered_polygons.inc"
};
static Sint32 tree_dark_3_withered_vertices[71] = {
#include "chao/flower_models/tree_dark_3_withered_vertices.inc"
};
static NJS_CNK_MODEL tree_dark_3_withered_model = {
#include "chao/flower_models/tree_dark_3_withered_model.inc"
};
static NJS_CNK_OBJECT tree_dark_3_withered = {
#include "chao/flower_models/tree_dark_3_withered.inc"
};
static NJS_CNK_OBJECT garden_tree_neutral_node_7 = {
#include "chao/flower_models/garden_tree_neutral_node_7.inc"
};
static NJS_CNK_OBJECT garden_tree_neutral_node_6 = {
#include "chao/flower_models/garden_tree_neutral_node_6.inc"
};
static NJS_CNK_OBJECT garden_tree_neutral_node_5 = {
#include "chao/flower_models/garden_tree_neutral_node_5.inc"
};
static Sint16 garden_tree_neutral_node_4_polygons[57] = {
#include "chao/flower_models/garden_tree_neutral_node_4_polygons.inc"
};
static Sint32 garden_tree_neutral_node_4_vertices[63] = {
#include "chao/flower_models/garden_tree_neutral_node_4_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_neutral_node_4_model = {
#include "chao/flower_models/garden_tree_neutral_node_4_model.inc"
};
static NJS_CNK_OBJECT garden_tree_neutral_node_4 = {
#include "chao/flower_models/garden_tree_neutral_node_4.inc"
};
static Sint16 garden_tree_neutral_node_3_polygons[57] = {
#include "chao/flower_models/garden_tree_neutral_node_3_polygons.inc"
};
static Sint32 garden_tree_neutral_node_3_vertices[63] = {
#include "chao/flower_models/garden_tree_neutral_node_3_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_neutral_node_3_model = {
#include "chao/flower_models/garden_tree_neutral_node_3_model.inc"
};
static NJS_CNK_OBJECT garden_tree_neutral_node_3 = {
#include "chao/flower_models/garden_tree_neutral_node_3.inc"
};
static Sint16 garden_tree_neutral_node_2_polygons[57] = {
#include "chao/flower_models/garden_tree_neutral_node_2_polygons.inc"
};
static Sint32 garden_tree_neutral_node_2_vertices[63] = {
#include "chao/flower_models/garden_tree_neutral_node_2_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_neutral_node_2_model = {
#include "chao/flower_models/garden_tree_neutral_node_2_model.inc"
};
static NJS_CNK_OBJECT garden_tree_neutral_node_2 = {
#include "chao/flower_models/garden_tree_neutral_node_2.inc"
};
static Sint16 garden_tree_neutral_node_1_polygons[57] = {
#include "chao/flower_models/garden_tree_neutral_node_1_polygons.inc"
};
static Sint32 garden_tree_neutral_node_1_vertices[63] = {
#include "chao/flower_models/garden_tree_neutral_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_neutral_node_1_model = {
#include "chao/flower_models/garden_tree_neutral_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_tree_neutral_node_1 = {
#include "chao/flower_models/garden_tree_neutral_node_1.inc"
};
static Sint16 garden_tree_neutral_polygons[157] = {
#include "chao/flower_models/garden_tree_neutral_polygons.inc"
};
static Sint32 garden_tree_neutral_vertices[105] = {
#include "chao/flower_models/garden_tree_neutral_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_neutral_model = {
#include "chao/flower_models/garden_tree_neutral_model.inc"
};
NJS_CNK_OBJECT garden_tree_neutral = {
#include "chao/flower_models/garden_tree_neutral.inc"
};
static Sint16 garden_tree_hero_node_5_polygons[45] = {
#include "chao/flower_models/garden_tree_hero_node_5_polygons.inc"
};
static Sint32 garden_tree_hero_node_5_vertices[27] = {
#include "chao/flower_models/garden_tree_hero_node_5_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_hero_node_5_model = {
#include "chao/flower_models/garden_tree_hero_node_5_model.inc"
};
static NJS_CNK_OBJECT garden_tree_hero_node_5 = {
#include "chao/flower_models/garden_tree_hero_node_5.inc"
};
static Sint16 garden_tree_hero_node_4_polygons[45] = {
#include "chao/flower_models/garden_tree_hero_node_4_polygons.inc"
};
static Sint32 garden_tree_hero_node_4_vertices[27] = {
#include "chao/flower_models/garden_tree_hero_node_4_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_hero_node_4_model = {
#include "chao/flower_models/garden_tree_hero_node_4_model.inc"
};
static NJS_CNK_OBJECT garden_tree_hero_node_4 = {
#include "chao/flower_models/garden_tree_hero_node_4.inc"
};
static Sint16 garden_tree_hero_node_3_polygons[45] = {
#include "chao/flower_models/garden_tree_hero_node_3_polygons.inc"
};
static Sint32 garden_tree_hero_node_3_vertices[27] = {
#include "chao/flower_models/garden_tree_hero_node_3_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_hero_node_3_model = {
#include "chao/flower_models/garden_tree_hero_node_3_model.inc"
};
static NJS_CNK_OBJECT garden_tree_hero_node_3 = {
#include "chao/flower_models/garden_tree_hero_node_3.inc"
};
static Sint16 garden_tree_hero_node_2_polygons[45] = {
#include "chao/flower_models/garden_tree_hero_node_2_polygons.inc"
};
static Sint32 garden_tree_hero_node_2_vertices[27] = {
#include "chao/flower_models/garden_tree_hero_node_2_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_hero_node_2_model = {
#include "chao/flower_models/garden_tree_hero_node_2_model.inc"
};
static NJS_CNK_OBJECT garden_tree_hero_node_2 = {
#include "chao/flower_models/garden_tree_hero_node_2.inc"
};
static Sint16 garden_tree_hero_node_1_polygons[45] = {
#include "chao/flower_models/garden_tree_hero_node_1_polygons.inc"
};
static Sint32 garden_tree_hero_node_1_vertices[27] = {
#include "chao/flower_models/garden_tree_hero_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_hero_node_1_model = {
#include "chao/flower_models/garden_tree_hero_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_tree_hero_node_1 = {
#include "chao/flower_models/garden_tree_hero_node_1.inc"
};
static Sint16 garden_tree_hero_polygons[143] = {
#include "chao/flower_models/garden_tree_hero_polygons.inc"
};
static Sint32 garden_tree_hero_vertices[71] = {
#include "chao/flower_models/garden_tree_hero_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_hero_model = {
#include "chao/flower_models/garden_tree_hero_model.inc"
};
NJS_CNK_OBJECT garden_tree_hero = {
#include "chao/flower_models/garden_tree_hero.inc"
};
static NJS_CNK_OBJECT garden_tree_dark_node_3 = {
#include "chao/flower_models/garden_tree_dark_node_3.inc"
};
static NJS_CNK_OBJECT garden_tree_dark_node_2 = {
#include "chao/flower_models/garden_tree_dark_node_2.inc"
};
static NJS_CNK_OBJECT garden_tree_dark_node_1 = {
#include "chao/flower_models/garden_tree_dark_node_1.inc"
};
static Sint16 garden_tree_dark_polygons[333] = {
#include "chao/flower_models/garden_tree_dark_polygons.inc"
};
static Sint32 garden_tree_dark_vertices[195] = {
#include "chao/flower_models/garden_tree_dark_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_dark_model = {
#include "chao/flower_models/garden_tree_dark_model.inc"
};
NJS_CNK_OBJECT garden_tree_dark = {
#include "chao/flower_models/garden_tree_dark.inc"
};
static Sint16 garden_tree_dark_crown_polygons[25] = {
#include "chao/flower_models/garden_tree_dark_crown_polygons.inc"
};
static Sint32 garden_tree_dark_crown_vertices[19] = {
#include "chao/flower_models/garden_tree_dark_crown_vertices.inc"
};
static NJS_CNK_MODEL garden_tree_dark_crown_model = {
#include "chao/flower_models/garden_tree_dark_crown_model.inc"
};
NJS_CNK_OBJECT garden_tree_dark_crown = {
#include "chao/flower_models/garden_tree_dark_crown.inc"
};
NJS_CNK_OBJECT *garden_tree_models[GARDEN_SEED_TYPES][3] = {
#include "chao/flower_models/garden_tree_models.inc"
};
