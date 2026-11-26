#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg44/sky.h"

/* the file names that mad_space_sky_land and its texture list point at. each
 * starts on a 4-byte boundary */
const char mad_space_sky_land_filenames[67] = {
#include "stg44/sky_surfaces/mad_space_sky_land_filenames.inc"
};

/* STG44.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between terrain.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry mad_space_sky_land_entries[54];
struct land_table mad_space_sky_land;
static NJS_TEXNAME mad_space_sky_land_textures[5];
NJS_TEXLIST mad_space_animated_surface_texlist;
static Sint16 mad_space_sky_land_draw_52_polygons[379];
static Sint32 mad_space_sky_land_draw_52_vertices[239];
static NJS_CNK_MODEL mad_space_sky_land_draw_52_model;
NJS_CNK_OBJECT mad_space_land_0;
static Sint16 mad_space_sky_land_draw_48_polygons[221];
static Sint32 mad_space_sky_land_draw_48_vertices[135];
static NJS_CNK_MODEL mad_space_sky_land_draw_48_model;
NJS_CNK_OBJECT mad_space_land_1;
static Sint16 mad_space_sky_land_draw_44_polygons[149];
static Sint32 mad_space_sky_land_draw_44_vertices[87];
static NJS_CNK_MODEL mad_space_sky_land_draw_44_model;
NJS_CNK_OBJECT mad_space_land_2;
static Sint16 mad_space_sky_land_draw_40_polygons[61];
static Sint32 mad_space_sky_land_draw_40_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_40_model;
NJS_CNK_OBJECT mad_space_land_3;
static Sint16 mad_space_sky_land_draw_36_polygons[61];
static Sint32 mad_space_sky_land_draw_36_vertices[43];
static NJS_CNK_MODEL mad_space_sky_land_draw_36_model;
NJS_CNK_OBJECT mad_space_land_4;
static Sint16 mad_space_sky_land_draw_35_polygons[133];
static Sint32 mad_space_sky_land_draw_35_vertices[67];
static NJS_CNK_MODEL mad_space_sky_land_draw_35_model;
NJS_CNK_OBJECT mad_space_land_5;
static Sint16 mad_space_sky_land_draw_34_polygons[103];
static Sint32 mad_space_sky_land_draw_34_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_34_model;
NJS_CNK_OBJECT mad_space_land_6;
static Sint16 mad_space_sky_land_draw_33_polygons[103];
static Sint32 mad_space_sky_land_draw_33_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_33_model;
NJS_CNK_OBJECT mad_space_land_9;
static Sint16 mad_space_sky_land_draw_32_polygons[103];
static Sint32 mad_space_sky_land_draw_32_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_32_model;
NJS_CNK_OBJECT mad_space_land_16;
static Sint16 mad_space_sky_land_draw_31_polygons[103];
static Sint32 mad_space_sky_land_draw_31_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_31_model;
NJS_CNK_OBJECT mad_space_land_15;
static Sint16 mad_space_sky_land_draw_30_polygons[103];
static Sint32 mad_space_sky_land_draw_30_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_30_model;
NJS_CNK_OBJECT mad_space_land_14;
static Sint16 mad_space_sky_land_draw_29_polygons[103];
static Sint32 mad_space_sky_land_draw_29_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_29_model;
NJS_CNK_OBJECT mad_space_land_13;
static Sint16 mad_space_sky_land_draw_28_polygons[103];
static Sint32 mad_space_sky_land_draw_28_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_28_model;
NJS_CNK_OBJECT mad_space_land_12;
static Sint16 mad_space_sky_land_draw_27_polygons[103];
static Sint32 mad_space_sky_land_draw_27_vertices[51];
static NJS_CNK_MODEL mad_space_sky_land_draw_27_model;
NJS_CNK_OBJECT mad_space_land_11;
static Sint16 mad_space_sky_land_draw_24_polygons[155];
static Sint32 mad_space_sky_land_draw_24_vertices[131];
static NJS_CNK_MODEL mad_space_sky_land_draw_24_model;
NJS_CNK_OBJECT mad_space_land_7;
static Sint16 mad_space_sky_land_draw_0_polygons[35];
static Sint32 mad_space_sky_land_draw_0_vertices[35];
static NJS_CNK_MODEL mad_space_sky_land_draw_0_model;
NJS_CNK_OBJECT mad_space_land_8;
static NJS_CNK_OBJECT mad_space_sky_land_draw_52;
static NJS_CNK_OBJECT mad_space_sky_land_draw_50;
static NJS_CNK_OBJECT mad_space_sky_land_draw_49;
static NJS_CNK_OBJECT mad_space_sky_land_draw_48;
static NJS_CNK_OBJECT mad_space_sky_land_draw_46;
static NJS_CNK_OBJECT mad_space_sky_land_draw_45;
static NJS_CNK_OBJECT mad_space_sky_land_draw_44;
static NJS_CNK_OBJECT mad_space_sky_land_draw_42;
static NJS_CNK_OBJECT mad_space_sky_land_draw_41;
static NJS_CNK_OBJECT mad_space_sky_land_draw_40;
static NJS_CNK_OBJECT mad_space_sky_land_draw_38;
static NJS_CNK_OBJECT mad_space_sky_land_draw_37;
static NJS_CNK_OBJECT mad_space_sky_land_draw_36;
static NJS_CNK_OBJECT mad_space_sky_land_draw_25;
static NJS_CNK_OBJECT mad_space_sky_land_draw_24;
static NJS_CNK_OBJECT mad_space_sky_land_draw_22;
static NJS_CNK_OBJECT mad_space_sky_land_draw_21;
static NJS_CNK_OBJECT mad_space_sky_land_draw_20;
static NJS_CNK_OBJECT mad_space_sky_land_draw_19;
static NJS_CNK_OBJECT mad_space_sky_land_draw_18;
static NJS_CNK_OBJECT mad_space_sky_land_draw_17;
static NJS_CNK_OBJECT mad_space_sky_land_draw_16;
static NJS_CNK_OBJECT mad_space_sky_land_draw_15;
static NJS_CNK_OBJECT mad_space_sky_land_draw_14;
static NJS_CNK_OBJECT mad_space_sky_land_draw_13;
static NJS_CNK_OBJECT mad_space_sky_land_draw_12;
static NJS_CNK_OBJECT mad_space_sky_land_draw_11;
static NJS_CNK_OBJECT mad_space_sky_land_draw_10;
static NJS_CNK_OBJECT mad_space_sky_land_draw_9;
static NJS_CNK_OBJECT mad_space_sky_land_draw_8;
static NJS_CNK_OBJECT mad_space_sky_land_draw_7;
static NJS_CNK_OBJECT mad_space_sky_land_draw_6;
static NJS_CNK_OBJECT mad_space_sky_land_draw_5;
static NJS_CNK_OBJECT mad_space_sky_land_draw_4;
static NJS_CNK_OBJECT mad_space_sky_land_draw_3;
static NJS_CNK_OBJECT mad_space_sky_land_draw_2;
static NJS_CNK_OBJECT mad_space_sky_land_draw_1;
static NJS_CNK_OBJECT mad_space_sky_land_draw_0;

static struct land_entry mad_space_sky_land_entries[54] = {
#include "stg44/sky_surfaces/mad_space_sky_land_entries.inc"
};
struct land_table mad_space_sky_land = {
#include "stg44/sky_surfaces/mad_space_sky_land.inc"
};
static NJS_TEXNAME mad_space_sky_land_textures[5] = {
#include "stg44/sky_surfaces/mad_space_sky_land_textures.inc"
};
NJS_TEXLIST mad_space_animated_surface_texlist = {
#include "stg44/sky_surfaces/mad_space_animated_surface_texlist.inc"
};
static Sint16 mad_space_sky_land_draw_52_polygons[379] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_52_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_52_vertices[239] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_52_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_52_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_52_model.inc"
};
NJS_CNK_OBJECT mad_space_land_0 = {
#include "stg44/sky_surfaces/mad_space_land_0.inc"
};
static Sint16 mad_space_sky_land_draw_48_polygons[221] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_48_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_48_vertices[135] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_48_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_48_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_48_model.inc"
};
NJS_CNK_OBJECT mad_space_land_1 = {
#include "stg44/sky_surfaces/mad_space_land_1.inc"
};
static Sint16 mad_space_sky_land_draw_44_polygons[149] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_44_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_44_vertices[87] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_44_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_44_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_44_model.inc"
};
NJS_CNK_OBJECT mad_space_land_2 = {
#include "stg44/sky_surfaces/mad_space_land_2.inc"
};
static Sint16 mad_space_sky_land_draw_40_polygons[61] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_40_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_40_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_40_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_40_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_40_model.inc"
};
NJS_CNK_OBJECT mad_space_land_3 = {
#include "stg44/sky_surfaces/mad_space_land_3.inc"
};
static Sint16 mad_space_sky_land_draw_36_polygons[61] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_36_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_36_vertices[43] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_36_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_36_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_36_model.inc"
};
NJS_CNK_OBJECT mad_space_land_4 = {
#include "stg44/sky_surfaces/mad_space_land_4.inc"
};
static Sint16 mad_space_sky_land_draw_35_polygons[133] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_35_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_35_vertices[67] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_35_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_35_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_35_model.inc"
};
NJS_CNK_OBJECT mad_space_land_5 = {
#include "stg44/sky_surfaces/mad_space_land_5.inc"
};
static Sint16 mad_space_sky_land_draw_34_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_34_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_34_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_34_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_34_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_34_model.inc"
};
NJS_CNK_OBJECT mad_space_land_6 = {
#include "stg44/sky_surfaces/mad_space_land_6.inc"
};
static Sint16 mad_space_sky_land_draw_33_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_33_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_33_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_33_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_33_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_33_model.inc"
};
NJS_CNK_OBJECT mad_space_land_9 = {
#include "stg44/sky_surfaces/mad_space_land_9.inc"
};
static Sint16 mad_space_sky_land_draw_32_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_32_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_32_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_32_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_32_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_32_model.inc"
};
NJS_CNK_OBJECT mad_space_land_16 = {
#include "stg44/sky_surfaces/mad_space_land_16.inc"
};
static Sint16 mad_space_sky_land_draw_31_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_31_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_31_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_31_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_31_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_31_model.inc"
};
NJS_CNK_OBJECT mad_space_land_15 = {
#include "stg44/sky_surfaces/mad_space_land_15.inc"
};
static Sint16 mad_space_sky_land_draw_30_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_30_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_30_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_30_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_30_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_30_model.inc"
};
NJS_CNK_OBJECT mad_space_land_14 = {
#include "stg44/sky_surfaces/mad_space_land_14.inc"
};
static Sint16 mad_space_sky_land_draw_29_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_29_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_29_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_29_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_29_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_29_model.inc"
};
NJS_CNK_OBJECT mad_space_land_13 = {
#include "stg44/sky_surfaces/mad_space_land_13.inc"
};
static Sint16 mad_space_sky_land_draw_28_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_28_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_28_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_28_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_28_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_28_model.inc"
};
NJS_CNK_OBJECT mad_space_land_12 = {
#include "stg44/sky_surfaces/mad_space_land_12.inc"
};
static Sint16 mad_space_sky_land_draw_27_polygons[103] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_27_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_27_vertices[51] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_27_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_27_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_27_model.inc"
};
NJS_CNK_OBJECT mad_space_land_11 = {
#include "stg44/sky_surfaces/mad_space_land_11.inc"
};
static Sint16 mad_space_sky_land_draw_24_polygons[155] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_24_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_24_vertices[131] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_24_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_24_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_24_model.inc"
};
NJS_CNK_OBJECT mad_space_land_7 = {
#include "stg44/sky_surfaces/mad_space_land_7.inc"
};
static Sint16 mad_space_sky_land_draw_0_polygons[35] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_0_polygons.inc"
};
static Sint32 mad_space_sky_land_draw_0_vertices[35] = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL mad_space_sky_land_draw_0_model = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_0_model.inc"
};
NJS_CNK_OBJECT mad_space_land_8 = {
#include "stg44/sky_surfaces/mad_space_land_8.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_52 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_52.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_50 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_50.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_49 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_49.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_48 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_48.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_46 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_46.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_45 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_45.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_44 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_44.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_42 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_42.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_41 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_41.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_40 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_40.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_38 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_38.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_37 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_37.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_36 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_36.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_25 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_25.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_24 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_24.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_22 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_22.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_21 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_21.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_20 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_20.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_19 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_19.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_18 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_18.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_17 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_17.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_16 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_16.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_15 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_15.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_14 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_14.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_13 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_13.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_12 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_12.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_11 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_11.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_10 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_10.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_9 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_9.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_8 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_8.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_7 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_7.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_6 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_6.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_5 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_5.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_4 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_4.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_3 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_3.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_2 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_2.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_1 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_1.inc"
};
static NJS_CNK_OBJECT mad_space_sky_land_draw_0 = {
#include "stg44/sky_surfaces/mad_space_sky_land_draw_0.inc"
};
