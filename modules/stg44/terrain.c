#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg44/mad_space.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char mad_space_land_filenames[605] = {
#include "stg44/terrain/mad_space_land_filenames.inc"
};

/* STG44.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between mad_space.c and sky_surfaces.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[164];
struct land_table mad_space_land;
static NJS_TEXNAME textures[47];
NJS_TEXLIST mad_space_land_texlist;
static Sint16 draw_86_polygons[323];
static Sint32 draw_86_vertices[223];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_86;
static Sint16 draw_85_polygons[323];
static Sint32 draw_85_vertices[223];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_85;
static Sint16 draw_84_polygons[323];
static Sint32 draw_84_vertices[223];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_83_polygons[323];
static Sint32 draw_83_vertices[223];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_83;
static Sint16 draw_82_polygons[323];
static Sint32 draw_82_vertices[223];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_82;
static Sint16 draw_81_polygons[323];
static Sint32 draw_81_vertices[223];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_80_polygons[323];
static Sint32 draw_80_vertices[223];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_79_polygons[323];
static Sint32 draw_79_vertices[223];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_78_polygons[421];
static Sint32 draw_78_vertices[299];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_78;
static Sint16 draw_75_polygons[545];
static Sint32 draw_75_vertices[339];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_74_polygons[135];
static Sint32 draw_74_vertices[99];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_71_polygons[45];
static Sint32 draw_71_vertices[39];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_70_polygons[215];
static Sint32 draw_70_vertices[123];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_68_polygons[77];
static Sint32 draw_68_vertices[63];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_67_polygons[475];
static Sint32 draw_67_vertices[311];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[305];
static Sint32 draw_66_vertices[203];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_64_polygons[107];
static Sint32 draw_64_vertices[103];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_60_polygons[469];
static Sint32 draw_60_vertices[347];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_56_polygons[313];
static Sint32 draw_56_vertices[203];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_55_polygons[495];
static Sint32 draw_55_vertices[311];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_54_polygons[475];
static Sint32 draw_54_vertices[315];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_53_polygons[489];
static Sint32 draw_53_vertices[319];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_52_polygons[141];
static Sint32 draw_52_vertices[99];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[237];
static Sint32 draw_51_vertices[199];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[129];
static Sint32 draw_50_vertices[99];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_48_polygons[77];
static Sint32 draw_48_vertices[71];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_46_polygons[107];
static Sint32 draw_46_vertices[95];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_43_polygons[449];
static Sint32 draw_43_vertices[275];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_41_polygons[541];
static Sint32 draw_41_vertices[343];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_33_polygons[73];
static Sint32 draw_33_vertices[67];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_21_polygons[209];
static Sint32 draw_21_vertices[195];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_9_polygons[35];
static Sint32 draw_9_vertices[35];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_8_polygons[329];
static Sint32 draw_8_vertices[275];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_4_polygons[169];
static Sint32 draw_4_vertices[163];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_7;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[182];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[94];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static Sint16 draw_3_polygons[1783];
static Sint32 draw_3_vertices[1259];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[1789];
static Sint32 draw_2_vertices[1259];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[1789];
static Sint32 draw_1_vertices[1267];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[1065];
static Sint32 draw_0_vertices[1299];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[115];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[66];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[162];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[77];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[155];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[79];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[66];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[31];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[19];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[16];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[30];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[18];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[74];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[34];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[83];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[35];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[90];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[38];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[2];
static Sint16 collision_37_faces[125];
static Sint16 collision_37_faces_1[32];
static NJS_MESHSET collision_37_meshsets[2];
static NJS_POINT3 collision_37_points[60];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[92];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[52];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[78];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[40];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[15];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[14];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[15];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[12];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[84];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[32];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[26];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[18];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[90];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[40];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[117];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[46];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[32];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[15];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[238];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[85];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[7];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[6];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[30];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[21];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_75;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_11;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_74;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_76;

static struct land_entry entries[164] = {
#include "stg44/terrain/entries.inc"
};
struct land_table mad_space_land = {
#include "stg44/terrain/mad_space_land.inc"
};
static NJS_TEXNAME textures[47] = {
#include "stg44/terrain/textures.inc"
};
NJS_TEXLIST mad_space_land_texlist = {
#include "stg44/terrain/mad_space_land_texlist.inc"
};
static Sint16 draw_86_polygons[323] = {
#include "stg44/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[223] = {
#include "stg44/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg44/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg44/terrain/draw_86.inc"
};
static Sint16 draw_85_polygons[323] = {
#include "stg44/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[223] = {
#include "stg44/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg44/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg44/terrain/draw_85.inc"
};
static Sint16 draw_84_polygons[323] = {
#include "stg44/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[223] = {
#include "stg44/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg44/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg44/terrain/draw_84.inc"
};
static Sint16 draw_83_polygons[323] = {
#include "stg44/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[223] = {
#include "stg44/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg44/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg44/terrain/draw_83.inc"
};
static Sint16 draw_82_polygons[323] = {
#include "stg44/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[223] = {
#include "stg44/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg44/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg44/terrain/draw_82.inc"
};
static Sint16 draw_81_polygons[323] = {
#include "stg44/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[223] = {
#include "stg44/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg44/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg44/terrain/draw_81.inc"
};
static Sint16 draw_80_polygons[323] = {
#include "stg44/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[223] = {
#include "stg44/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg44/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg44/terrain/draw_80.inc"
};
static Sint16 draw_79_polygons[323] = {
#include "stg44/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[223] = {
#include "stg44/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg44/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg44/terrain/draw_79.inc"
};
static Sint16 draw_78_polygons[421] = {
#include "stg44/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[299] = {
#include "stg44/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg44/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg44/terrain/draw_78.inc"
};
static Sint16 draw_75_polygons[545] = {
#include "stg44/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[339] = {
#include "stg44/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg44/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg44/terrain/draw_77.inc"
};
static Sint16 draw_74_polygons[135] = {
#include "stg44/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[99] = {
#include "stg44/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg44/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg44/terrain/draw_74.inc"
};
static Sint16 draw_71_polygons[45] = {
#include "stg44/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[39] = {
#include "stg44/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg44/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg44/terrain/draw_73.inc"
};
static Sint16 draw_70_polygons[215] = {
#include "stg44/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[123] = {
#include "stg44/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg44/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg44/terrain/draw_70.inc"
};
static Sint16 draw_68_polygons[77] = {
#include "stg44/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[63] = {
#include "stg44/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg44/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg44/terrain/draw_69.inc"
};
static Sint16 draw_67_polygons[475] = {
#include "stg44/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[311] = {
#include "stg44/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg44/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg44/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[305] = {
#include "stg44/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[203] = {
#include "stg44/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg44/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg44/terrain/draw_66.inc"
};
static Sint16 draw_64_polygons[107] = {
#include "stg44/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[103] = {
#include "stg44/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg44/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg44/terrain/draw_65.inc"
};
static Sint16 draw_60_polygons[469] = {
#include "stg44/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[347] = {
#include "stg44/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg44/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg44/terrain/draw_63.inc"
};
static Sint16 draw_56_polygons[313] = {
#include "stg44/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[203] = {
#include "stg44/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg44/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg44/terrain/draw_59.inc"
};
static Sint16 draw_55_polygons[495] = {
#include "stg44/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[311] = {
#include "stg44/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg44/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg44/terrain/draw_55.inc"
};
static Sint16 draw_54_polygons[475] = {
#include "stg44/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[315] = {
#include "stg44/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg44/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg44/terrain/draw_54.inc"
};
static Sint16 draw_53_polygons[489] = {
#include "stg44/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[319] = {
#include "stg44/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg44/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg44/terrain/draw_53.inc"
};
static Sint16 draw_52_polygons[141] = {
#include "stg44/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[99] = {
#include "stg44/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg44/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg44/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[237] = {
#include "stg44/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[199] = {
#include "stg44/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg44/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg44/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[129] = {
#include "stg44/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[99] = {
#include "stg44/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg44/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg44/terrain/draw_50.inc"
};
static Sint16 draw_48_polygons[77] = {
#include "stg44/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[71] = {
#include "stg44/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg44/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg44/terrain/draw_49.inc"
};
static Sint16 draw_46_polygons[107] = {
#include "stg44/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[95] = {
#include "stg44/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg44/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg44/terrain/draw_47.inc"
};
static Sint16 draw_43_polygons[449] = {
#include "stg44/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[275] = {
#include "stg44/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg44/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg44/terrain/draw_45.inc"
};
static Sint16 draw_41_polygons[541] = {
#include "stg44/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[343] = {
#include "stg44/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg44/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg44/terrain/draw_42.inc"
};
static Sint16 draw_33_polygons[73] = {
#include "stg44/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[67] = {
#include "stg44/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg44/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg44/terrain/draw_40.inc"
};
static Sint16 draw_21_polygons[209] = {
#include "stg44/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[195] = {
#include "stg44/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg44/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg44/terrain/draw_32.inc"
};
static Sint16 draw_9_polygons[35] = {
#include "stg44/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[35] = {
#include "stg44/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg44/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg44/terrain/draw_20.inc"
};
static Sint16 draw_8_polygons[329] = {
#include "stg44/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[275] = {
#include "stg44/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg44/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg44/terrain/draw_8.inc"
};
static Sint16 draw_4_polygons[169] = {
#include "stg44/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[163] = {
#include "stg44/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg44/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg44/terrain/draw_7.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg44/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[182] = {
#include "stg44/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg44/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[94] = {
#include "stg44/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg44/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg44/terrain/collision_0.inc"
};
static Sint16 draw_3_polygons[1783] = {
#include "stg44/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[1259] = {
#include "stg44/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg44/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg44/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[1789] = {
#include "stg44/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[1259] = {
#include "stg44/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg44/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg44/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[1789] = {
#include "stg44/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[1267] = {
#include "stg44/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg44/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg44/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[1065] = {
#include "stg44/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[1299] = {
#include "stg44/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg44/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg44/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg44/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[115] = {
#include "stg44/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg44/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[66] = {
#include "stg44/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg44/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg44/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg44/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[162] = {
#include "stg44/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg44/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[77] = {
#include "stg44/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg44/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg44/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg44/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[155] = {
#include "stg44/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg44/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[79] = {
#include "stg44/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg44/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg44/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg44/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[66] = {
#include "stg44/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg44/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[31] = {
#include "stg44/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg44/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg44/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg44/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[19] = {
#include "stg44/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg44/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[16] = {
#include "stg44/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg44/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg44/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg44/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[30] = {
#include "stg44/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg44/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[18] = {
#include "stg44/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg44/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg44/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg44/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[74] = {
#include "stg44/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg44/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[34] = {
#include "stg44/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg44/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg44/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg44/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[83] = {
#include "stg44/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg44/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[35] = {
#include "stg44/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg44/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg44/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg44/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[90] = {
#include "stg44/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg44/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[38] = {
#include "stg44/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg44/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg44/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[2] = {
#include "stg44/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[125] = {
#include "stg44/terrain/collision_37_faces.inc"
};
static Sint16 collision_37_faces_1[32] = {
#include "stg44/terrain/collision_37_faces_1.inc"
};
static NJS_MESHSET collision_37_meshsets[2] = {
#include "stg44/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[60] = {
#include "stg44/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg44/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg44/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg44/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[92] = {
#include "stg44/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg44/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[52] = {
#include "stg44/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg44/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg44/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg44/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[78] = {
#include "stg44/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg44/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[40] = {
#include "stg44/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg44/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg44/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg44/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[15] = {
#include "stg44/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg44/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[14] = {
#include "stg44/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg44/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg44/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg44/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[15] = {
#include "stg44/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg44/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[12] = {
#include "stg44/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg44/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg44/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg44/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[84] = {
#include "stg44/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg44/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[32] = {
#include "stg44/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg44/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg44/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg44/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[26] = {
#include "stg44/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg44/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[18] = {
#include "stg44/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg44/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg44/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg44/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[90] = {
#include "stg44/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg44/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[40] = {
#include "stg44/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg44/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg44/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg44/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[117] = {
#include "stg44/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg44/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[46] = {
#include "stg44/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg44/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg44/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg44/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[32] = {
#include "stg44/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg44/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[15] = {
#include "stg44/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg44/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg44/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg44/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[238] = {
#include "stg44/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg44/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[85] = {
#include "stg44/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg44/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg44/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg44/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[7] = {
#include "stg44/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg44/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[6] = {
#include "stg44/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg44/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg44/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg44/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[30] = {
#include "stg44/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg44/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[21] = {
#include "stg44/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg44/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg44/terrain/collision_73.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg44/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg44/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg44/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg44/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg44/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg44/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg44/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg44/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg44/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg44/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg44/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg44/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg44/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg44/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg44/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg44/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg44/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg44/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg44/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg44/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg44/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg44/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg44/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg44/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg44/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg44/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg44/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg44/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg44/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg44/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg44/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg44/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg44/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg44/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg44/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg44/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg44/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg44/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg44/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg44/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg44/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg44/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg44/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg44/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg44/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg44/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg44/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg44/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg44/terrain/draw_4.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg44/terrain/collision_1.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg44/terrain/collision_2.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg44/terrain/collision_4.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg44/terrain/collision_5.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg44/terrain/collision_6.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg44/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg44/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg44/terrain/collision_10.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg44/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg44/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg44/terrain/collision_14.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg44/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg44/terrain/collision_17.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg44/terrain/collision_18.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg44/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg44/terrain/collision_21.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg44/terrain/collision_23.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg44/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg44/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg44/terrain/collision_27.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg44/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg44/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg44/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg44/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg44/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg44/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg44/terrain/collision_35.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg44/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg44/terrain/collision_39.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg44/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg44/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg44/terrain/collision_44.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg44/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg44/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg44/terrain/collision_48.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg44/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg44/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg44/terrain/collision_54.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg44/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg44/terrain/collision_57.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg44/terrain/collision_59.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg44/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg44/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg44/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg44/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg44/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg44/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg44/terrain/collision_67.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg44/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg44/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg44/terrain/collision_72.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg44/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg44/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg44/terrain/collision_76.inc"
};
