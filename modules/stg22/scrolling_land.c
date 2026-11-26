#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg22/background_textures.h"
#include "stg22/surfaces.h"

/* the file names that gadget_scrolling_land and its texture list point at. each
 * starts on a 4-byte boundary */
const char gadget_scrolling_land_filenames[72] = {
#include "stg22/scrolling_land/gadget_scrolling_land_filenames.inc"
};

/* STG22.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between object_kinds.c and switch_textures.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry gadget_scrolling_land_entries[192];
struct land_table gadget_scrolling_land;
static NJS_TEXNAME gadget_scrolling_land_textures[5];
NJS_TEXLIST gadget_surface_texlist;
static Sint16 gadget_scrolling_land_draw_1_polygons[31];
static Sint32 gadget_scrolling_land_draw_1_vertices[27];
static NJS_CNK_MODEL gadget_scrolling_land_draw_1_model;
NJS_CNK_OBJECT gadget_light_strip_191;
static Sint16 gadget_scrolling_land_draw_37_polygons[53];
static Sint32 gadget_scrolling_land_draw_37_vertices[23];
static NJS_CNK_MODEL gadget_scrolling_land_draw_37_model;
NJS_CNK_OBJECT gadget_light_strip_190;
static Sint16 gadget_scrolling_land_draw_22_polygons[53];
static Sint32 gadget_scrolling_land_draw_22_vertices[23];
static NJS_CNK_MODEL gadget_scrolling_land_draw_22_model;
NJS_CNK_OBJECT gadget_light_strip_187;
static Sint16 gadget_scrolling_land_draw_34_polygons[31];
static Sint32 gadget_scrolling_land_draw_34_vertices[27];
static NJS_CNK_MODEL gadget_scrolling_land_draw_34_model;
NJS_CNK_OBJECT gadget_light_strip_180;
static Sint16 gadget_scrolling_land_draw_35_polygons[31];
static Sint32 gadget_scrolling_land_draw_35_vertices[27];
static NJS_CNK_MODEL gadget_scrolling_land_draw_35_model;
NJS_CNK_OBJECT gadget_light_strip_179;
static Sint16 gadget_scrolling_land_draw_0_polygons[65];
static Sint32 gadget_scrolling_land_draw_0_vertices[35];
static NJS_CNK_MODEL gadget_scrolling_land_draw_0_model;
NJS_CNK_OBJECT gadget_light_strip_178;
static Sint16 gadget_scrolling_land_draw_86_polygons[31];
static Sint32 gadget_scrolling_land_draw_86_vertices[27];
static NJS_CNK_MODEL gadget_scrolling_land_draw_86_model;
NJS_CNK_OBJECT gadget_light_strip_177;
static Sint16 gadget_scrolling_land_draw_102_polygons[31];
static Sint32 gadget_scrolling_land_draw_102_vertices[27];
static NJS_CNK_MODEL gadget_scrolling_land_draw_102_model;
NJS_CNK_OBJECT gadget_light_strip_169;
static Sint16 gadget_scrolling_land_draw_28_polygons[71];
static Sint32 gadget_scrolling_land_draw_28_vertices[35];
static NJS_CNK_MODEL gadget_scrolling_land_draw_28_model;
NJS_CNK_OBJECT gadget_light_strip_160;
static Sint16 gadget_scrolling_land_draw_29_polygons[77];
static Sint32 gadget_scrolling_land_draw_29_vertices[51];
static NJS_CNK_MODEL gadget_scrolling_land_draw_29_model;
NJS_CNK_OBJECT gadget_light_strip_158;
static Sint16 gadget_scrolling_land_draw_26_polygons[65];
static Sint32 gadget_scrolling_land_draw_26_vertices[35];
static NJS_CNK_MODEL gadget_scrolling_land_draw_26_model;
NJS_CNK_OBJECT gadget_light_strip_157;
static Sint16 gadget_scrolling_land_draw_150_polygons[49];
static Sint32 gadget_scrolling_land_draw_150_vertices[51];
static NJS_CNK_MODEL gadget_scrolling_land_draw_150_model;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_151;
static Sint16 gadget_scrolling_land_draw_82_polygons[25];
static Sint32 gadget_scrolling_land_draw_82_vertices[19];
static NJS_CNK_MODEL gadget_scrolling_land_draw_82_model;
NJS_CNK_OBJECT gadget_light_strip_149;
static Sint16 gadget_scrolling_land_draw_91_polygons[49];
static Sint32 gadget_scrolling_land_draw_91_vertices[51];
static NJS_CNK_MODEL gadget_scrolling_land_draw_91_model;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_92;
static Sint16 gadget_scrolling_land_draw_49_polygons[147];
static Sint32 gadget_scrolling_land_draw_49_vertices[91];
static NJS_CNK_MODEL gadget_scrolling_land_draw_49_model;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_49;
static Sint16 gadget_scrolling_land_draw_21_polygons[65];
static Sint32 gadget_scrolling_land_draw_21_vertices[35];
static NJS_CNK_MODEL gadget_scrolling_land_draw_21_model;
NJS_CNK_OBJECT gadget_light_strip_031;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_189;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_188;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_186;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_185;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_184;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_183;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_182;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_181;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_176;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_175;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_174;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_173;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_172;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_171;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_170;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_168;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_167;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_166;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_165;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_164;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_163;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_162;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_161;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_159;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_156;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_155;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_154;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_153;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_152;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_150;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_148;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_147;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_146;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_145;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_144;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_143;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_142;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_141;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_140;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_139;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_138;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_137;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_136;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_135;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_134;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_133;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_132;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_131;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_130;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_129;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_128;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_127;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_126;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_125;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_124;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_123;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_122;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_121;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_120;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_119;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_118;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_117;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_116;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_115;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_114;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_113;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_112;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_111;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_110;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_109;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_108;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_107;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_106;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_105;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_104;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_103;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_102;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_101;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_100;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_99;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_98;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_97;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_96;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_95;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_94;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_93;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_91;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_90;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_89;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_88;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_87;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_86;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_85;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_84;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_83;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_82;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_81;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_80;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_79;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_78;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_77;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_76;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_75;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_74;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_73;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_72;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_71;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_70;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_69;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_68;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_67;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_66;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_65;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_64;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_63;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_62;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_61;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_60;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_59;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_58;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_57;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_56;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_55;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_54;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_53;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_52;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_51;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_50;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_48;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_47;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_46;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_45;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_44;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_43;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_42;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_41;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_40;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_39;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_38;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_37;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_36;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_35;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_34;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_33;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_32;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_30;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_29;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_28;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_27;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_26;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_25;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_24;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_23;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_22;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_21;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_20;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_19;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_18;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_17;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_16;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_15;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_14;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_13;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_12;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_11;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_10;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_9;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_8;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_7;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_6;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_5;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_4;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_3;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_2;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_1;
static NJS_CNK_OBJECT gadget_scrolling_land_draw_0;

static struct land_entry gadget_scrolling_land_entries[192] = {
#include "stg22/scrolling_land/gadget_scrolling_land_entries.inc"
};
struct land_table gadget_scrolling_land = {
#include "stg22/scrolling_land/gadget_scrolling_land.inc"
};
static NJS_TEXNAME gadget_scrolling_land_textures[5] = {
#include "stg22/scrolling_land/gadget_scrolling_land_textures.inc"
};
NJS_TEXLIST gadget_surface_texlist = {
#include "stg22/scrolling_land/gadget_surface_texlist.inc"
};
static Sint16 gadget_scrolling_land_draw_1_polygons[31] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_1_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_1_vertices[27] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_1_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_1_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_191 = {
#include "stg22/scrolling_land/gadget_light_strip_191.inc"
};
static Sint16 gadget_scrolling_land_draw_37_polygons[53] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_37_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_37_vertices[23] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_37_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_37_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_37_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_190 = {
#include "stg22/scrolling_land/gadget_light_strip_190.inc"
};
static Sint16 gadget_scrolling_land_draw_22_polygons[53] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_22_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_22_vertices[23] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_22_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_22_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_22_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_187 = {
#include "stg22/scrolling_land/gadget_light_strip_187.inc"
};
static Sint16 gadget_scrolling_land_draw_34_polygons[31] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_34_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_34_vertices[27] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_34_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_34_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_34_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_180 = {
#include "stg22/scrolling_land/gadget_light_strip_180.inc"
};
static Sint16 gadget_scrolling_land_draw_35_polygons[31] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_35_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_35_vertices[27] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_35_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_35_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_35_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_179 = {
#include "stg22/scrolling_land/gadget_light_strip_179.inc"
};
static Sint16 gadget_scrolling_land_draw_0_polygons[65] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_0_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_0_vertices[35] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_0_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_0_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_178 = {
#include "stg22/scrolling_land/gadget_light_strip_178.inc"
};
static Sint16 gadget_scrolling_land_draw_86_polygons[31] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_86_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_86_vertices[27] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_86_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_86_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_86_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_177 = {
#include "stg22/scrolling_land/gadget_light_strip_177.inc"
};
static Sint16 gadget_scrolling_land_draw_102_polygons[31] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_102_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_102_vertices[27] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_102_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_102_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_102_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_169 = {
#include "stg22/scrolling_land/gadget_light_strip_169.inc"
};
static Sint16 gadget_scrolling_land_draw_28_polygons[71] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_28_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_28_vertices[35] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_28_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_28_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_28_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_160 = {
#include "stg22/scrolling_land/gadget_light_strip_160.inc"
};
static Sint16 gadget_scrolling_land_draw_29_polygons[77] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_29_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_29_vertices[51] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_29_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_29_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_29_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_158 = {
#include "stg22/scrolling_land/gadget_light_strip_158.inc"
};
static Sint16 gadget_scrolling_land_draw_26_polygons[65] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_26_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_26_vertices[35] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_26_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_26_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_26_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_157 = {
#include "stg22/scrolling_land/gadget_light_strip_157.inc"
};
static Sint16 gadget_scrolling_land_draw_150_polygons[49] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_150_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_150_vertices[51] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_150_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_150_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_150_model.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_151 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_151.inc"
};
static Sint16 gadget_scrolling_land_draw_82_polygons[25] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_82_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_82_vertices[19] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_82_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_82_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_82_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_149 = {
#include "stg22/scrolling_land/gadget_light_strip_149.inc"
};
static Sint16 gadget_scrolling_land_draw_91_polygons[49] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_91_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_91_vertices[51] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_91_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_91_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_91_model.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_92 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_92.inc"
};
static Sint16 gadget_scrolling_land_draw_49_polygons[147] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_49_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_49_vertices[91] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_49_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_49_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_49_model.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_49 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_49.inc"
};
static Sint16 gadget_scrolling_land_draw_21_polygons[65] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_21_polygons.inc"
};
static Sint32 gadget_scrolling_land_draw_21_vertices[35] = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_21_vertices.inc"
};
static NJS_CNK_MODEL gadget_scrolling_land_draw_21_model = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_21_model.inc"
};
NJS_CNK_OBJECT gadget_light_strip_031 = {
#include "stg22/scrolling_land/gadget_light_strip_031.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_189 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_189.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_188 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_188.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_186 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_186.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_185 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_185.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_184 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_184.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_183 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_183.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_182 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_182.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_181 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_181.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_176 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_176.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_175 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_175.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_174 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_174.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_173 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_173.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_172 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_172.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_171 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_171.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_170 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_170.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_168 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_168.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_167 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_167.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_166 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_166.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_165 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_165.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_164 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_164.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_163 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_163.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_162 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_162.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_161 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_161.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_159 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_159.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_156 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_156.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_155 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_155.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_154 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_154.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_153 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_153.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_152 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_152.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_150 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_150.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_148 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_148.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_147 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_147.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_146 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_146.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_145 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_145.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_144 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_144.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_143 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_143.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_142 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_142.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_141 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_141.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_140 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_140.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_139 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_139.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_138 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_138.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_137 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_137.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_136 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_136.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_135 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_135.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_134 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_134.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_133 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_133.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_132 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_132.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_131 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_131.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_130 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_130.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_129 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_129.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_128 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_128.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_127 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_127.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_126 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_126.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_125 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_125.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_124 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_124.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_123 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_123.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_122 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_122.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_121 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_121.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_120 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_120.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_119 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_119.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_118 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_118.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_117 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_117.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_116 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_116.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_115 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_115.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_114 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_114.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_113 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_113.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_112 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_112.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_111 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_111.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_110 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_110.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_109 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_109.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_108 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_108.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_107 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_107.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_106 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_106.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_105 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_105.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_104 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_104.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_103 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_103.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_102 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_102.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_101 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_101.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_100 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_100.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_99 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_99.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_98 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_98.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_97 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_97.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_96 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_96.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_95 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_95.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_94 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_94.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_93 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_93.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_91 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_91.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_90 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_90.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_89 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_89.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_88 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_88.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_87 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_87.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_86 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_86.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_85 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_85.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_84 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_84.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_83 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_83.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_82 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_82.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_81 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_81.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_80 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_80.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_79 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_79.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_78 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_78.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_77 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_77.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_76 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_76.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_75 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_75.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_74 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_74.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_73 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_73.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_72 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_72.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_71 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_71.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_70 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_70.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_69 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_69.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_68 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_68.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_67 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_67.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_66 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_66.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_65 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_65.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_64 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_64.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_63 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_63.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_62 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_62.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_61 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_61.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_60 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_60.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_59 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_59.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_58 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_58.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_57 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_57.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_56 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_56.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_55 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_55.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_54 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_54.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_53 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_53.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_52 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_52.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_51 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_51.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_50 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_50.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_48 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_48.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_47 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_47.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_46 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_46.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_45 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_45.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_44 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_44.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_43 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_43.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_42 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_42.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_41 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_41.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_40 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_40.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_39 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_39.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_38 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_38.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_37 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_37.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_36 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_36.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_35 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_35.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_34 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_34.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_33 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_33.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_32 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_32.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_30 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_30.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_29 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_29.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_28 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_28.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_27 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_27.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_26 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_26.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_25 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_25.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_24 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_24.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_23 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_23.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_22 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_22.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_21 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_21.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_20 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_20.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_19 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_19.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_18 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_18.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_17 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_17.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_16 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_16.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_15 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_15.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_14 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_14.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_13 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_13.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_12 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_12.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_11 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_11.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_10 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_10.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_9 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_9.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_8 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_8.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_7 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_7.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_6 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_6.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_5 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_5.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_4 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_4.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_3 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_3.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_2 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_2.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_1 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_1.inc"
};
static NJS_CNK_OBJECT gadget_scrolling_land_draw_0 = {
#include "stg22/scrolling_land/gadget_scrolling_land_draw_0.inc"
};
