#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg43/sky_land.h"

/* the file names that cosmic_wall_sky_land and its texture list point at. each
 * starts on a 4-byte boundary */
const char cosmic_wall_sky_land_filenames[70] = {
#include "stg43/sky_land/cosmic_wall_sky_land_filenames.inc"
};

/* STG43.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between cosmic_wall.c and terrain.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry cosmic_wall_sky_land_entries[93];
struct land_table cosmic_wall_sky_land;
static NJS_TEXNAME cosmic_wall_sky_land_textures[5];
NJS_TEXLIST cosmic_wall_animated_surface_texlist;
static Sint16 cosmic_wall_sky_land_draw_60_polygons[251];
static Sint32 cosmic_wall_sky_land_draw_60_vertices[159];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_60_model;
NJS_CNK_OBJECT cosmic_wall_land_0;
static Sint16 cosmic_wall_sky_land_draw_69_polygons[147];
static Sint32 cosmic_wall_sky_land_draw_69_vertices[67];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_69_model;
NJS_CNK_OBJECT cosmic_wall_land_1;
static Sint16 cosmic_wall_sky_land_draw_67_polygons[447];
static Sint32 cosmic_wall_sky_land_draw_67_vertices[451];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_67_model;
NJS_CNK_OBJECT cosmic_wall_land_2;
static Sint16 cosmic_wall_sky_land_draw_59_polygons[109];
static Sint32 cosmic_wall_sky_land_draw_59_vertices[99];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_59_model;
NJS_CNK_OBJECT cosmic_wall_land_3;
static Sint16 cosmic_wall_sky_land_draw_6_polygons[89];
static Sint32 cosmic_wall_sky_land_draw_6_vertices[99];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_6_model;
NJS_CNK_OBJECT cosmic_wall_land_4;
static Sint16 cosmic_wall_sky_land_draw_17_polygons[151];
static Sint32 cosmic_wall_sky_land_draw_17_vertices[151];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_17_model;
NJS_CNK_OBJECT cosmic_wall_land_5;
static Sint16 cosmic_wall_sky_land_draw_1_polygons[29];
static Sint32 cosmic_wall_sky_land_draw_1_vertices[27];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_1_model;
NJS_CNK_OBJECT cosmic_wall_land_6;
static Sint16 cosmic_wall_sky_land_draw_0_polygons[49];
static Sint32 cosmic_wall_sky_land_draw_0_vertices[51];
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_0_model;
NJS_CNK_OBJECT cosmic_wall_land_7;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_92;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_91;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_89;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_88;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_87;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_85;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_83;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_82;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_81;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_80;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_79;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_78;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_77;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_76;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_75;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_74;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_73;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_72;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_71;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_70;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_69;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_68;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_67;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_66;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_65;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_64;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_63;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_62;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_61;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_60;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_57;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_56;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_55;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_54;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_53;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_52;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_51;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_50;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_49;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_48;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_47;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_46;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_45;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_44;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_43;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_42;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_41;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_40;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_39;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_38;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_37;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_36;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_35;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_34;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_33;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_32;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_31;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_30;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_29;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_28;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_27;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_26;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_25;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_24;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_23;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_22;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_21;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_20;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_19;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_18;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_16;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_15;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_14;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_13;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_12;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_11;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_10;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_9;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_8;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_7;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_6;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_5;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_4;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_2;
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_1;

static struct land_entry cosmic_wall_sky_land_entries[93] = {
#include "stg43/sky_land/cosmic_wall_sky_land_entries.inc"
};
struct land_table cosmic_wall_sky_land = {
#include "stg43/sky_land/cosmic_wall_sky_land.inc"
};
static NJS_TEXNAME cosmic_wall_sky_land_textures[5] = {
#include "stg43/sky_land/cosmic_wall_sky_land_textures.inc"
};
NJS_TEXLIST cosmic_wall_animated_surface_texlist = {
#include "stg43/sky_land/cosmic_wall_animated_surface_texlist.inc"
};
static Sint16 cosmic_wall_sky_land_draw_60_polygons[251] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_60_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_60_vertices[159] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_60_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_60_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_60_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_0 = {
#include "stg43/sky_land/cosmic_wall_land_0.inc"
};
static Sint16 cosmic_wall_sky_land_draw_69_polygons[147] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_69_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_69_vertices[67] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_69_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_69_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_69_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_1 = {
#include "stg43/sky_land/cosmic_wall_land_1.inc"
};
static Sint16 cosmic_wall_sky_land_draw_67_polygons[447] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_67_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_67_vertices[451] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_67_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_67_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_67_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_2 = {
#include "stg43/sky_land/cosmic_wall_land_2.inc"
};
static Sint16 cosmic_wall_sky_land_draw_59_polygons[109] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_59_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_59_vertices[99] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_59_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_59_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_59_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_3 = {
#include "stg43/sky_land/cosmic_wall_land_3.inc"
};
static Sint16 cosmic_wall_sky_land_draw_6_polygons[89] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_6_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_6_vertices[99] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_6_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_6_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_6_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_4 = {
#include "stg43/sky_land/cosmic_wall_land_4.inc"
};
static Sint16 cosmic_wall_sky_land_draw_17_polygons[151] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_17_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_17_vertices[151] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_17_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_17_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_17_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_5 = {
#include "stg43/sky_land/cosmic_wall_land_5.inc"
};
static Sint16 cosmic_wall_sky_land_draw_1_polygons[29] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_1_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_1_vertices[27] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_1_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_1_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_6 = {
#include "stg43/sky_land/cosmic_wall_land_6.inc"
};
static Sint16 cosmic_wall_sky_land_draw_0_polygons[49] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_0_polygons.inc"
};
static Sint32 cosmic_wall_sky_land_draw_0_vertices[51] = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL cosmic_wall_sky_land_draw_0_model = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_0_model.inc"
};
NJS_CNK_OBJECT cosmic_wall_land_7 = {
#include "stg43/sky_land/cosmic_wall_land_7.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_92 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_92.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_91 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_91.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_89 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_89.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_88 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_88.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_87 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_87.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_85 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_85.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_83 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_83.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_82 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_82.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_81 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_81.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_80 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_80.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_79 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_79.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_78 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_78.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_77 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_77.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_76 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_76.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_75 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_75.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_74 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_74.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_73 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_73.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_72 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_72.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_71 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_71.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_70 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_70.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_69 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_69.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_68 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_68.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_67 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_67.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_66 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_66.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_65 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_65.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_64 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_64.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_63 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_63.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_62 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_62.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_61 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_61.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_60 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_60.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_57 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_57.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_56 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_56.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_55 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_55.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_54 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_54.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_53 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_53.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_52 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_52.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_51 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_51.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_50 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_50.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_49 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_49.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_48 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_48.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_47 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_47.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_46 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_46.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_45 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_45.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_44 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_44.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_43 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_43.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_42 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_42.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_41 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_41.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_40 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_40.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_39 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_39.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_38 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_38.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_37 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_37.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_36 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_36.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_35 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_35.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_34 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_34.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_33 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_33.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_32 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_32.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_31 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_31.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_30 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_30.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_29 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_29.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_28 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_28.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_27 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_27.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_26 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_26.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_25 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_25.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_24 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_24.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_23 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_23.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_22 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_22.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_21 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_21.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_20 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_20.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_19 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_19.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_18 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_18.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_16 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_16.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_15 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_15.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_14 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_14.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_13 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_13.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_12 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_12.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_11 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_11.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_10 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_10.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_9 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_9.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_8 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_8.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_7 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_7.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_6 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_6.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_5 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_5.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_4 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_4.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_2 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_2.inc"
};
static NJS_CNK_OBJECT cosmic_wall_sky_land_draw_1 = {
#include "stg43/sky_land/cosmic_wall_sky_land_draw_1.inc"
};
