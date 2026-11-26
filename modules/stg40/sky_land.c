#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg40/sky_land.h"

/* the file names that final_chase_sky_land and its texture list point at. each
 * starts on a 4-byte boundary */
const char final_chase_sky_land_filenames[115] = {
#include "stg40/sky_land/final_chase_sky_land_filenames.inc"
};

/* STG40.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between terrain.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry final_chase_sky_land_entries[185];
struct land_table final_chase_sky_land;
static NJS_TEXNAME final_chase_sky_land_textures[9];
NJS_TEXLIST final_chase_animated_surface_texlist;
static Sint16 final_chase_sky_land_draw_0_polygons[251];
static Sint32 final_chase_sky_land_draw_0_vertices[159];
static NJS_CNK_MODEL final_chase_sky_land_draw_0_model;
NJS_CNK_OBJECT background_land_0;
static Sint16 final_chase_sky_land_draw_106_polygons[665];
static Sint32 final_chase_sky_land_draw_106_vertices[431];
static NJS_CNK_MODEL final_chase_sky_land_draw_106_model;
NJS_CNK_OBJECT background_land_1;
static Sint16 final_chase_sky_land_draw_182_polygons[285];
static Sint32 final_chase_sky_land_draw_182_vertices[179];
static NJS_CNK_MODEL final_chase_sky_land_draw_182_model;
NJS_CNK_OBJECT background_land_2;
static Sint16 final_chase_sky_land_draw_181_polygons[313];
static Sint32 final_chase_sky_land_draw_181_vertices[191];
static NJS_CNK_MODEL final_chase_sky_land_draw_181_model;
NJS_CNK_OBJECT background_land_3;
static Sint16 final_chase_sky_land_draw_180_polygons[267];
static Sint32 final_chase_sky_land_draw_180_vertices[167];
static NJS_CNK_MODEL final_chase_sky_land_draw_180_model;
NJS_CNK_OBJECT background_land_4;
static Sint16 final_chase_sky_land_draw_57_polygons[483];
static Sint32 final_chase_sky_land_draw_57_vertices[307];
static NJS_CNK_MODEL final_chase_sky_land_draw_57_model;
NJS_CNK_OBJECT background_land_5;
static Sint16 final_chase_sky_land_draw_175_polygons[491];
static Sint32 final_chase_sky_land_draw_175_vertices[311];
static NJS_CNK_MODEL final_chase_sky_land_draw_175_model;
NJS_CNK_OBJECT background_land_6;
static Sint16 final_chase_sky_land_draw_174_polygons[1259];
static Sint32 final_chase_sky_land_draw_174_vertices[827];
static NJS_CNK_MODEL final_chase_sky_land_draw_174_model;
NJS_CNK_OBJECT background_land_7;
static Sint16 final_chase_sky_land_draw_167_polygons[273];
static Sint32 final_chase_sky_land_draw_167_vertices[147];
static NJS_CNK_MODEL final_chase_sky_land_draw_167_model;
NJS_CNK_OBJECT background_land_8;
static Sint16 final_chase_sky_land_draw_2_polygons[145];
static Sint32 final_chase_sky_land_draw_2_vertices[67];
static NJS_CNK_MODEL final_chase_sky_land_draw_2_model;
NJS_CNK_OBJECT background_land_11;
static Sint16 final_chase_sky_land_draw_9_polygons[29];
static Sint32 final_chase_sky_land_draw_9_vertices[27];
static NJS_CNK_MODEL final_chase_sky_land_draw_9_model;
NJS_CNK_OBJECT background_land_9;
static Sint16 final_chase_sky_land_draw_8_polygons[49];
static Sint32 final_chase_sky_land_draw_8_vertices[51];
static NJS_CNK_MODEL final_chase_sky_land_draw_8_model;
NJS_CNK_OBJECT background_land_10;
static Sint16 final_chase_sky_land_draw_125_polygons[219];
static Sint32 final_chase_sky_land_draw_125_vertices[99];
static NJS_CNK_MODEL final_chase_sky_land_draw_125_model;
NJS_CNK_OBJECT background_land_12;
static Sint16 final_chase_sky_land_draw_120_polygons[697];
static Sint32 final_chase_sky_land_draw_120_vertices[419];
static NJS_CNK_MODEL final_chase_sky_land_draw_120_model;
NJS_CNK_OBJECT background_land_14;
static Sint16 final_chase_sky_land_draw_105_polygons[269];
static Sint32 final_chase_sky_land_draw_105_vertices[147];
static NJS_CNK_MODEL final_chase_sky_land_draw_105_model;
NJS_CNK_OBJECT background_land_15;
static Sint16 final_chase_sky_land_draw_78_polygons[145];
static Sint32 final_chase_sky_land_draw_78_vertices[51];
static NJS_CNK_MODEL final_chase_sky_land_draw_78_model;
NJS_CNK_OBJECT background_land_16;
static Sint16 final_chase_sky_land_draw_74_polygons[563];
static Sint32 final_chase_sky_land_draw_74_vertices[343];
static NJS_CNK_MODEL final_chase_sky_land_draw_74_model;
NJS_CNK_OBJECT background_land_17;
static Sint16 final_chase_sky_land_draw_73_polygons[593];
static Sint32 final_chase_sky_land_draw_73_vertices[363];
static NJS_CNK_MODEL final_chase_sky_land_draw_73_model;
NJS_CNK_OBJECT background_land_18;
static Sint16 final_chase_sky_land_draw_71_polygons[577];
static Sint32 final_chase_sky_land_draw_71_vertices[339];
static NJS_CNK_MODEL final_chase_sky_land_draw_71_model;
NJS_CNK_OBJECT background_land_19;
static Sint16 final_chase_sky_land_draw_63_polygons[273];
static Sint32 final_chase_sky_land_draw_63_vertices[147];
static NJS_CNK_MODEL final_chase_sky_land_draw_63_model;
NJS_CNK_OBJECT background_land_20;
static Sint16 final_chase_sky_land_draw_62_polygons[491];
static Sint32 final_chase_sky_land_draw_62_vertices[311];
static NJS_CNK_MODEL final_chase_sky_land_draw_62_model;
NJS_CNK_OBJECT background_land_21;
static Sint16 final_chase_sky_land_draw_53_polygons[563];
static Sint32 final_chase_sky_land_draw_53_vertices[331];
static NJS_CNK_MODEL final_chase_sky_land_draw_53_model;
NJS_CNK_OBJECT background_land_22;
static Sint16 final_chase_sky_land_draw_52_polygons[635];
static Sint32 final_chase_sky_land_draw_52_vertices[391];
static NJS_CNK_MODEL final_chase_sky_land_draw_52_model;
NJS_CNK_OBJECT background_land_23;
static Sint16 final_chase_sky_land_draw_51_polygons[1019];
static Sint32 final_chase_sky_land_draw_51_vertices[647];
static NJS_CNK_MODEL final_chase_sky_land_draw_51_model;
NJS_CNK_OBJECT background_land_24;
static Sint16 final_chase_sky_land_draw_42_polygons[1085];
static Sint32 final_chase_sky_land_draw_42_vertices[687];
static NJS_CNK_MODEL final_chase_sky_land_draw_42_model;
NJS_CNK_OBJECT background_land_25;
static Sint16 final_chase_sky_land_draw_38_polygons[197];
static Sint32 final_chase_sky_land_draw_38_vertices[99];
static NJS_CNK_MODEL final_chase_sky_land_draw_38_model;
NJS_CNK_OBJECT background_land_26;
static Sint16 final_chase_sky_land_draw_15_polygons[87];
static Sint32 final_chase_sky_land_draw_15_vertices[51];
static NJS_CNK_MODEL final_chase_sky_land_draw_15_model;
NJS_CNK_OBJECT background_land_27;
static Sint16 final_chase_sky_land_draw_12_polygons[219];
static Sint32 final_chase_sky_land_draw_12_vertices[99];
static NJS_CNK_MODEL final_chase_sky_land_draw_12_model;
NJS_CNK_OBJECT background_land_13;
static NJS_CNK_OBJECT final_chase_sky_land_draw_179;
static NJS_CNK_OBJECT final_chase_sky_land_draw_178;
static NJS_CNK_OBJECT final_chase_sky_land_draw_177;
static NJS_CNK_OBJECT final_chase_sky_land_draw_173;
static NJS_CNK_OBJECT final_chase_sky_land_draw_172;
static NJS_CNK_OBJECT final_chase_sky_land_draw_171;
static NJS_CNK_OBJECT final_chase_sky_land_draw_170;
static NJS_CNK_OBJECT final_chase_sky_land_draw_169;
static NJS_CNK_OBJECT final_chase_sky_land_draw_168;
static NJS_CNK_OBJECT final_chase_sky_land_draw_166;
static NJS_CNK_OBJECT final_chase_sky_land_draw_165;
static NJS_CNK_OBJECT final_chase_sky_land_draw_162;
static NJS_CNK_OBJECT final_chase_sky_land_draw_160;
static NJS_CNK_OBJECT final_chase_sky_land_draw_159;
static NJS_CNK_OBJECT final_chase_sky_land_draw_158;
static NJS_CNK_OBJECT final_chase_sky_land_draw_157;
static NJS_CNK_OBJECT final_chase_sky_land_draw_156;
static NJS_CNK_OBJECT final_chase_sky_land_draw_155;
static NJS_CNK_OBJECT final_chase_sky_land_draw_154;
static NJS_CNK_OBJECT final_chase_sky_land_draw_153;
static NJS_CNK_OBJECT final_chase_sky_land_draw_152;
static NJS_CNK_OBJECT final_chase_sky_land_draw_151;
static NJS_CNK_OBJECT final_chase_sky_land_draw_150;
static NJS_CNK_OBJECT final_chase_sky_land_draw_149;
static NJS_CNK_OBJECT final_chase_sky_land_draw_148;
static NJS_CNK_OBJECT final_chase_sky_land_draw_147;
static NJS_CNK_OBJECT final_chase_sky_land_draw_146;
static NJS_CNK_OBJECT final_chase_sky_land_draw_145;
static NJS_CNK_OBJECT final_chase_sky_land_draw_144;
static NJS_CNK_OBJECT final_chase_sky_land_draw_143;
static NJS_CNK_OBJECT final_chase_sky_land_draw_142;
static NJS_CNK_OBJECT final_chase_sky_land_draw_141;
static NJS_CNK_OBJECT final_chase_sky_land_draw_140;
static NJS_CNK_OBJECT final_chase_sky_land_draw_139;
static NJS_CNK_OBJECT final_chase_sky_land_draw_138;
static NJS_CNK_OBJECT final_chase_sky_land_draw_137;
static NJS_CNK_OBJECT final_chase_sky_land_draw_136;
static NJS_CNK_OBJECT final_chase_sky_land_draw_135;
static NJS_CNK_OBJECT final_chase_sky_land_draw_134;
static NJS_CNK_OBJECT final_chase_sky_land_draw_133;
static NJS_CNK_OBJECT final_chase_sky_land_draw_132;
static NJS_CNK_OBJECT final_chase_sky_land_draw_131;
static NJS_CNK_OBJECT final_chase_sky_land_draw_130;
static NJS_CNK_OBJECT final_chase_sky_land_draw_129;
static NJS_CNK_OBJECT final_chase_sky_land_draw_128;
static NJS_CNK_OBJECT final_chase_sky_land_draw_127;
static NJS_CNK_OBJECT final_chase_sky_land_draw_126;
static NJS_CNK_OBJECT final_chase_sky_land_draw_124;
static NJS_CNK_OBJECT final_chase_sky_land_draw_123;
static NJS_CNK_OBJECT final_chase_sky_land_draw_122;
static NJS_CNK_OBJECT final_chase_sky_land_draw_121;
static NJS_CNK_OBJECT final_chase_sky_land_draw_119;
static NJS_CNK_OBJECT final_chase_sky_land_draw_118;
static NJS_CNK_OBJECT final_chase_sky_land_draw_117;
static NJS_CNK_OBJECT final_chase_sky_land_draw_116;
static NJS_CNK_OBJECT final_chase_sky_land_draw_115;
static NJS_CNK_OBJECT final_chase_sky_land_draw_114;
static NJS_CNK_OBJECT final_chase_sky_land_draw_113;
static NJS_CNK_OBJECT final_chase_sky_land_draw_112;
static NJS_CNK_OBJECT final_chase_sky_land_draw_111;
static NJS_CNK_OBJECT final_chase_sky_land_draw_110;
static NJS_CNK_OBJECT final_chase_sky_land_draw_109;
static NJS_CNK_OBJECT final_chase_sky_land_draw_108;
static NJS_CNK_OBJECT final_chase_sky_land_draw_107;
static NJS_CNK_OBJECT final_chase_sky_land_draw_106;
static NJS_CNK_OBJECT final_chase_sky_land_draw_104;
static NJS_CNK_OBJECT final_chase_sky_land_draw_103;
static NJS_CNK_OBJECT final_chase_sky_land_draw_102;
static NJS_CNK_OBJECT final_chase_sky_land_draw_101;
static NJS_CNK_OBJECT final_chase_sky_land_draw_100;
static NJS_CNK_OBJECT final_chase_sky_land_draw_99;
static NJS_CNK_OBJECT final_chase_sky_land_draw_98;
static NJS_CNK_OBJECT final_chase_sky_land_draw_97;
static NJS_CNK_OBJECT final_chase_sky_land_draw_96;
static NJS_CNK_OBJECT final_chase_sky_land_draw_95;
static NJS_CNK_OBJECT final_chase_sky_land_draw_94;
static NJS_CNK_OBJECT final_chase_sky_land_draw_93;
static NJS_CNK_OBJECT final_chase_sky_land_draw_92;
static NJS_CNK_OBJECT final_chase_sky_land_draw_91;
static NJS_CNK_OBJECT final_chase_sky_land_draw_90;
static NJS_CNK_OBJECT final_chase_sky_land_draw_89;
static NJS_CNK_OBJECT final_chase_sky_land_draw_88;
static NJS_CNK_OBJECT final_chase_sky_land_draw_87;
static NJS_CNK_OBJECT final_chase_sky_land_draw_86;
static NJS_CNK_OBJECT final_chase_sky_land_draw_85;
static NJS_CNK_OBJECT final_chase_sky_land_draw_84;
static NJS_CNK_OBJECT final_chase_sky_land_draw_83;
static NJS_CNK_OBJECT final_chase_sky_land_draw_82;
static NJS_CNK_OBJECT final_chase_sky_land_draw_81;
static NJS_CNK_OBJECT final_chase_sky_land_draw_80;
static NJS_CNK_OBJECT final_chase_sky_land_draw_79;
static NJS_CNK_OBJECT final_chase_sky_land_draw_77;
static NJS_CNK_OBJECT final_chase_sky_land_draw_76;
static NJS_CNK_OBJECT final_chase_sky_land_draw_75;
static NJS_CNK_OBJECT final_chase_sky_land_draw_72;
static NJS_CNK_OBJECT final_chase_sky_land_draw_70;
static NJS_CNK_OBJECT final_chase_sky_land_draw_69;
static NJS_CNK_OBJECT final_chase_sky_land_draw_68;
static NJS_CNK_OBJECT final_chase_sky_land_draw_67;
static NJS_CNK_OBJECT final_chase_sky_land_draw_66;
static NJS_CNK_OBJECT final_chase_sky_land_draw_65;
static NJS_CNK_OBJECT final_chase_sky_land_draw_64;
static NJS_CNK_OBJECT final_chase_sky_land_draw_61;
static NJS_CNK_OBJECT final_chase_sky_land_draw_60;
static NJS_CNK_OBJECT final_chase_sky_land_draw_59;
static NJS_CNK_OBJECT final_chase_sky_land_draw_58;
static NJS_CNK_OBJECT final_chase_sky_land_draw_57;
static NJS_CNK_OBJECT final_chase_sky_land_draw_56;
static NJS_CNK_OBJECT final_chase_sky_land_draw_55;
static NJS_CNK_OBJECT final_chase_sky_land_draw_54;
static NJS_CNK_OBJECT final_chase_sky_land_draw_50;
static NJS_CNK_OBJECT final_chase_sky_land_draw_49;
static NJS_CNK_OBJECT final_chase_sky_land_draw_48;
static NJS_CNK_OBJECT final_chase_sky_land_draw_47;
static NJS_CNK_OBJECT final_chase_sky_land_draw_46;
static NJS_CNK_OBJECT final_chase_sky_land_draw_45;
static NJS_CNK_OBJECT final_chase_sky_land_draw_44;
static NJS_CNK_OBJECT final_chase_sky_land_draw_43;
static NJS_CNK_OBJECT final_chase_sky_land_draw_41;
static NJS_CNK_OBJECT final_chase_sky_land_draw_40;
static NJS_CNK_OBJECT final_chase_sky_land_draw_39;
static NJS_CNK_OBJECT final_chase_sky_land_draw_37;
static NJS_CNK_OBJECT final_chase_sky_land_draw_36;
static NJS_CNK_OBJECT final_chase_sky_land_draw_35;
static NJS_CNK_OBJECT final_chase_sky_land_draw_34;
static NJS_CNK_OBJECT final_chase_sky_land_draw_33;
static NJS_CNK_OBJECT final_chase_sky_land_draw_32;
static NJS_CNK_OBJECT final_chase_sky_land_draw_31;
static NJS_CNK_OBJECT final_chase_sky_land_draw_30;
static NJS_CNK_OBJECT final_chase_sky_land_draw_29;
static NJS_CNK_OBJECT final_chase_sky_land_draw_28;
static NJS_CNK_OBJECT final_chase_sky_land_draw_27;
static NJS_CNK_OBJECT final_chase_sky_land_draw_26;
static NJS_CNK_OBJECT final_chase_sky_land_draw_25;
static NJS_CNK_OBJECT final_chase_sky_land_draw_24;
static NJS_CNK_OBJECT final_chase_sky_land_draw_23;
static NJS_CNK_OBJECT final_chase_sky_land_draw_22;
static NJS_CNK_OBJECT final_chase_sky_land_draw_21;
static NJS_CNK_OBJECT final_chase_sky_land_draw_20;
static NJS_CNK_OBJECT final_chase_sky_land_draw_19;
static NJS_CNK_OBJECT final_chase_sky_land_draw_18;
static NJS_CNK_OBJECT final_chase_sky_land_draw_17;
static NJS_CNK_OBJECT final_chase_sky_land_draw_16;
static NJS_CNK_OBJECT final_chase_sky_land_draw_14;
static NJS_CNK_OBJECT final_chase_sky_land_draw_12;
static NJS_CNK_OBJECT final_chase_sky_land_draw_11;
static NJS_CNK_OBJECT final_chase_sky_land_draw_10;
static NJS_CNK_OBJECT final_chase_sky_land_draw_9;
static NJS_CNK_OBJECT final_chase_sky_land_draw_8;
static NJS_CNK_OBJECT final_chase_sky_land_draw_7;
static NJS_CNK_OBJECT final_chase_sky_land_draw_6;
static NJS_CNK_OBJECT final_chase_sky_land_draw_5;
static NJS_CNK_OBJECT final_chase_sky_land_draw_4;
static NJS_CNK_OBJECT final_chase_sky_land_draw_3;
static NJS_CNK_OBJECT final_chase_sky_land_draw_2;
static NJS_CNK_OBJECT final_chase_sky_land_draw_1;
static NJS_CNK_OBJECT final_chase_sky_land_draw_0;

static struct land_entry final_chase_sky_land_entries[185] = {
#include "stg40/sky_land/final_chase_sky_land_entries.inc"
};
struct land_table final_chase_sky_land = {
#include "stg40/sky_land/final_chase_sky_land.inc"
};
static NJS_TEXNAME final_chase_sky_land_textures[9] = {
#include "stg40/sky_land/final_chase_sky_land_textures.inc"
};
NJS_TEXLIST final_chase_animated_surface_texlist = {
#include "stg40/sky_land/final_chase_animated_surface_texlist.inc"
};
static Sint16 final_chase_sky_land_draw_0_polygons[251] = {
#include "stg40/sky_land/final_chase_sky_land_draw_0_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_0_vertices[159] = {
#include "stg40/sky_land/final_chase_sky_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_0_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_0_model.inc"
};
NJS_CNK_OBJECT background_land_0 = {
#include "stg40/sky_land/background_land_0.inc"
};
static Sint16 final_chase_sky_land_draw_106_polygons[665] = {
#include "stg40/sky_land/final_chase_sky_land_draw_106_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_106_vertices[431] = {
#include "stg40/sky_land/final_chase_sky_land_draw_106_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_106_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_106_model.inc"
};
NJS_CNK_OBJECT background_land_1 = {
#include "stg40/sky_land/background_land_1.inc"
};
static Sint16 final_chase_sky_land_draw_182_polygons[285] = {
#include "stg40/sky_land/final_chase_sky_land_draw_182_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_182_vertices[179] = {
#include "stg40/sky_land/final_chase_sky_land_draw_182_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_182_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_182_model.inc"
};
NJS_CNK_OBJECT background_land_2 = {
#include "stg40/sky_land/background_land_2.inc"
};
static Sint16 final_chase_sky_land_draw_181_polygons[313] = {
#include "stg40/sky_land/final_chase_sky_land_draw_181_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_181_vertices[191] = {
#include "stg40/sky_land/final_chase_sky_land_draw_181_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_181_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_181_model.inc"
};
NJS_CNK_OBJECT background_land_3 = {
#include "stg40/sky_land/background_land_3.inc"
};
static Sint16 final_chase_sky_land_draw_180_polygons[267] = {
#include "stg40/sky_land/final_chase_sky_land_draw_180_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_180_vertices[167] = {
#include "stg40/sky_land/final_chase_sky_land_draw_180_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_180_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_180_model.inc"
};
NJS_CNK_OBJECT background_land_4 = {
#include "stg40/sky_land/background_land_4.inc"
};
static Sint16 final_chase_sky_land_draw_57_polygons[483] = {
#include "stg40/sky_land/final_chase_sky_land_draw_57_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_57_vertices[307] = {
#include "stg40/sky_land/final_chase_sky_land_draw_57_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_57_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_57_model.inc"
};
NJS_CNK_OBJECT background_land_5 = {
#include "stg40/sky_land/background_land_5.inc"
};
static Sint16 final_chase_sky_land_draw_175_polygons[491] = {
#include "stg40/sky_land/final_chase_sky_land_draw_175_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_175_vertices[311] = {
#include "stg40/sky_land/final_chase_sky_land_draw_175_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_175_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_175_model.inc"
};
NJS_CNK_OBJECT background_land_6 = {
#include "stg40/sky_land/background_land_6.inc"
};
static Sint16 final_chase_sky_land_draw_174_polygons[1259] = {
#include "stg40/sky_land/final_chase_sky_land_draw_174_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_174_vertices[827] = {
#include "stg40/sky_land/final_chase_sky_land_draw_174_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_174_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_174_model.inc"
};
NJS_CNK_OBJECT background_land_7 = {
#include "stg40/sky_land/background_land_7.inc"
};
static Sint16 final_chase_sky_land_draw_167_polygons[273] = {
#include "stg40/sky_land/final_chase_sky_land_draw_167_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_167_vertices[147] = {
#include "stg40/sky_land/final_chase_sky_land_draw_167_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_167_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_167_model.inc"
};
NJS_CNK_OBJECT background_land_8 = {
#include "stg40/sky_land/background_land_8.inc"
};
static Sint16 final_chase_sky_land_draw_2_polygons[145] = {
#include "stg40/sky_land/final_chase_sky_land_draw_2_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_2_vertices[67] = {
#include "stg40/sky_land/final_chase_sky_land_draw_2_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_2_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_2_model.inc"
};
NJS_CNK_OBJECT background_land_11 = {
#include "stg40/sky_land/background_land_11.inc"
};
static Sint16 final_chase_sky_land_draw_9_polygons[29] = {
#include "stg40/sky_land/final_chase_sky_land_draw_9_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_9_vertices[27] = {
#include "stg40/sky_land/final_chase_sky_land_draw_9_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_9_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_9_model.inc"
};
NJS_CNK_OBJECT background_land_9 = {
#include "stg40/sky_land/background_land_9.inc"
};
static Sint16 final_chase_sky_land_draw_8_polygons[49] = {
#include "stg40/sky_land/final_chase_sky_land_draw_8_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_8_vertices[51] = {
#include "stg40/sky_land/final_chase_sky_land_draw_8_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_8_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_8_model.inc"
};
NJS_CNK_OBJECT background_land_10 = {
#include "stg40/sky_land/background_land_10.inc"
};
static Sint16 final_chase_sky_land_draw_125_polygons[219] = {
#include "stg40/sky_land/final_chase_sky_land_draw_125_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_125_vertices[99] = {
#include "stg40/sky_land/final_chase_sky_land_draw_125_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_125_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_125_model.inc"
};
NJS_CNK_OBJECT background_land_12 = {
#include "stg40/sky_land/background_land_12.inc"
};
static Sint16 final_chase_sky_land_draw_120_polygons[697] = {
#include "stg40/sky_land/final_chase_sky_land_draw_120_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_120_vertices[419] = {
#include "stg40/sky_land/final_chase_sky_land_draw_120_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_120_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_120_model.inc"
};
NJS_CNK_OBJECT background_land_14 = {
#include "stg40/sky_land/background_land_14.inc"
};
static Sint16 final_chase_sky_land_draw_105_polygons[269] = {
#include "stg40/sky_land/final_chase_sky_land_draw_105_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_105_vertices[147] = {
#include "stg40/sky_land/final_chase_sky_land_draw_105_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_105_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_105_model.inc"
};
NJS_CNK_OBJECT background_land_15 = {
#include "stg40/sky_land/background_land_15.inc"
};
static Sint16 final_chase_sky_land_draw_78_polygons[145] = {
#include "stg40/sky_land/final_chase_sky_land_draw_78_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_78_vertices[51] = {
#include "stg40/sky_land/final_chase_sky_land_draw_78_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_78_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_78_model.inc"
};
NJS_CNK_OBJECT background_land_16 = {
#include "stg40/sky_land/background_land_16.inc"
};
static Sint16 final_chase_sky_land_draw_74_polygons[563] = {
#include "stg40/sky_land/final_chase_sky_land_draw_74_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_74_vertices[343] = {
#include "stg40/sky_land/final_chase_sky_land_draw_74_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_74_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_74_model.inc"
};
NJS_CNK_OBJECT background_land_17 = {
#include "stg40/sky_land/background_land_17.inc"
};
static Sint16 final_chase_sky_land_draw_73_polygons[593] = {
#include "stg40/sky_land/final_chase_sky_land_draw_73_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_73_vertices[363] = {
#include "stg40/sky_land/final_chase_sky_land_draw_73_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_73_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_73_model.inc"
};
NJS_CNK_OBJECT background_land_18 = {
#include "stg40/sky_land/background_land_18.inc"
};
static Sint16 final_chase_sky_land_draw_71_polygons[577] = {
#include "stg40/sky_land/final_chase_sky_land_draw_71_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_71_vertices[339] = {
#include "stg40/sky_land/final_chase_sky_land_draw_71_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_71_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_71_model.inc"
};
NJS_CNK_OBJECT background_land_19 = {
#include "stg40/sky_land/background_land_19.inc"
};
static Sint16 final_chase_sky_land_draw_63_polygons[273] = {
#include "stg40/sky_land/final_chase_sky_land_draw_63_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_63_vertices[147] = {
#include "stg40/sky_land/final_chase_sky_land_draw_63_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_63_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_63_model.inc"
};
NJS_CNK_OBJECT background_land_20 = {
#include "stg40/sky_land/background_land_20.inc"
};
static Sint16 final_chase_sky_land_draw_62_polygons[491] = {
#include "stg40/sky_land/final_chase_sky_land_draw_62_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_62_vertices[311] = {
#include "stg40/sky_land/final_chase_sky_land_draw_62_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_62_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_62_model.inc"
};
NJS_CNK_OBJECT background_land_21 = {
#include "stg40/sky_land/background_land_21.inc"
};
static Sint16 final_chase_sky_land_draw_53_polygons[563] = {
#include "stg40/sky_land/final_chase_sky_land_draw_53_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_53_vertices[331] = {
#include "stg40/sky_land/final_chase_sky_land_draw_53_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_53_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_53_model.inc"
};
NJS_CNK_OBJECT background_land_22 = {
#include "stg40/sky_land/background_land_22.inc"
};
static Sint16 final_chase_sky_land_draw_52_polygons[635] = {
#include "stg40/sky_land/final_chase_sky_land_draw_52_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_52_vertices[391] = {
#include "stg40/sky_land/final_chase_sky_land_draw_52_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_52_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_52_model.inc"
};
NJS_CNK_OBJECT background_land_23 = {
#include "stg40/sky_land/background_land_23.inc"
};
static Sint16 final_chase_sky_land_draw_51_polygons[1019] = {
#include "stg40/sky_land/final_chase_sky_land_draw_51_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_51_vertices[647] = {
#include "stg40/sky_land/final_chase_sky_land_draw_51_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_51_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_51_model.inc"
};
NJS_CNK_OBJECT background_land_24 = {
#include "stg40/sky_land/background_land_24.inc"
};
static Sint16 final_chase_sky_land_draw_42_polygons[1085] = {
#include "stg40/sky_land/final_chase_sky_land_draw_42_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_42_vertices[687] = {
#include "stg40/sky_land/final_chase_sky_land_draw_42_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_42_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_42_model.inc"
};
NJS_CNK_OBJECT background_land_25 = {
#include "stg40/sky_land/background_land_25.inc"
};
static Sint16 final_chase_sky_land_draw_38_polygons[197] = {
#include "stg40/sky_land/final_chase_sky_land_draw_38_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_38_vertices[99] = {
#include "stg40/sky_land/final_chase_sky_land_draw_38_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_38_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_38_model.inc"
};
NJS_CNK_OBJECT background_land_26 = {
#include "stg40/sky_land/background_land_26.inc"
};
static Sint16 final_chase_sky_land_draw_15_polygons[87] = {
#include "stg40/sky_land/final_chase_sky_land_draw_15_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_15_vertices[51] = {
#include "stg40/sky_land/final_chase_sky_land_draw_15_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_15_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_15_model.inc"
};
NJS_CNK_OBJECT background_land_27 = {
#include "stg40/sky_land/background_land_27.inc"
};
static Sint16 final_chase_sky_land_draw_12_polygons[219] = {
#include "stg40/sky_land/final_chase_sky_land_draw_12_polygons.inc"
};
static Sint32 final_chase_sky_land_draw_12_vertices[99] = {
#include "stg40/sky_land/final_chase_sky_land_draw_12_vertices.inc"
};
static NJS_CNK_MODEL final_chase_sky_land_draw_12_model = {
#include "stg40/sky_land/final_chase_sky_land_draw_12_model.inc"
};
NJS_CNK_OBJECT background_land_13 = {
#include "stg40/sky_land/background_land_13.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_179 = {
#include "stg40/sky_land/final_chase_sky_land_draw_179.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_178 = {
#include "stg40/sky_land/final_chase_sky_land_draw_178.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_177 = {
#include "stg40/sky_land/final_chase_sky_land_draw_177.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_173 = {
#include "stg40/sky_land/final_chase_sky_land_draw_173.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_172 = {
#include "stg40/sky_land/final_chase_sky_land_draw_172.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_171 = {
#include "stg40/sky_land/final_chase_sky_land_draw_171.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_170 = {
#include "stg40/sky_land/final_chase_sky_land_draw_170.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_169 = {
#include "stg40/sky_land/final_chase_sky_land_draw_169.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_168 = {
#include "stg40/sky_land/final_chase_sky_land_draw_168.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_166 = {
#include "stg40/sky_land/final_chase_sky_land_draw_166.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_165 = {
#include "stg40/sky_land/final_chase_sky_land_draw_165.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_162 = {
#include "stg40/sky_land/final_chase_sky_land_draw_162.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_160 = {
#include "stg40/sky_land/final_chase_sky_land_draw_160.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_159 = {
#include "stg40/sky_land/final_chase_sky_land_draw_159.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_158 = {
#include "stg40/sky_land/final_chase_sky_land_draw_158.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_157 = {
#include "stg40/sky_land/final_chase_sky_land_draw_157.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_156 = {
#include "stg40/sky_land/final_chase_sky_land_draw_156.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_155 = {
#include "stg40/sky_land/final_chase_sky_land_draw_155.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_154 = {
#include "stg40/sky_land/final_chase_sky_land_draw_154.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_153 = {
#include "stg40/sky_land/final_chase_sky_land_draw_153.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_152 = {
#include "stg40/sky_land/final_chase_sky_land_draw_152.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_151 = {
#include "stg40/sky_land/final_chase_sky_land_draw_151.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_150 = {
#include "stg40/sky_land/final_chase_sky_land_draw_150.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_149 = {
#include "stg40/sky_land/final_chase_sky_land_draw_149.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_148 = {
#include "stg40/sky_land/final_chase_sky_land_draw_148.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_147 = {
#include "stg40/sky_land/final_chase_sky_land_draw_147.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_146 = {
#include "stg40/sky_land/final_chase_sky_land_draw_146.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_145 = {
#include "stg40/sky_land/final_chase_sky_land_draw_145.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_144 = {
#include "stg40/sky_land/final_chase_sky_land_draw_144.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_143 = {
#include "stg40/sky_land/final_chase_sky_land_draw_143.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_142 = {
#include "stg40/sky_land/final_chase_sky_land_draw_142.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_141 = {
#include "stg40/sky_land/final_chase_sky_land_draw_141.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_140 = {
#include "stg40/sky_land/final_chase_sky_land_draw_140.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_139 = {
#include "stg40/sky_land/final_chase_sky_land_draw_139.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_138 = {
#include "stg40/sky_land/final_chase_sky_land_draw_138.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_137 = {
#include "stg40/sky_land/final_chase_sky_land_draw_137.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_136 = {
#include "stg40/sky_land/final_chase_sky_land_draw_136.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_135 = {
#include "stg40/sky_land/final_chase_sky_land_draw_135.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_134 = {
#include "stg40/sky_land/final_chase_sky_land_draw_134.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_133 = {
#include "stg40/sky_land/final_chase_sky_land_draw_133.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_132 = {
#include "stg40/sky_land/final_chase_sky_land_draw_132.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_131 = {
#include "stg40/sky_land/final_chase_sky_land_draw_131.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_130 = {
#include "stg40/sky_land/final_chase_sky_land_draw_130.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_129 = {
#include "stg40/sky_land/final_chase_sky_land_draw_129.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_128 = {
#include "stg40/sky_land/final_chase_sky_land_draw_128.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_127 = {
#include "stg40/sky_land/final_chase_sky_land_draw_127.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_126 = {
#include "stg40/sky_land/final_chase_sky_land_draw_126.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_124 = {
#include "stg40/sky_land/final_chase_sky_land_draw_124.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_123 = {
#include "stg40/sky_land/final_chase_sky_land_draw_123.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_122 = {
#include "stg40/sky_land/final_chase_sky_land_draw_122.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_121 = {
#include "stg40/sky_land/final_chase_sky_land_draw_121.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_119 = {
#include "stg40/sky_land/final_chase_sky_land_draw_119.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_118 = {
#include "stg40/sky_land/final_chase_sky_land_draw_118.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_117 = {
#include "stg40/sky_land/final_chase_sky_land_draw_117.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_116 = {
#include "stg40/sky_land/final_chase_sky_land_draw_116.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_115 = {
#include "stg40/sky_land/final_chase_sky_land_draw_115.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_114 = {
#include "stg40/sky_land/final_chase_sky_land_draw_114.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_113 = {
#include "stg40/sky_land/final_chase_sky_land_draw_113.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_112 = {
#include "stg40/sky_land/final_chase_sky_land_draw_112.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_111 = {
#include "stg40/sky_land/final_chase_sky_land_draw_111.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_110 = {
#include "stg40/sky_land/final_chase_sky_land_draw_110.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_109 = {
#include "stg40/sky_land/final_chase_sky_land_draw_109.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_108 = {
#include "stg40/sky_land/final_chase_sky_land_draw_108.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_107 = {
#include "stg40/sky_land/final_chase_sky_land_draw_107.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_106 = {
#include "stg40/sky_land/final_chase_sky_land_draw_106.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_104 = {
#include "stg40/sky_land/final_chase_sky_land_draw_104.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_103 = {
#include "stg40/sky_land/final_chase_sky_land_draw_103.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_102 = {
#include "stg40/sky_land/final_chase_sky_land_draw_102.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_101 = {
#include "stg40/sky_land/final_chase_sky_land_draw_101.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_100 = {
#include "stg40/sky_land/final_chase_sky_land_draw_100.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_99 = {
#include "stg40/sky_land/final_chase_sky_land_draw_99.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_98 = {
#include "stg40/sky_land/final_chase_sky_land_draw_98.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_97 = {
#include "stg40/sky_land/final_chase_sky_land_draw_97.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_96 = {
#include "stg40/sky_land/final_chase_sky_land_draw_96.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_95 = {
#include "stg40/sky_land/final_chase_sky_land_draw_95.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_94 = {
#include "stg40/sky_land/final_chase_sky_land_draw_94.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_93 = {
#include "stg40/sky_land/final_chase_sky_land_draw_93.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_92 = {
#include "stg40/sky_land/final_chase_sky_land_draw_92.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_91 = {
#include "stg40/sky_land/final_chase_sky_land_draw_91.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_90 = {
#include "stg40/sky_land/final_chase_sky_land_draw_90.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_89 = {
#include "stg40/sky_land/final_chase_sky_land_draw_89.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_88 = {
#include "stg40/sky_land/final_chase_sky_land_draw_88.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_87 = {
#include "stg40/sky_land/final_chase_sky_land_draw_87.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_86 = {
#include "stg40/sky_land/final_chase_sky_land_draw_86.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_85 = {
#include "stg40/sky_land/final_chase_sky_land_draw_85.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_84 = {
#include "stg40/sky_land/final_chase_sky_land_draw_84.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_83 = {
#include "stg40/sky_land/final_chase_sky_land_draw_83.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_82 = {
#include "stg40/sky_land/final_chase_sky_land_draw_82.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_81 = {
#include "stg40/sky_land/final_chase_sky_land_draw_81.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_80 = {
#include "stg40/sky_land/final_chase_sky_land_draw_80.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_79 = {
#include "stg40/sky_land/final_chase_sky_land_draw_79.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_77 = {
#include "stg40/sky_land/final_chase_sky_land_draw_77.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_76 = {
#include "stg40/sky_land/final_chase_sky_land_draw_76.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_75 = {
#include "stg40/sky_land/final_chase_sky_land_draw_75.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_72 = {
#include "stg40/sky_land/final_chase_sky_land_draw_72.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_70 = {
#include "stg40/sky_land/final_chase_sky_land_draw_70.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_69 = {
#include "stg40/sky_land/final_chase_sky_land_draw_69.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_68 = {
#include "stg40/sky_land/final_chase_sky_land_draw_68.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_67 = {
#include "stg40/sky_land/final_chase_sky_land_draw_67.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_66 = {
#include "stg40/sky_land/final_chase_sky_land_draw_66.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_65 = {
#include "stg40/sky_land/final_chase_sky_land_draw_65.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_64 = {
#include "stg40/sky_land/final_chase_sky_land_draw_64.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_61 = {
#include "stg40/sky_land/final_chase_sky_land_draw_61.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_60 = {
#include "stg40/sky_land/final_chase_sky_land_draw_60.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_59 = {
#include "stg40/sky_land/final_chase_sky_land_draw_59.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_58 = {
#include "stg40/sky_land/final_chase_sky_land_draw_58.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_57 = {
#include "stg40/sky_land/final_chase_sky_land_draw_57.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_56 = {
#include "stg40/sky_land/final_chase_sky_land_draw_56.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_55 = {
#include "stg40/sky_land/final_chase_sky_land_draw_55.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_54 = {
#include "stg40/sky_land/final_chase_sky_land_draw_54.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_50 = {
#include "stg40/sky_land/final_chase_sky_land_draw_50.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_49 = {
#include "stg40/sky_land/final_chase_sky_land_draw_49.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_48 = {
#include "stg40/sky_land/final_chase_sky_land_draw_48.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_47 = {
#include "stg40/sky_land/final_chase_sky_land_draw_47.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_46 = {
#include "stg40/sky_land/final_chase_sky_land_draw_46.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_45 = {
#include "stg40/sky_land/final_chase_sky_land_draw_45.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_44 = {
#include "stg40/sky_land/final_chase_sky_land_draw_44.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_43 = {
#include "stg40/sky_land/final_chase_sky_land_draw_43.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_41 = {
#include "stg40/sky_land/final_chase_sky_land_draw_41.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_40 = {
#include "stg40/sky_land/final_chase_sky_land_draw_40.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_39 = {
#include "stg40/sky_land/final_chase_sky_land_draw_39.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_37 = {
#include "stg40/sky_land/final_chase_sky_land_draw_37.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_36 = {
#include "stg40/sky_land/final_chase_sky_land_draw_36.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_35 = {
#include "stg40/sky_land/final_chase_sky_land_draw_35.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_34 = {
#include "stg40/sky_land/final_chase_sky_land_draw_34.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_33 = {
#include "stg40/sky_land/final_chase_sky_land_draw_33.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_32 = {
#include "stg40/sky_land/final_chase_sky_land_draw_32.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_31 = {
#include "stg40/sky_land/final_chase_sky_land_draw_31.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_30 = {
#include "stg40/sky_land/final_chase_sky_land_draw_30.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_29 = {
#include "stg40/sky_land/final_chase_sky_land_draw_29.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_28 = {
#include "stg40/sky_land/final_chase_sky_land_draw_28.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_27 = {
#include "stg40/sky_land/final_chase_sky_land_draw_27.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_26 = {
#include "stg40/sky_land/final_chase_sky_land_draw_26.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_25 = {
#include "stg40/sky_land/final_chase_sky_land_draw_25.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_24 = {
#include "stg40/sky_land/final_chase_sky_land_draw_24.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_23 = {
#include "stg40/sky_land/final_chase_sky_land_draw_23.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_22 = {
#include "stg40/sky_land/final_chase_sky_land_draw_22.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_21 = {
#include "stg40/sky_land/final_chase_sky_land_draw_21.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_20 = {
#include "stg40/sky_land/final_chase_sky_land_draw_20.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_19 = {
#include "stg40/sky_land/final_chase_sky_land_draw_19.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_18 = {
#include "stg40/sky_land/final_chase_sky_land_draw_18.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_17 = {
#include "stg40/sky_land/final_chase_sky_land_draw_17.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_16 = {
#include "stg40/sky_land/final_chase_sky_land_draw_16.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_14 = {
#include "stg40/sky_land/final_chase_sky_land_draw_14.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_12 = {
#include "stg40/sky_land/final_chase_sky_land_draw_12.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_11 = {
#include "stg40/sky_land/final_chase_sky_land_draw_11.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_10 = {
#include "stg40/sky_land/final_chase_sky_land_draw_10.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_9 = {
#include "stg40/sky_land/final_chase_sky_land_draw_9.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_8 = {
#include "stg40/sky_land/final_chase_sky_land_draw_8.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_7 = {
#include "stg40/sky_land/final_chase_sky_land_draw_7.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_6 = {
#include "stg40/sky_land/final_chase_sky_land_draw_6.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_5 = {
#include "stg40/sky_land/final_chase_sky_land_draw_5.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_4 = {
#include "stg40/sky_land/final_chase_sky_land_draw_4.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_3 = {
#include "stg40/sky_land/final_chase_sky_land_draw_3.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_2 = {
#include "stg40/sky_land/final_chase_sky_land_draw_2.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_1 = {
#include "stg40/sky_land/final_chase_sky_land_draw_1.inc"
};
static NJS_CNK_OBJECT final_chase_sky_land_draw_0 = {
#include "stg40/sky_land/final_chase_sky_land_draw_0.inc"
};
