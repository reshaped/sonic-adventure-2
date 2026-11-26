#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg31/scenery_land.h"

/* The background land references its archive and each texture in this pool. */
const char green_hill_distant_land_filenames[26] = {
#include "stg31/distant_land/green_hill_distant_land_filenames.inc"
};

/* STG31.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between horizon_scroll.c and water_land.c */
static struct land_entry green_hill_distant_land_entries[16] = {
#include "stg31/distant_land/green_hill_distant_land_entries.inc"
};
struct land_table green_hill_distant_land = {
#include "stg31/distant_land/green_hill_distant_land.inc"
};
static NJS_TEXNAME green_hill_distant_land_textures[1] = {
#include "stg31/distant_land/green_hill_distant_land_textures.inc"
};
NJS_TEXLIST green_hill_distant_texlist = {
#include "stg31/distant_land/green_hill_distant_texlist.inc"
};
static Sint16 green_hill_distant_land_draw_15_polygons[383] = {
#include "stg31/distant_land/green_hill_distant_land_draw_15_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_15_vertices[267] = {
#include "stg31/distant_land/green_hill_distant_land_draw_15_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_15_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_15_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_1 = {
#include "stg31/distant_land/green_hill_surface_1.inc"
};
static Sint16 green_hill_distant_land_draw_14_polygons[155] = {
#include "stg31/distant_land/green_hill_distant_land_draw_14_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_14_vertices[127] = {
#include "stg31/distant_land/green_hill_distant_land_draw_14_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_14_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_14_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_2 = {
#include "stg31/distant_land/green_hill_surface_2.inc"
};
static Sint16 green_hill_distant_land_draw_13_polygons[473] = {
#include "stg31/distant_land/green_hill_distant_land_draw_13_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_13_vertices[319] = {
#include "stg31/distant_land/green_hill_distant_land_draw_13_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_13_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_13_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_3 = {
#include "stg31/distant_land/green_hill_surface_3.inc"
};
static Sint16 green_hill_distant_land_draw_12_polygons[79] = {
#include "stg31/distant_land/green_hill_distant_land_draw_12_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_12_vertices[63] = {
#include "stg31/distant_land/green_hill_distant_land_draw_12_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_12_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_12_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_4 = {
#include "stg31/distant_land/green_hill_surface_4.inc"
};
static Sint16 green_hill_distant_land_draw_11_polygons[183] = {
#include "stg31/distant_land/green_hill_distant_land_draw_11_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_11_vertices[131] = {
#include "stg31/distant_land/green_hill_distant_land_draw_11_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_11_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_11_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_5 = {
#include "stg31/distant_land/green_hill_surface_5.inc"
};
static Sint16 green_hill_distant_land_draw_10_polygons[209] = {
#include "stg31/distant_land/green_hill_distant_land_draw_10_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_10_vertices[151] = {
#include "stg31/distant_land/green_hill_distant_land_draw_10_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_10_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_10_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_6 = {
#include "stg31/distant_land/green_hill_surface_6.inc"
};
static Sint16 green_hill_distant_land_draw_9_polygons[159] = {
#include "stg31/distant_land/green_hill_distant_land_draw_9_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_9_vertices[123] = {
#include "stg31/distant_land/green_hill_distant_land_draw_9_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_9_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_9_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_7 = {
#include "stg31/distant_land/green_hill_surface_7.inc"
};
static Sint16 green_hill_distant_land_draw_8_polygons[519] = {
#include "stg31/distant_land/green_hill_distant_land_draw_8_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_8_vertices[383] = {
#include "stg31/distant_land/green_hill_distant_land_draw_8_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_8_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_8_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_0 = {
#include "stg31/distant_land/green_hill_surface_0.inc"
};
static Sint16 green_hill_distant_land_draw_7_polygons[389] = {
#include "stg31/distant_land/green_hill_distant_land_draw_7_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_7_vertices[263] = {
#include "stg31/distant_land/green_hill_distant_land_draw_7_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_7_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_7_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_9 = {
#include "stg31/distant_land/green_hill_surface_9.inc"
};
static Sint16 green_hill_distant_land_draw_6_polygons[129] = {
#include "stg31/distant_land/green_hill_distant_land_draw_6_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_6_vertices[103] = {
#include "stg31/distant_land/green_hill_distant_land_draw_6_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_6_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_6_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_10 = {
#include "stg31/distant_land/green_hill_surface_10.inc"
};
static Sint16 green_hill_distant_land_draw_5_polygons[459] = {
#include "stg31/distant_land/green_hill_distant_land_draw_5_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_5_vertices[303] = {
#include "stg31/distant_land/green_hill_distant_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_5_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_5_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_11 = {
#include "stg31/distant_land/green_hill_surface_11.inc"
};
static Sint16 green_hill_distant_land_draw_4_polygons[79] = {
#include "stg31/distant_land/green_hill_distant_land_draw_4_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_4_vertices[63] = {
#include "stg31/distant_land/green_hill_distant_land_draw_4_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_4_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_4_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_12 = {
#include "stg31/distant_land/green_hill_surface_12.inc"
};
static Sint16 green_hill_distant_land_draw_3_polygons[183] = {
#include "stg31/distant_land/green_hill_distant_land_draw_3_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_3_vertices[131] = {
#include "stg31/distant_land/green_hill_distant_land_draw_3_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_3_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_3_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_13 = {
#include "stg31/distant_land/green_hill_surface_13.inc"
};
static Sint16 green_hill_distant_land_draw_2_polygons[209] = {
#include "stg31/distant_land/green_hill_distant_land_draw_2_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_2_vertices[151] = {
#include "stg31/distant_land/green_hill_distant_land_draw_2_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_2_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_2_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_14 = {
#include "stg31/distant_land/green_hill_surface_14.inc"
};
static Sint16 green_hill_distant_land_draw_1_polygons[159] = {
#include "stg31/distant_land/green_hill_distant_land_draw_1_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_1_vertices[123] = {
#include "stg31/distant_land/green_hill_distant_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_1_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_1_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_15 = {
#include "stg31/distant_land/green_hill_surface_15.inc"
};
static Sint16 green_hill_distant_land_draw_0_polygons[477] = {
#include "stg31/distant_land/green_hill_distant_land_draw_0_polygons.inc"
};
static Sint32 green_hill_distant_land_draw_0_vertices[367] = {
#include "stg31/distant_land/green_hill_distant_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL green_hill_distant_land_draw_0_model = {
#include "stg31/distant_land/green_hill_distant_land_draw_0_model.inc"
};
NJS_CNK_OBJECT green_hill_surface_8 = {
#include "stg31/distant_land/green_hill_surface_8.inc"
};
