#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg24/scenery.h"

/* The background land references its archive and each texture in this pool. */
const char engine_pulse_land_filenames[72] = {
#include "stg24/pulse_tex/engine_pulse_land_filenames.inc"
};

/* STG24.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between poison_tex.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry engine_pulse_land_entries[135];
struct land_table engine_pulse_land;
static NJS_TEXNAME engine_pulse_land_textures[5];
NJS_TEXLIST engine_pulse_texlist;
static Sint16 engine_pulse_land_draw_23_polygons[63];
static Sint32 engine_pulse_land_draw_23_vertices[35];
static NJS_CNK_MODEL engine_pulse_land_draw_23_model;
NJS_CNK_OBJECT engine_scroll_object_1;
static Sint16 engine_pulse_land_draw_22_polygons[75];
static Sint32 engine_pulse_land_draw_22_vertices[51];
static NJS_CNK_MODEL engine_pulse_land_draw_22_model;
NJS_CNK_OBJECT engine_scroll_object_2;
static Sint16 engine_pulse_land_draw_7_polygons[29];
static Sint32 engine_pulse_land_draw_7_vertices[27];
static NJS_CNK_MODEL engine_pulse_land_draw_7_model;
NJS_CNK_OBJECT engine_scroll_object_0;
static Sint16 engine_pulse_land_draw_1_polygons[63];
static Sint32 engine_pulse_land_draw_1_vertices[35];
static NJS_CNK_MODEL engine_pulse_land_draw_1_model;
NJS_CNK_OBJECT engine_scroll_object_3;
static Sint16 engine_pulse_land_draw_19_polygons[51];
static Sint32 engine_pulse_land_draw_19_vertices[23];
static NJS_CNK_MODEL engine_pulse_land_draw_19_model;
NJS_CNK_OBJECT engine_scroll_object_4;
static Sint16 engine_pulse_land_draw_2_polygons[49];
static Sint32 engine_pulse_land_draw_2_vertices[35];
static NJS_CNK_MODEL engine_pulse_land_draw_2_model;
NJS_CNK_OBJECT engine_scroll_object_5;
static Sint16 engine_pulse_land_draw_5_polygons[67];
static Sint32 engine_pulse_land_draw_5_vertices[51];
static NJS_CNK_MODEL engine_pulse_land_draw_5_model;
NJS_CNK_OBJECT engine_scroll_object_6;
static Sint16 engine_pulse_land_draw_56_polygons[29];
static Sint32 engine_pulse_land_draw_56_vertices[27];
static NJS_CNK_MODEL engine_pulse_land_draw_56_model;
NJS_CNK_OBJECT engine_scroll_object_7;
static Sint16 engine_pulse_land_draw_63_polygons[29];
static Sint32 engine_pulse_land_draw_63_vertices[27];
static NJS_CNK_MODEL engine_pulse_land_draw_63_model;
NJS_CNK_OBJECT engine_scroll_object_8;
static Sint16 engine_pulse_land_draw_0_polygons[197];
static Sint32 engine_pulse_land_draw_0_vertices[155];
static NJS_CNK_MODEL engine_pulse_land_draw_0_model;
NJS_CNK_OBJECT engine_scroll_object_9;
static NJS_CNK_OBJECT engine_pulse_land_draw_132;
static NJS_CNK_OBJECT engine_pulse_land_draw_131;
static NJS_CNK_OBJECT engine_pulse_land_draw_129;
static NJS_CNK_OBJECT engine_pulse_land_draw_128;
static NJS_CNK_OBJECT engine_pulse_land_draw_127;
static NJS_CNK_OBJECT engine_pulse_land_draw_124;
static NJS_CNK_OBJECT engine_pulse_land_draw_123;
static NJS_CNK_OBJECT engine_pulse_land_draw_122;
static NJS_CNK_OBJECT engine_pulse_land_draw_121;
static NJS_CNK_OBJECT engine_pulse_land_draw_120;
static NJS_CNK_OBJECT engine_pulse_land_draw_119;
static NJS_CNK_OBJECT engine_pulse_land_draw_118;
static NJS_CNK_OBJECT engine_pulse_land_draw_117;
static NJS_CNK_OBJECT engine_pulse_land_draw_116;
static NJS_CNK_OBJECT engine_pulse_land_draw_115;
static NJS_CNK_OBJECT engine_pulse_land_draw_114;
static NJS_CNK_OBJECT engine_pulse_land_draw_113;
static NJS_CNK_OBJECT engine_pulse_land_draw_112;
static NJS_CNK_OBJECT engine_pulse_land_draw_111;
static NJS_CNK_OBJECT engine_pulse_land_draw_110;
static NJS_CNK_OBJECT engine_pulse_land_draw_109;
static NJS_CNK_OBJECT engine_pulse_land_draw_108;
static NJS_CNK_OBJECT engine_pulse_land_draw_107;
static NJS_CNK_OBJECT engine_pulse_land_draw_106;
static NJS_CNK_OBJECT engine_pulse_land_draw_105;
static NJS_CNK_OBJECT engine_pulse_land_draw_104;
static NJS_CNK_OBJECT engine_pulse_land_draw_103;
static NJS_CNK_OBJECT engine_pulse_land_draw_102;
static NJS_CNK_OBJECT engine_pulse_land_draw_101;
static NJS_CNK_OBJECT engine_pulse_land_draw_100;
static NJS_CNK_OBJECT engine_pulse_land_draw_99;
static NJS_CNK_OBJECT engine_pulse_land_draw_98;
static NJS_CNK_OBJECT engine_pulse_land_draw_97;
static NJS_CNK_OBJECT engine_pulse_land_draw_96;
static NJS_CNK_OBJECT engine_pulse_land_draw_95;
static NJS_CNK_OBJECT engine_pulse_land_draw_92;
static NJS_CNK_OBJECT engine_pulse_land_draw_91;
static NJS_CNK_OBJECT engine_pulse_land_draw_90;
static NJS_CNK_OBJECT engine_pulse_land_draw_89;
static NJS_CNK_OBJECT engine_pulse_land_draw_88;
static NJS_CNK_OBJECT engine_pulse_land_draw_87;
static NJS_CNK_OBJECT engine_pulse_land_draw_86;
static NJS_CNK_OBJECT engine_pulse_land_draw_85;
static NJS_CNK_OBJECT engine_pulse_land_draw_84;
static NJS_CNK_OBJECT engine_pulse_land_draw_83;
static NJS_CNK_OBJECT engine_pulse_land_draw_82;
static NJS_CNK_OBJECT engine_pulse_land_draw_81;
static NJS_CNK_OBJECT engine_pulse_land_draw_80;
static NJS_CNK_OBJECT engine_pulse_land_draw_79;
static NJS_CNK_OBJECT engine_pulse_land_draw_78;
static NJS_CNK_OBJECT engine_pulse_land_draw_77;
static NJS_CNK_OBJECT engine_pulse_land_draw_76;
static NJS_CNK_OBJECT engine_pulse_land_draw_75;
static NJS_CNK_OBJECT engine_pulse_land_draw_74;
static NJS_CNK_OBJECT engine_pulse_land_draw_73;
static NJS_CNK_OBJECT engine_pulse_land_draw_72;
static NJS_CNK_OBJECT engine_pulse_land_draw_71;
static NJS_CNK_OBJECT engine_pulse_land_draw_69;
static NJS_CNK_OBJECT engine_pulse_land_draw_68;
static NJS_CNK_OBJECT engine_pulse_land_draw_67;
static NJS_CNK_OBJECT engine_pulse_land_draw_66;
static NJS_CNK_OBJECT engine_pulse_land_draw_65;
static NJS_CNK_OBJECT engine_pulse_land_draw_64;
static NJS_CNK_OBJECT engine_pulse_land_draw_62;
static NJS_CNK_OBJECT engine_pulse_land_draw_61;
static NJS_CNK_OBJECT engine_pulse_land_draw_60;
static NJS_CNK_OBJECT engine_pulse_land_draw_59;
static NJS_CNK_OBJECT engine_pulse_land_draw_58;
static NJS_CNK_OBJECT engine_pulse_land_draw_57;
static NJS_CNK_OBJECT engine_pulse_land_draw_56;
static NJS_CNK_OBJECT engine_pulse_land_draw_55;
static NJS_CNK_OBJECT engine_pulse_land_draw_54;
static NJS_CNK_OBJECT engine_pulse_land_draw_53;
static NJS_CNK_OBJECT engine_pulse_land_draw_52;
static NJS_CNK_OBJECT engine_pulse_land_draw_51;
static NJS_CNK_OBJECT engine_pulse_land_draw_50;
static NJS_CNK_OBJECT engine_pulse_land_draw_49;
static NJS_CNK_OBJECT engine_pulse_land_draw_48;
static NJS_CNK_OBJECT engine_pulse_land_draw_47;
static NJS_CNK_OBJECT engine_pulse_land_draw_46;
static NJS_CNK_OBJECT engine_pulse_land_draw_45;
static NJS_CNK_OBJECT engine_pulse_land_draw_44;
static NJS_CNK_OBJECT engine_pulse_land_draw_43;
static NJS_CNK_OBJECT engine_pulse_land_draw_42;
static NJS_CNK_OBJECT engine_pulse_land_draw_41;
static NJS_CNK_OBJECT engine_pulse_land_draw_40;
static NJS_CNK_OBJECT engine_pulse_land_draw_39;
static NJS_CNK_OBJECT engine_pulse_land_draw_38;
static NJS_CNK_OBJECT engine_pulse_land_draw_37;
static NJS_CNK_OBJECT engine_pulse_land_draw_36;
static NJS_CNK_OBJECT engine_pulse_land_draw_35;
static NJS_CNK_OBJECT engine_pulse_land_draw_34;
static NJS_CNK_OBJECT engine_pulse_land_draw_33;
static NJS_CNK_OBJECT engine_pulse_land_draw_32;
static NJS_CNK_OBJECT engine_pulse_land_draw_31;
static NJS_CNK_OBJECT engine_pulse_land_draw_30;
static NJS_CNK_OBJECT engine_pulse_land_draw_29;
static NJS_CNK_OBJECT engine_pulse_land_draw_28;
static NJS_CNK_OBJECT engine_pulse_land_draw_27;
static NJS_CNK_OBJECT engine_pulse_land_draw_26;
static NJS_CNK_OBJECT engine_pulse_land_draw_25;
static NJS_CNK_OBJECT engine_pulse_land_draw_24;
static NJS_CNK_OBJECT engine_pulse_land_draw_23;
static NJS_CNK_OBJECT engine_pulse_land_draw_22;
static NJS_CNK_OBJECT engine_pulse_land_draw_21;
static NJS_CNK_OBJECT engine_pulse_land_draw_20;
static NJS_CNK_OBJECT engine_pulse_land_draw_19;
static NJS_CNK_OBJECT engine_pulse_land_draw_18;
static NJS_CNK_OBJECT engine_pulse_land_draw_17;
static NJS_CNK_OBJECT engine_pulse_land_draw_16;
static NJS_CNK_OBJECT engine_pulse_land_draw_15;
static NJS_CNK_OBJECT engine_pulse_land_draw_14;
static NJS_CNK_OBJECT engine_pulse_land_draw_13;
static NJS_CNK_OBJECT engine_pulse_land_draw_12;
static NJS_CNK_OBJECT engine_pulse_land_draw_11;
static NJS_CNK_OBJECT engine_pulse_land_draw_10;
static NJS_CNK_OBJECT engine_pulse_land_draw_9;
static NJS_CNK_OBJECT engine_pulse_land_draw_8;
static NJS_CNK_OBJECT engine_pulse_land_draw_7;
static NJS_CNK_OBJECT engine_pulse_land_draw_6;
static NJS_CNK_OBJECT engine_pulse_land_draw_5;
static NJS_CNK_OBJECT engine_pulse_land_draw_4;
static NJS_CNK_OBJECT engine_pulse_land_draw_3;
static NJS_CNK_OBJECT engine_pulse_land_draw_2;
static NJS_CNK_OBJECT engine_pulse_land_draw_1;

static struct land_entry engine_pulse_land_entries[135] = {
#include "stg24/pulse_tex/engine_pulse_land_entries.inc"
};
struct land_table engine_pulse_land = {
#include "stg24/pulse_tex/engine_pulse_land.inc"
};
static NJS_TEXNAME engine_pulse_land_textures[5] = {
#include "stg24/pulse_tex/engine_pulse_land_textures.inc"
};
NJS_TEXLIST engine_pulse_texlist = {
#include "stg24/pulse_tex/engine_pulse_texlist.inc"
};
static Sint16 engine_pulse_land_draw_23_polygons[63] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_23_polygons.inc"
};
static Sint32 engine_pulse_land_draw_23_vertices[35] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_23_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_23_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_23_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_1 = {
#include "stg24/pulse_tex/engine_scroll_object_1.inc"
};
static Sint16 engine_pulse_land_draw_22_polygons[75] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_22_polygons.inc"
};
static Sint32 engine_pulse_land_draw_22_vertices[51] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_22_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_22_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_22_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_2 = {
#include "stg24/pulse_tex/engine_scroll_object_2.inc"
};
static Sint16 engine_pulse_land_draw_7_polygons[29] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_7_polygons.inc"
};
static Sint32 engine_pulse_land_draw_7_vertices[27] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_7_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_7_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_7_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_0 = {
#include "stg24/pulse_tex/engine_scroll_object_0.inc"
};
static Sint16 engine_pulse_land_draw_1_polygons[63] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_1_polygons.inc"
};
static Sint32 engine_pulse_land_draw_1_vertices[35] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_1_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_1_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_3 = {
#include "stg24/pulse_tex/engine_scroll_object_3.inc"
};
static Sint16 engine_pulse_land_draw_19_polygons[51] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_19_polygons.inc"
};
static Sint32 engine_pulse_land_draw_19_vertices[23] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_19_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_19_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_19_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_4 = {
#include "stg24/pulse_tex/engine_scroll_object_4.inc"
};
static Sint16 engine_pulse_land_draw_2_polygons[49] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_2_polygons.inc"
};
static Sint32 engine_pulse_land_draw_2_vertices[35] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_2_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_2_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_2_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_5 = {
#include "stg24/pulse_tex/engine_scroll_object_5.inc"
};
static Sint16 engine_pulse_land_draw_5_polygons[67] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_5_polygons.inc"
};
static Sint32 engine_pulse_land_draw_5_vertices[51] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_5_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_5_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_6 = {
#include "stg24/pulse_tex/engine_scroll_object_6.inc"
};
static Sint16 engine_pulse_land_draw_56_polygons[29] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_56_polygons.inc"
};
static Sint32 engine_pulse_land_draw_56_vertices[27] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_56_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_56_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_56_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_7 = {
#include "stg24/pulse_tex/engine_scroll_object_7.inc"
};
static Sint16 engine_pulse_land_draw_63_polygons[29] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_63_polygons.inc"
};
static Sint32 engine_pulse_land_draw_63_vertices[27] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_63_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_63_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_63_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_8 = {
#include "stg24/pulse_tex/engine_scroll_object_8.inc"
};
static Sint16 engine_pulse_land_draw_0_polygons[197] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_0_polygons.inc"
};
static Sint32 engine_pulse_land_draw_0_vertices[155] = {
#include "stg24/pulse_tex/engine_pulse_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL engine_pulse_land_draw_0_model = {
#include "stg24/pulse_tex/engine_pulse_land_draw_0_model.inc"
};
NJS_CNK_OBJECT engine_scroll_object_9 = {
#include "stg24/pulse_tex/engine_scroll_object_9.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_132 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_132.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_131 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_131.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_129 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_129.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_128 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_128.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_127 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_127.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_124 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_124.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_123 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_123.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_122 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_122.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_121 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_121.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_120 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_120.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_119 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_119.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_118 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_118.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_117 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_117.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_116 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_116.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_115 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_115.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_114 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_114.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_113 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_113.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_112 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_112.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_111 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_111.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_110 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_110.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_109 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_109.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_108 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_108.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_107 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_107.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_106 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_106.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_105 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_105.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_104 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_104.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_103 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_103.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_102 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_102.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_101 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_101.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_100 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_100.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_99 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_99.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_98 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_98.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_97 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_97.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_96 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_96.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_95 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_95.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_92 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_92.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_91 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_91.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_90 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_90.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_89 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_89.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_88 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_88.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_87 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_87.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_86 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_86.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_85 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_85.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_84 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_84.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_83 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_83.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_82 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_82.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_81 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_81.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_80 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_80.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_79 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_79.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_78 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_78.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_77 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_77.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_76 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_76.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_75 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_75.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_74 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_74.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_73 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_73.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_72 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_72.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_71 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_71.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_69 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_69.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_68 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_68.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_67 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_67.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_66 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_66.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_65 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_65.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_64 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_64.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_62 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_62.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_61 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_61.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_60 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_60.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_59 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_59.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_58 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_58.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_57 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_57.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_56 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_56.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_55 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_55.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_54 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_54.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_53 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_53.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_52 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_52.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_51 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_51.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_50 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_50.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_49 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_49.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_48 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_48.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_47 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_47.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_46 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_46.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_45 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_45.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_44 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_44.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_43 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_43.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_42 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_42.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_41 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_41.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_40 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_40.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_39 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_39.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_38 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_38.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_37 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_37.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_36 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_36.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_35 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_35.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_34 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_34.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_33 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_33.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_32 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_32.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_31 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_31.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_30 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_30.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_29 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_29.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_28 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_28.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_27 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_27.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_26 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_26.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_25 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_25.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_24 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_24.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_23 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_23.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_22 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_22.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_21 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_21.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_20 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_20.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_19 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_19.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_18 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_18.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_17 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_17.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_16 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_16.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_15 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_15.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_14 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_14.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_13 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_13.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_12 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_12.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_11 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_11.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_10 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_10.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_9 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_9.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_8 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_8.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_7 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_7.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_6 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_6.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_5 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_5.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_4 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_4.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_3 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_3.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_2 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_2.inc"
};
static NJS_CNK_OBJECT engine_pulse_land_draw_1 = {
#include "stg24/pulse_tex/engine_pulse_land_draw_1.inc"
};
