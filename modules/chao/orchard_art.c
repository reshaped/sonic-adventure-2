/* CHAO.PRS's chunk models and chunk object pointers, from the disc. its D
 * lies between machine_art.c and garden_decor.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/orchard_art.h"

static Sint16 seed_neutral_1_polygons[217] = {
#include "chao/orchard_art/seed_neutral_1_polygons.inc"
};
static Sint32 seed_neutral_1_vertices[195] = {
#include "chao/orchard_art/seed_neutral_1_vertices.inc"
};
static NJS_CNK_MODEL seed_neutral_1_model = {
#include "chao/orchard_art/seed_neutral_1_model.inc"
};
static NJS_CNK_OBJECT seed_neutral_1 = {
#include "chao/orchard_art/seed_neutral_1.inc"
};
static Sint16 seed_neutral_2_polygons[301] = {
#include "chao/orchard_art/seed_neutral_2_polygons.inc"
};
static Sint32 seed_neutral_2_vertices[171] = {
#include "chao/orchard_art/seed_neutral_2_vertices.inc"
};
static NJS_CNK_MODEL seed_neutral_2_model = {
#include "chao/orchard_art/seed_neutral_2_model.inc"
};
static NJS_CNK_OBJECT seed_neutral_2 = {
#include "chao/orchard_art/seed_neutral_2.inc"
};
static Sint16 seed_neutral_3_polygons[169] = {
#include "chao/orchard_art/seed_neutral_3_polygons.inc"
};
static Sint32 seed_neutral_3_vertices[123] = {
#include "chao/orchard_art/seed_neutral_3_vertices.inc"
};
static NJS_CNK_MODEL seed_neutral_3_model = {
#include "chao/orchard_art/seed_neutral_3_model.inc"
};
static NJS_CNK_OBJECT seed_neutral_3 = {
#include "chao/orchard_art/seed_neutral_3.inc"
};
static Sint16 seed_hero_1_polygons[203] = {
#include "chao/orchard_art/seed_hero_1_polygons.inc"
};
static Sint32 seed_hero_1_vertices[171] = {
#include "chao/orchard_art/seed_hero_1_vertices.inc"
};
static NJS_CNK_MODEL seed_hero_1_model = {
#include "chao/orchard_art/seed_hero_1_model.inc"
};
static NJS_CNK_OBJECT seed_hero_1 = {
#include "chao/orchard_art/seed_hero_1.inc"
};
static Sint16 seed_hero_2_polygons[165] = {
#include "chao/orchard_art/seed_hero_2_polygons.inc"
};
static Sint32 seed_hero_2_vertices[111] = {
#include "chao/orchard_art/seed_hero_2_vertices.inc"
};
static NJS_CNK_MODEL seed_hero_2_model = {
#include "chao/orchard_art/seed_hero_2_model.inc"
};
static NJS_CNK_OBJECT seed_hero_2 = {
#include "chao/orchard_art/seed_hero_2.inc"
};
static Sint16 seed_hero_3_polygons[403] = {
#include "chao/orchard_art/seed_hero_3_polygons.inc"
};
static Sint32 seed_hero_3_vertices[303] = {
#include "chao/orchard_art/seed_hero_3_vertices.inc"
};
static NJS_CNK_MODEL seed_hero_3_model = {
#include "chao/orchard_art/seed_hero_3_model.inc"
};
static NJS_CNK_OBJECT seed_hero_3 = {
#include "chao/orchard_art/seed_hero_3.inc"
};
static Sint16 seed_dark_1_polygons[269] = {
#include "chao/orchard_art/seed_dark_1_polygons.inc"
};
static Sint32 seed_dark_1_vertices[219] = {
#include "chao/orchard_art/seed_dark_1_vertices.inc"
};
static NJS_CNK_MODEL seed_dark_1_model = {
#include "chao/orchard_art/seed_dark_1_model.inc"
};
static NJS_CNK_OBJECT seed_dark_1 = {
#include "chao/orchard_art/seed_dark_1.inc"
};
static Sint16 seed_dark_2_polygons[131] = {
#include "chao/orchard_art/seed_dark_2_polygons.inc"
};
static Sint32 seed_dark_2_vertices[99] = {
#include "chao/orchard_art/seed_dark_2_vertices.inc"
};
static NJS_CNK_MODEL seed_dark_2_model = {
#include "chao/orchard_art/seed_dark_2_model.inc"
};
static NJS_CNK_OBJECT seed_dark_2 = {
#include "chao/orchard_art/seed_dark_2.inc"
};
static Sint16 seed_dark_3_polygons[175] = {
#include "chao/orchard_art/seed_dark_3_polygons.inc"
};
static Sint32 seed_dark_3_vertices[165] = {
#include "chao/orchard_art/seed_dark_3_vertices.inc"
};
static NJS_CNK_MODEL seed_dark_3_model = {
#include "chao/orchard_art/seed_dark_3_model.inc"
};
static NJS_CNK_OBJECT seed_dark_3 = {
#include "chao/orchard_art/seed_dark_3.inc"
};
NJS_CNK_OBJECT *garden_seed_models[GARDEN_SEED_TYPES] = {
#include "chao/orchard_art/garden_seed_models.inc"
};
