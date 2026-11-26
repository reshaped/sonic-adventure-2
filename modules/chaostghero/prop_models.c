/* the Hero Garden's props, which set_chao_hero.bin places: pillars, flowers, a
 * tower and three tufts of grass. a tuft is a root and seven blades */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostghero/land.h"

static Sint16 hero_pillar_polygons[183] = {
#include "chaostghero/prop_models/hero_pillar_polygons.inc"
};

static Sint32 hero_pillar_vertices[99] = {
#include "chaostghero/prop_models/hero_pillar_vertices.inc"
};

static NJS_CNK_MODEL hero_pillar_model = {
#include "chaostghero/prop_models/hero_pillar_model.inc"
};

NJS_CNK_OBJECT hero_pillar = {
#include "chaostghero/prop_models/hero_pillar.inc"
};

static Sint16 hero_red_flowers_polygons[251] = {
#include "chaostghero/prop_models/hero_red_flowers_polygons.inc"
};

static Sint32 hero_red_flowers_vertices[291] = {
#include "chaostghero/prop_models/hero_red_flowers_vertices.inc"
};

static NJS_CNK_MODEL hero_red_flowers_model = {
#include "chaostghero/prop_models/hero_red_flowers_model.inc"
};

NJS_CNK_OBJECT hero_red_flowers = {
#include "chaostghero/prop_models/hero_red_flowers.inc"
};

static Sint16 hero_purple_flowers_polygons[213] = {
#include "chaostghero/prop_models/hero_purple_flowers_polygons.inc"
};

static Sint32 hero_purple_flowers_vertices[243] = {
#include "chaostghero/prop_models/hero_purple_flowers_vertices.inc"
};

static NJS_CNK_MODEL hero_purple_flowers_model = {
#include "chaostghero/prop_models/hero_purple_flowers_model.inc"
};

NJS_CNK_OBJECT hero_purple_flowers = {
#include "chaostghero/prop_models/hero_purple_flowers.inc"
};

static Sint16 hero_broken_pillar_polygons[151] = {
#include "chaostghero/prop_models/hero_broken_pillar_polygons.inc"
};

static Sint32 hero_broken_pillar_vertices[75] = {
#include "chaostghero/prop_models/hero_broken_pillar_vertices.inc"
};

static NJS_CNK_MODEL hero_broken_pillar_model = {
#include "chaostghero/prop_models/hero_broken_pillar_model.inc"
};

NJS_CNK_OBJECT hero_broken_pillar = {
#include "chaostghero/prop_models/hero_broken_pillar.inc"
};

static Sint16 hero_grass_a_blade_7_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_a_blade_7_polygons.inc"
};

static Sint32 hero_grass_a_blade_7_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_a_blade_7_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_blade_7_model = {
#include "chaostghero/prop_models/hero_grass_a_blade_7_model.inc"
};

static NJS_CNK_OBJECT hero_grass_a_blade_7 = {
#include "chaostghero/prop_models/hero_grass_a_blade_7.inc"
};

static Sint16 hero_grass_a_blade_6_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_a_blade_6_polygons.inc"
};

static Sint32 hero_grass_a_blade_6_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_a_blade_6_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_blade_6_model = {
#include "chaostghero/prop_models/hero_grass_a_blade_6_model.inc"
};

static NJS_CNK_OBJECT hero_grass_a_blade_6 = {
#include "chaostghero/prop_models/hero_grass_a_blade_6.inc"
};

static Sint16 hero_grass_a_blade_5_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_a_blade_5_polygons.inc"
};

static Sint32 hero_grass_a_blade_5_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_a_blade_5_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_blade_5_model = {
#include "chaostghero/prop_models/hero_grass_a_blade_5_model.inc"
};

static NJS_CNK_OBJECT hero_grass_a_blade_5 = {
#include "chaostghero/prop_models/hero_grass_a_blade_5.inc"
};

static Sint16 hero_grass_a_blade_4_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_a_blade_4_polygons.inc"
};

static Sint32 hero_grass_a_blade_4_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_a_blade_4_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_blade_4_model = {
#include "chaostghero/prop_models/hero_grass_a_blade_4_model.inc"
};

static NJS_CNK_OBJECT hero_grass_a_blade_4 = {
#include "chaostghero/prop_models/hero_grass_a_blade_4.inc"
};

static Sint16 hero_grass_a_blade_3_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_a_blade_3_polygons.inc"
};

static Sint32 hero_grass_a_blade_3_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_a_blade_3_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_blade_3_model = {
#include "chaostghero/prop_models/hero_grass_a_blade_3_model.inc"
};

static NJS_CNK_OBJECT hero_grass_a_blade_3 = {
#include "chaostghero/prop_models/hero_grass_a_blade_3.inc"
};

static Sint16 hero_grass_a_blade_2_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_a_blade_2_polygons.inc"
};

static Sint32 hero_grass_a_blade_2_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_a_blade_2_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_blade_2_model = {
#include "chaostghero/prop_models/hero_grass_a_blade_2_model.inc"
};

static NJS_CNK_OBJECT hero_grass_a_blade_2 = {
#include "chaostghero/prop_models/hero_grass_a_blade_2.inc"
};

static Sint16 hero_grass_a_blade_1_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_a_blade_1_polygons.inc"
};

static Sint32 hero_grass_a_blade_1_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_a_blade_1_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_blade_1_model = {
#include "chaostghero/prop_models/hero_grass_a_blade_1_model.inc"
};

static NJS_CNK_OBJECT hero_grass_a_blade_1 = {
#include "chaostghero/prop_models/hero_grass_a_blade_1.inc"
};

static Sint16 hero_grass_a_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_a_polygons.inc"
};

static Sint32 hero_grass_a_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_a_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_a_model = {
#include "chaostghero/prop_models/hero_grass_a_model.inc"
};

NJS_CNK_OBJECT hero_grass_a = {
#include "chaostghero/prop_models/hero_grass_a.inc"
};

static Sint16 hero_grass_b_blade_7_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_b_blade_7_polygons.inc"
};

static Sint32 hero_grass_b_blade_7_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_b_blade_7_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_blade_7_model = {
#include "chaostghero/prop_models/hero_grass_b_blade_7_model.inc"
};

static NJS_CNK_OBJECT hero_grass_b_blade_7 = {
#include "chaostghero/prop_models/hero_grass_b_blade_7.inc"
};

static Sint16 hero_grass_b_blade_6_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_b_blade_6_polygons.inc"
};

static Sint32 hero_grass_b_blade_6_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_b_blade_6_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_blade_6_model = {
#include "chaostghero/prop_models/hero_grass_b_blade_6_model.inc"
};

static NJS_CNK_OBJECT hero_grass_b_blade_6 = {
#include "chaostghero/prop_models/hero_grass_b_blade_6.inc"
};

static Sint16 hero_grass_b_blade_5_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_b_blade_5_polygons.inc"
};

static Sint32 hero_grass_b_blade_5_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_b_blade_5_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_blade_5_model = {
#include "chaostghero/prop_models/hero_grass_b_blade_5_model.inc"
};

static NJS_CNK_OBJECT hero_grass_b_blade_5 = {
#include "chaostghero/prop_models/hero_grass_b_blade_5.inc"
};

static Sint16 hero_grass_b_blade_4_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_b_blade_4_polygons.inc"
};

static Sint32 hero_grass_b_blade_4_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_b_blade_4_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_blade_4_model = {
#include "chaostghero/prop_models/hero_grass_b_blade_4_model.inc"
};

static NJS_CNK_OBJECT hero_grass_b_blade_4 = {
#include "chaostghero/prop_models/hero_grass_b_blade_4.inc"
};

static Sint16 hero_grass_b_blade_3_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_b_blade_3_polygons.inc"
};

static Sint32 hero_grass_b_blade_3_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_b_blade_3_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_blade_3_model = {
#include "chaostghero/prop_models/hero_grass_b_blade_3_model.inc"
};

static NJS_CNK_OBJECT hero_grass_b_blade_3 = {
#include "chaostghero/prop_models/hero_grass_b_blade_3.inc"
};

static Sint16 hero_grass_b_blade_2_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_b_blade_2_polygons.inc"
};

static Sint32 hero_grass_b_blade_2_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_b_blade_2_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_blade_2_model = {
#include "chaostghero/prop_models/hero_grass_b_blade_2_model.inc"
};

static NJS_CNK_OBJECT hero_grass_b_blade_2 = {
#include "chaostghero/prop_models/hero_grass_b_blade_2.inc"
};

static Sint16 hero_grass_b_blade_1_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_b_blade_1_polygons.inc"
};

static Sint32 hero_grass_b_blade_1_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_b_blade_1_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_blade_1_model = {
#include "chaostghero/prop_models/hero_grass_b_blade_1_model.inc"
};

static NJS_CNK_OBJECT hero_grass_b_blade_1 = {
#include "chaostghero/prop_models/hero_grass_b_blade_1.inc"
};

static Sint16 hero_grass_b_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_b_polygons.inc"
};

static Sint32 hero_grass_b_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_b_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_b_model = {
#include "chaostghero/prop_models/hero_grass_b_model.inc"
};

NJS_CNK_OBJECT hero_grass_b = {
#include "chaostghero/prop_models/hero_grass_b.inc"
};

static Sint16 hero_grass_c_blade_7_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_c_blade_7_polygons.inc"
};

static Sint32 hero_grass_c_blade_7_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_c_blade_7_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_blade_7_model = {
#include "chaostghero/prop_models/hero_grass_c_blade_7_model.inc"
};

static NJS_CNK_OBJECT hero_grass_c_blade_7 = {
#include "chaostghero/prop_models/hero_grass_c_blade_7.inc"
};

static Sint16 hero_grass_c_blade_6_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_c_blade_6_polygons.inc"
};

static Sint32 hero_grass_c_blade_6_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_c_blade_6_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_blade_6_model = {
#include "chaostghero/prop_models/hero_grass_c_blade_6_model.inc"
};

static NJS_CNK_OBJECT hero_grass_c_blade_6 = {
#include "chaostghero/prop_models/hero_grass_c_blade_6.inc"
};

static Sint16 hero_grass_c_blade_5_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_c_blade_5_polygons.inc"
};

static Sint32 hero_grass_c_blade_5_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_c_blade_5_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_blade_5_model = {
#include "chaostghero/prop_models/hero_grass_c_blade_5_model.inc"
};

static NJS_CNK_OBJECT hero_grass_c_blade_5 = {
#include "chaostghero/prop_models/hero_grass_c_blade_5.inc"
};

static Sint16 hero_grass_c_blade_4_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_c_blade_4_polygons.inc"
};

static Sint32 hero_grass_c_blade_4_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_c_blade_4_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_blade_4_model = {
#include "chaostghero/prop_models/hero_grass_c_blade_4_model.inc"
};

static NJS_CNK_OBJECT hero_grass_c_blade_4 = {
#include "chaostghero/prop_models/hero_grass_c_blade_4.inc"
};

static Sint16 hero_grass_c_blade_3_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_c_blade_3_polygons.inc"
};

static Sint32 hero_grass_c_blade_3_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_c_blade_3_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_blade_3_model = {
#include "chaostghero/prop_models/hero_grass_c_blade_3_model.inc"
};

static NJS_CNK_OBJECT hero_grass_c_blade_3 = {
#include "chaostghero/prop_models/hero_grass_c_blade_3.inc"
};

static Sint16 hero_grass_c_blade_2_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_c_blade_2_polygons.inc"
};

static Sint32 hero_grass_c_blade_2_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_c_blade_2_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_blade_2_model = {
#include "chaostghero/prop_models/hero_grass_c_blade_2_model.inc"
};

static NJS_CNK_OBJECT hero_grass_c_blade_2 = {
#include "chaostghero/prop_models/hero_grass_c_blade_2.inc"
};

static Sint16 hero_grass_c_blade_1_polygons[25] = {
#include "chaostghero/prop_models/hero_grass_c_blade_1_polygons.inc"
};

static Sint32 hero_grass_c_blade_1_vertices[19] = {
#include "chaostghero/prop_models/hero_grass_c_blade_1_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_blade_1_model = {
#include "chaostghero/prop_models/hero_grass_c_blade_1_model.inc"
};

static NJS_CNK_OBJECT hero_grass_c_blade_1 = {
#include "chaostghero/prop_models/hero_grass_c_blade_1.inc"
};

static Sint16 hero_grass_c_polygons[39] = {
#include "chaostghero/prop_models/hero_grass_c_polygons.inc"
};

static Sint32 hero_grass_c_vertices[23] = {
#include "chaostghero/prop_models/hero_grass_c_vertices.inc"
};

static NJS_CNK_MODEL hero_grass_c_model = {
#include "chaostghero/prop_models/hero_grass_c_model.inc"
};

NJS_CNK_OBJECT hero_grass_c = {
#include "chaostghero/prop_models/hero_grass_c.inc"
};

static Sint16 hero_tower_polygons[253] = {
#include "chaostghero/prop_models/hero_tower_polygons.inc"
};

static Sint32 hero_tower_vertices[283] = {
#include "chaostghero/prop_models/hero_tower_vertices.inc"
};

static NJS_CNK_MODEL hero_tower_model = {
#include "chaostghero/prop_models/hero_tower_model.inc"
};

NJS_CNK_OBJECT hero_tower = {
#include "chaostghero/prop_models/hero_tower.inc"
};
