/* the Hero Garden's land, its seven water surfaces and its props' models. the
 * land and the water draw with hero_land_texlist, and the props with
 * AL_HERO_OBJ_TEX's textures */
#ifndef CHAOSTGHERO_LAND_H
#define CHAOSTGHERO_LAND_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"

extern struct land_table hero_land;
/* the buffer the garden compiles hero_land into, at its address in the
 * image. the garden names it through CHAO_LAND_BUFFER, which GCC's build
 * replaces with end */
extern Uint8 hero_land_buffer[];
extern NJS_TEXLIST hero_land_texlist;
extern NJS_CNK_OBJECT hero_fountain_jet_a;
extern NJS_CNK_OBJECT hero_fountain_jet_b;
extern NJS_CNK_OBJECT hero_fountain_jet_c;
extern NJS_CNK_OBJECT hero_fountain_jet_d;
extern NJS_CNK_OBJECT hero_fountain_base;
extern NJS_CNK_OBJECT hero_upper_pool;
extern NJS_CNK_OBJECT hero_lower_pool;

extern NJS_CNK_OBJECT hero_pillar, hero_broken_pillar;
extern NJS_CNK_OBJECT hero_red_flowers, hero_purple_flowers;
extern NJS_CNK_OBJECT hero_grass_a, hero_grass_b, hero_grass_c, hero_tower;

#endif
