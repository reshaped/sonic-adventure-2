#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg31/scenery_land.h"

/* The background land references its archive and each texture in this pool. */
const char green_hill_water_land_filenames[113] = {
#include "stg31/water_land/green_hill_water_land_filenames.inc"
};

/* STG31.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between distant_land.c and terrain.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry green_hill_water_land_entries[8];
struct land_table green_hill_water_land;
static NJS_TEXNAME green_hill_water_land_textures[5];
NJS_TEXLIST green_hill_water_texlist;
static Sint16 green_hill_water_land_draw_7_polygons[159];
static Sint32 green_hill_water_land_draw_7_vertices[147];
static NJS_CNK_MODEL green_hill_water_land_draw_7_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_7;
static Sint16 green_hill_water_land_draw_6_polygons[89];
static Sint32 green_hill_water_land_draw_6_vertices[87];
static NJS_CNK_MODEL green_hill_water_land_draw_6_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_6;
static Sint16 green_hill_water_land_draw_5_polygons[163];
static Sint32 green_hill_water_land_draw_5_vertices[155];
static NJS_CNK_MODEL green_hill_water_land_draw_5_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_5;
static Sint16 green_hill_water_land_draw_4_polygons[109];
static Sint32 green_hill_water_land_draw_4_vertices[99];
static NJS_CNK_MODEL green_hill_water_land_draw_4_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_4;
static Sint16 green_hill_water_land_draw_3_polygons[97];
static Sint32 green_hill_water_land_draw_3_vertices[87];
static NJS_CNK_MODEL green_hill_water_land_draw_3_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_3;
static Sint16 green_hill_water_land_draw_2_polygons[85];
static Sint32 green_hill_water_land_draw_2_vertices[75];
static NJS_CNK_MODEL green_hill_water_land_draw_2_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_2;
static Sint16 green_hill_water_land_draw_1_polygons[133];
static Sint32 green_hill_water_land_draw_1_vertices[123];
static NJS_CNK_MODEL green_hill_water_land_draw_1_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_1;
static Sint16 green_hill_water_land_draw_0_polygons[433];
static Sint32 green_hill_water_land_draw_0_vertices[339];
static NJS_CNK_MODEL green_hill_water_land_draw_0_model;
static NJS_CNK_OBJECT green_hill_water_land_draw_0;

static struct land_entry green_hill_water_land_entries[8] = {
#include "stg31/water_land/green_hill_water_land_entries.inc"
};
struct land_table green_hill_water_land = {
#include "stg31/water_land/green_hill_water_land.inc"
};
static NJS_TEXNAME green_hill_water_land_textures[5] = {
#include "stg31/water_land/green_hill_water_land_textures.inc"
};
NJS_TEXLIST green_hill_water_texlist = {
#include "stg31/water_land/green_hill_water_texlist.inc"
};
static Sint16 green_hill_water_land_draw_7_polygons[159] = {
#include "stg31/water_land/green_hill_water_land_draw_7_polygons.inc"
};
static Sint32 green_hill_water_land_draw_7_vertices[147] = {
#include "stg31/water_land/green_hill_water_land_draw_7_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_7_model = {
#include "stg31/water_land/green_hill_water_land_draw_7_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_7 = {
#include "stg31/water_land/green_hill_water_land_draw_7.inc"
};
static Sint16 green_hill_water_land_draw_6_polygons[89] = {
#include "stg31/water_land/green_hill_water_land_draw_6_polygons.inc"
};
static Sint32 green_hill_water_land_draw_6_vertices[87] = {
#include "stg31/water_land/green_hill_water_land_draw_6_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_6_model = {
#include "stg31/water_land/green_hill_water_land_draw_6_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_6 = {
#include "stg31/water_land/green_hill_water_land_draw_6.inc"
};
static Sint16 green_hill_water_land_draw_5_polygons[163] = {
#include "stg31/water_land/green_hill_water_land_draw_5_polygons.inc"
};
static Sint32 green_hill_water_land_draw_5_vertices[155] = {
#include "stg31/water_land/green_hill_water_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_5_model = {
#include "stg31/water_land/green_hill_water_land_draw_5_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_5 = {
#include "stg31/water_land/green_hill_water_land_draw_5.inc"
};
static Sint16 green_hill_water_land_draw_4_polygons[109] = {
#include "stg31/water_land/green_hill_water_land_draw_4_polygons.inc"
};
static Sint32 green_hill_water_land_draw_4_vertices[99] = {
#include "stg31/water_land/green_hill_water_land_draw_4_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_4_model = {
#include "stg31/water_land/green_hill_water_land_draw_4_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_4 = {
#include "stg31/water_land/green_hill_water_land_draw_4.inc"
};
static Sint16 green_hill_water_land_draw_3_polygons[97] = {
#include "stg31/water_land/green_hill_water_land_draw_3_polygons.inc"
};
static Sint32 green_hill_water_land_draw_3_vertices[87] = {
#include "stg31/water_land/green_hill_water_land_draw_3_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_3_model = {
#include "stg31/water_land/green_hill_water_land_draw_3_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_3 = {
#include "stg31/water_land/green_hill_water_land_draw_3.inc"
};
static Sint16 green_hill_water_land_draw_2_polygons[85] = {
#include "stg31/water_land/green_hill_water_land_draw_2_polygons.inc"
};
static Sint32 green_hill_water_land_draw_2_vertices[75] = {
#include "stg31/water_land/green_hill_water_land_draw_2_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_2_model = {
#include "stg31/water_land/green_hill_water_land_draw_2_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_2 = {
#include "stg31/water_land/green_hill_water_land_draw_2.inc"
};
static Sint16 green_hill_water_land_draw_1_polygons[133] = {
#include "stg31/water_land/green_hill_water_land_draw_1_polygons.inc"
};
static Sint32 green_hill_water_land_draw_1_vertices[123] = {
#include "stg31/water_land/green_hill_water_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_1_model = {
#include "stg31/water_land/green_hill_water_land_draw_1_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_1 = {
#include "stg31/water_land/green_hill_water_land_draw_1.inc"
};
static Sint16 green_hill_water_land_draw_0_polygons[433] = {
#include "stg31/water_land/green_hill_water_land_draw_0_polygons.inc"
};
static Sint32 green_hill_water_land_draw_0_vertices[339] = {
#include "stg31/water_land/green_hill_water_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL green_hill_water_land_draw_0_model = {
#include "stg31/water_land/green_hill_water_land_draw_0_model.inc"
};
static NJS_CNK_OBJECT green_hill_water_land_draw_0 = {
#include "stg31/water_land/green_hill_water_land_draw_0.inc"
};
