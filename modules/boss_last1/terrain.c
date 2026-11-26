#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last1/terrain.h"
#include "collision/world.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char biolizard_land_filenames[306] = {
#include "boss_last1/terrain/biolizard_land_filenames.inc"
};

/* BOSS_LAST1.PRS's land, texture lists, chunk models and Basic models, from
 * the disc. its D lies between breath.c and rails.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[35];
struct land_table biolizard_land;
static NJS_TEXNAME textures[18];
static NJS_TEXLIST terrain_textures;
static Sint16 lower_bridge_polygons[327];
static Sint32 lower_bridge_vertices[275];
static NJS_CNK_MODEL lower_bridge_model;
static NJS_CNK_OBJECT lower_bridge;
static Sint16 upper_columns_polygons[63];
static Sint32 upper_columns_vertices[67];
static NJS_CNK_MODEL upper_columns_model;
static NJS_CNK_OBJECT upper_columns;
static Sint16 upper_discs_polygons[745];
static Sint32 upper_discs_vertices[483];
static NJS_CNK_MODEL upper_discs_model;
static NJS_CNK_OBJECT upper_discs;
static Sint16 upper_vanes_polygons[677];
static Sint32 upper_vanes_vertices[507];
static NJS_CNK_MODEL upper_vanes_model;
static NJS_CNK_OBJECT upper_vanes;
static Sint16 hanging_crystal_polygons[117];
static Sint32 hanging_crystal_vertices[71];
static NJS_CNK_MODEL hanging_crystal_model;
static NJS_CNK_OBJECT hanging_crystal;
static Sint16 outer_pillars_polygons[511];
static Sint32 outer_pillars_vertices[475];
static NJS_CNK_MODEL outer_pillars_model;
static NJS_CNK_OBJECT outer_pillars;
static Sint16 chamber_walls_polygons[1183];
static Sint32 chamber_walls_vertices[607];
static NJS_CNK_MODEL chamber_walls_model;
static NJS_CNK_OBJECT chamber_walls;
static Sint16 panel_polygons[159];
static Sint32 panel_vertices[139];
static NJS_CNK_MODEL panel_model;
static NJS_CNK_OBJECT panel;
static Sint16 hanging_strand_01_polygons[213];
static Sint32 hanging_strand_01_vertices[179];
static NJS_CNK_MODEL hanging_strand_01_model;
static NJS_CNK_OBJECT hanging_strand_01;
static Sint16 hanging_strand_02_polygons[281];
static Sint32 hanging_strand_02_vertices[223];
static NJS_CNK_MODEL hanging_strand_02_model;
static NJS_CNK_OBJECT hanging_strand_02;
static Sint16 hanging_strand_03_polygons[281];
static Sint32 hanging_strand_03_vertices[223];
static NJS_CNK_MODEL hanging_strand_03_model;
static NJS_CNK_OBJECT hanging_strand_03;
static Sint16 hanging_strand_04_polygons[281];
static Sint32 hanging_strand_04_vertices[223];
static NJS_CNK_MODEL hanging_strand_04_model;
static NJS_CNK_OBJECT hanging_strand_04;
static Sint16 hanging_strand_05_polygons[281];
static Sint32 hanging_strand_05_vertices[223];
static NJS_CNK_MODEL hanging_strand_05_model;
static NJS_CNK_OBJECT hanging_strand_05;
static Sint16 hanging_strand_06_polygons[281];
static Sint32 hanging_strand_06_vertices[223];
static NJS_CNK_MODEL hanging_strand_06_model;
static NJS_CNK_OBJECT hanging_strand_06;
static Sint16 hanging_strand_07_polygons[281];
static Sint32 hanging_strand_07_vertices[223];
static NJS_CNK_MODEL hanging_strand_07_model;
static NJS_CNK_OBJECT hanging_strand_07;
static Sint16 hanging_strand_08_polygons[213];
static Sint32 hanging_strand_08_vertices[179];
static NJS_CNK_MODEL hanging_strand_08_model;
static NJS_CNK_OBJECT hanging_strand_08;
static Sint16 hanging_strand_09_polygons[261];
static Sint32 hanging_strand_09_vertices[207];
static NJS_CNK_MODEL hanging_strand_09_model;
static NJS_CNK_OBJECT hanging_strand_09;
static Sint16 hanging_strand_10_polygons[213];
static Sint32 hanging_strand_10_vertices[179];
static NJS_CNK_MODEL hanging_strand_10_model;
static NJS_CNK_OBJECT hanging_strand_10;
static Sint16 hanging_strand_11_polygons[213];
static Sint32 hanging_strand_11_vertices[179];
static NJS_CNK_MODEL hanging_strand_11_model;
static NJS_CNK_OBJECT hanging_strand_11;
static Sint16 hanging_strand_12_polygons[213];
static Sint32 hanging_strand_12_vertices[179];
static NJS_CNK_MODEL hanging_strand_12_model;
static NJS_CNK_OBJECT hanging_strand_12;
static Sint16 hanging_strand_13_polygons[213];
static Sint32 hanging_strand_13_vertices[179];
static NJS_CNK_MODEL hanging_strand_13_model;
static NJS_CNK_OBJECT hanging_strand_13;
static Sint16 hanging_strand_14_polygons[213];
static Sint32 hanging_strand_14_vertices[179];
static NJS_CNK_MODEL hanging_strand_14_model;
static NJS_CNK_OBJECT hanging_strand_14;
static Sint16 hanging_strand_15_polygons[213];
static Sint32 hanging_strand_15_vertices[179];
static NJS_CNK_MODEL hanging_strand_15_model;
static NJS_CNK_OBJECT hanging_strand_15;
static Sint16 hanging_strand_16_polygons[213];
static Sint32 hanging_strand_16_vertices[179];
static NJS_CNK_MODEL hanging_strand_16_model;
static NJS_CNK_OBJECT hanging_strand_16;
static Sint16 vegetation_polygons[287];
static Sint32 vegetation_vertices[239];
static NJS_CNK_MODEL vegetation_model;
static NJS_CNK_OBJECT vegetation;
static Sint16 biolizard_water_object_polygons[501];
static Sint32 biolizard_water_object_vertices[351];
static NJS_CNK_MODEL biolizard_water_object_model;
NJS_CNK_OBJECT biolizard_water_object;
static Sint16 platform_base_polygons[385];
static Sint32 platform_base_vertices[255];
static NJS_CNK_MODEL platform_base_model;
static NJS_CNK_OBJECT platform_base;
static Sint16 platform_posts_polygons[141];
static Sint32 platform_posts_vertices[135];
static NJS_CNK_MODEL platform_posts_model;
static NJS_CNK_OBJECT platform_posts;
static Sint16 platform_top_polygons[319];
static Sint32 platform_top_vertices[283];
static NJS_CNK_MODEL platform_top_model;
static NJS_CNK_OBJECT platform_top;
static Sint16 far_bridge_polygons[327];
static Sint32 far_bridge_vertices[259];
static NJS_CNK_MODEL far_bridge_model;
static NJS_CNK_OBJECT far_bridge;
static Sint16 bridge_ends_polygons[227];
static Sint32 bridge_ends_vertices[203];
static NJS_CNK_MODEL bridge_ends_model;
static NJS_CNK_OBJECT bridge_ends;
static Sint16 outer_ring_polygons[585];
static Sint32 outer_ring_vertices[451];
static NJS_CNK_MODEL outer_ring_model;
static NJS_CNK_OBJECT outer_ring;
static Sint16 inner_ring_polygons[991];
static Sint32 inner_ring_vertices[807];
static NJS_CNK_MODEL inner_ring_model;
static NJS_CNK_OBJECT inner_ring;
static NJS_MATERIAL solid_floor_materials[1];
static Sint16 solid_floor_faces[230];
static NJS_MESHSET solid_floor_meshsets[1];
static NJS_POINT3 solid_floor_points[98];
static NJS_MODEL solid_floor_model;
static NJS_OBJECT solid_floor;
static NJS_MATERIAL flat_floor_materials[1];
static Sint16 flat_floor_faces[45];
static NJS_MESHSET flat_floor_meshsets[1];
static NJS_POINT3 flat_floor_points[27];
static NJS_MODEL flat_floor_model;
static NJS_OBJECT flat_floor;

static struct land_entry entries[35] = {
#include "boss_last1/terrain/entries.inc"
};
struct land_table biolizard_land = {
#include "boss_last1/terrain/biolizard_land.inc"
};
static NJS_TEXNAME textures[18] = {
#include "boss_last1/terrain/textures.inc"
};
static NJS_TEXLIST terrain_textures = {
#include "boss_last1/terrain/terrain_textures.inc"
};
static Sint16 lower_bridge_polygons[327] = {
#include "boss_last1/terrain/lower_bridge_polygons.inc"
};
static Sint32 lower_bridge_vertices[275] = {
#include "boss_last1/terrain/lower_bridge_vertices.inc"
};
static NJS_CNK_MODEL lower_bridge_model = {
#include "boss_last1/terrain/lower_bridge_model.inc"
};
static NJS_CNK_OBJECT lower_bridge = {
#include "boss_last1/terrain/lower_bridge.inc"
};
static Sint16 upper_columns_polygons[63] = {
#include "boss_last1/terrain/upper_columns_polygons.inc"
};
static Sint32 upper_columns_vertices[67] = {
#include "boss_last1/terrain/upper_columns_vertices.inc"
};
static NJS_CNK_MODEL upper_columns_model = {
#include "boss_last1/terrain/upper_columns_model.inc"
};
static NJS_CNK_OBJECT upper_columns = {
#include "boss_last1/terrain/upper_columns.inc"
};
static Sint16 upper_discs_polygons[745] = {
#include "boss_last1/terrain/upper_discs_polygons.inc"
};
static Sint32 upper_discs_vertices[483] = {
#include "boss_last1/terrain/upper_discs_vertices.inc"
};
static NJS_CNK_MODEL upper_discs_model = {
#include "boss_last1/terrain/upper_discs_model.inc"
};
static NJS_CNK_OBJECT upper_discs = {
#include "boss_last1/terrain/upper_discs.inc"
};
static Sint16 upper_vanes_polygons[677] = {
#include "boss_last1/terrain/upper_vanes_polygons.inc"
};
static Sint32 upper_vanes_vertices[507] = {
#include "boss_last1/terrain/upper_vanes_vertices.inc"
};
static NJS_CNK_MODEL upper_vanes_model = {
#include "boss_last1/terrain/upper_vanes_model.inc"
};
static NJS_CNK_OBJECT upper_vanes = {
#include "boss_last1/terrain/upper_vanes.inc"
};
static Sint16 hanging_crystal_polygons[117] = {
#include "boss_last1/terrain/hanging_crystal_polygons.inc"
};
static Sint32 hanging_crystal_vertices[71] = {
#include "boss_last1/terrain/hanging_crystal_vertices.inc"
};
static NJS_CNK_MODEL hanging_crystal_model = {
#include "boss_last1/terrain/hanging_crystal_model.inc"
};
static NJS_CNK_OBJECT hanging_crystal = {
#include "boss_last1/terrain/hanging_crystal.inc"
};
static Sint16 outer_pillars_polygons[511] = {
#include "boss_last1/terrain/outer_pillars_polygons.inc"
};
static Sint32 outer_pillars_vertices[475] = {
#include "boss_last1/terrain/outer_pillars_vertices.inc"
};
static NJS_CNK_MODEL outer_pillars_model = {
#include "boss_last1/terrain/outer_pillars_model.inc"
};
static NJS_CNK_OBJECT outer_pillars = {
#include "boss_last1/terrain/outer_pillars.inc"
};
static Sint16 chamber_walls_polygons[1183] = {
#include "boss_last1/terrain/chamber_walls_polygons.inc"
};
static Sint32 chamber_walls_vertices[607] = {
#include "boss_last1/terrain/chamber_walls_vertices.inc"
};
static NJS_CNK_MODEL chamber_walls_model = {
#include "boss_last1/terrain/chamber_walls_model.inc"
};
static NJS_CNK_OBJECT chamber_walls = {
#include "boss_last1/terrain/chamber_walls.inc"
};
static Sint16 panel_polygons[159] = {
#include "boss_last1/terrain/panel_polygons.inc"
};
static Sint32 panel_vertices[139] = {
#include "boss_last1/terrain/panel_vertices.inc"
};
static NJS_CNK_MODEL panel_model = {
#include "boss_last1/terrain/panel_model.inc"
};
static NJS_CNK_OBJECT panel = {
#include "boss_last1/terrain/panel.inc"
};
static Sint16 hanging_strand_01_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_01_polygons.inc"
};
static Sint32 hanging_strand_01_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_01_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_01_model = {
#include "boss_last1/terrain/hanging_strand_01_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_01 = {
#include "boss_last1/terrain/hanging_strand_01.inc"
};
static Sint16 hanging_strand_02_polygons[281] = {
#include "boss_last1/terrain/hanging_strand_02_polygons.inc"
};
static Sint32 hanging_strand_02_vertices[223] = {
#include "boss_last1/terrain/hanging_strand_02_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_02_model = {
#include "boss_last1/terrain/hanging_strand_02_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_02 = {
#include "boss_last1/terrain/hanging_strand_02.inc"
};
static Sint16 hanging_strand_03_polygons[281] = {
#include "boss_last1/terrain/hanging_strand_03_polygons.inc"
};
static Sint32 hanging_strand_03_vertices[223] = {
#include "boss_last1/terrain/hanging_strand_03_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_03_model = {
#include "boss_last1/terrain/hanging_strand_03_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_03 = {
#include "boss_last1/terrain/hanging_strand_03.inc"
};
static Sint16 hanging_strand_04_polygons[281] = {
#include "boss_last1/terrain/hanging_strand_04_polygons.inc"
};
static Sint32 hanging_strand_04_vertices[223] = {
#include "boss_last1/terrain/hanging_strand_04_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_04_model = {
#include "boss_last1/terrain/hanging_strand_04_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_04 = {
#include "boss_last1/terrain/hanging_strand_04.inc"
};
static Sint16 hanging_strand_05_polygons[281] = {
#include "boss_last1/terrain/hanging_strand_05_polygons.inc"
};
static Sint32 hanging_strand_05_vertices[223] = {
#include "boss_last1/terrain/hanging_strand_05_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_05_model = {
#include "boss_last1/terrain/hanging_strand_05_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_05 = {
#include "boss_last1/terrain/hanging_strand_05.inc"
};
static Sint16 hanging_strand_06_polygons[281] = {
#include "boss_last1/terrain/hanging_strand_06_polygons.inc"
};
static Sint32 hanging_strand_06_vertices[223] = {
#include "boss_last1/terrain/hanging_strand_06_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_06_model = {
#include "boss_last1/terrain/hanging_strand_06_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_06 = {
#include "boss_last1/terrain/hanging_strand_06.inc"
};
static Sint16 hanging_strand_07_polygons[281] = {
#include "boss_last1/terrain/hanging_strand_07_polygons.inc"
};
static Sint32 hanging_strand_07_vertices[223] = {
#include "boss_last1/terrain/hanging_strand_07_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_07_model = {
#include "boss_last1/terrain/hanging_strand_07_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_07 = {
#include "boss_last1/terrain/hanging_strand_07.inc"
};
static Sint16 hanging_strand_08_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_08_polygons.inc"
};
static Sint32 hanging_strand_08_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_08_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_08_model = {
#include "boss_last1/terrain/hanging_strand_08_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_08 = {
#include "boss_last1/terrain/hanging_strand_08.inc"
};
static Sint16 hanging_strand_09_polygons[261] = {
#include "boss_last1/terrain/hanging_strand_09_polygons.inc"
};
static Sint32 hanging_strand_09_vertices[207] = {
#include "boss_last1/terrain/hanging_strand_09_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_09_model = {
#include "boss_last1/terrain/hanging_strand_09_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_09 = {
#include "boss_last1/terrain/hanging_strand_09.inc"
};
static Sint16 hanging_strand_10_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_10_polygons.inc"
};
static Sint32 hanging_strand_10_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_10_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_10_model = {
#include "boss_last1/terrain/hanging_strand_10_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_10 = {
#include "boss_last1/terrain/hanging_strand_10.inc"
};
static Sint16 hanging_strand_11_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_11_polygons.inc"
};
static Sint32 hanging_strand_11_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_11_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_11_model = {
#include "boss_last1/terrain/hanging_strand_11_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_11 = {
#include "boss_last1/terrain/hanging_strand_11.inc"
};
static Sint16 hanging_strand_12_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_12_polygons.inc"
};
static Sint32 hanging_strand_12_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_12_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_12_model = {
#include "boss_last1/terrain/hanging_strand_12_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_12 = {
#include "boss_last1/terrain/hanging_strand_12.inc"
};
static Sint16 hanging_strand_13_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_13_polygons.inc"
};
static Sint32 hanging_strand_13_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_13_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_13_model = {
#include "boss_last1/terrain/hanging_strand_13_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_13 = {
#include "boss_last1/terrain/hanging_strand_13.inc"
};
static Sint16 hanging_strand_14_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_14_polygons.inc"
};
static Sint32 hanging_strand_14_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_14_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_14_model = {
#include "boss_last1/terrain/hanging_strand_14_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_14 = {
#include "boss_last1/terrain/hanging_strand_14.inc"
};
static Sint16 hanging_strand_15_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_15_polygons.inc"
};
static Sint32 hanging_strand_15_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_15_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_15_model = {
#include "boss_last1/terrain/hanging_strand_15_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_15 = {
#include "boss_last1/terrain/hanging_strand_15.inc"
};
static Sint16 hanging_strand_16_polygons[213] = {
#include "boss_last1/terrain/hanging_strand_16_polygons.inc"
};
static Sint32 hanging_strand_16_vertices[179] = {
#include "boss_last1/terrain/hanging_strand_16_vertices.inc"
};
static NJS_CNK_MODEL hanging_strand_16_model = {
#include "boss_last1/terrain/hanging_strand_16_model.inc"
};
static NJS_CNK_OBJECT hanging_strand_16 = {
#include "boss_last1/terrain/hanging_strand_16.inc"
};
static Sint16 vegetation_polygons[287] = {
#include "boss_last1/terrain/vegetation_polygons.inc"
};
static Sint32 vegetation_vertices[239] = {
#include "boss_last1/terrain/vegetation_vertices.inc"
};
static NJS_CNK_MODEL vegetation_model = {
#include "boss_last1/terrain/vegetation_model.inc"
};
static NJS_CNK_OBJECT vegetation = {
#include "boss_last1/terrain/vegetation.inc"
};
static Sint16 biolizard_water_object_polygons[501] = {
#include "boss_last1/terrain/biolizard_water_object_polygons.inc"
};
static Sint32 biolizard_water_object_vertices[351] = {
#include "boss_last1/terrain/biolizard_water_object_vertices.inc"
};
static NJS_CNK_MODEL biolizard_water_object_model = {
#include "boss_last1/terrain/biolizard_water_object_model.inc"
};
NJS_CNK_OBJECT biolizard_water_object = {
#include "boss_last1/terrain/biolizard_water_object.inc"
};
static Sint16 platform_base_polygons[385] = {
#include "boss_last1/terrain/platform_base_polygons.inc"
};
static Sint32 platform_base_vertices[255] = {
#include "boss_last1/terrain/platform_base_vertices.inc"
};
static NJS_CNK_MODEL platform_base_model = {
#include "boss_last1/terrain/platform_base_model.inc"
};
static NJS_CNK_OBJECT platform_base = {
#include "boss_last1/terrain/platform_base.inc"
};
static Sint16 platform_posts_polygons[141] = {
#include "boss_last1/terrain/platform_posts_polygons.inc"
};
static Sint32 platform_posts_vertices[135] = {
#include "boss_last1/terrain/platform_posts_vertices.inc"
};
static NJS_CNK_MODEL platform_posts_model = {
#include "boss_last1/terrain/platform_posts_model.inc"
};
static NJS_CNK_OBJECT platform_posts = {
#include "boss_last1/terrain/platform_posts.inc"
};
static Sint16 platform_top_polygons[319] = {
#include "boss_last1/terrain/platform_top_polygons.inc"
};
static Sint32 platform_top_vertices[283] = {
#include "boss_last1/terrain/platform_top_vertices.inc"
};
static NJS_CNK_MODEL platform_top_model = {
#include "boss_last1/terrain/platform_top_model.inc"
};
static NJS_CNK_OBJECT platform_top = {
#include "boss_last1/terrain/platform_top.inc"
};
static Sint16 far_bridge_polygons[327] = {
#include "boss_last1/terrain/far_bridge_polygons.inc"
};
static Sint32 far_bridge_vertices[259] = {
#include "boss_last1/terrain/far_bridge_vertices.inc"
};
static NJS_CNK_MODEL far_bridge_model = {
#include "boss_last1/terrain/far_bridge_model.inc"
};
static NJS_CNK_OBJECT far_bridge = {
#include "boss_last1/terrain/far_bridge.inc"
};
static Sint16 bridge_ends_polygons[227] = {
#include "boss_last1/terrain/bridge_ends_polygons.inc"
};
static Sint32 bridge_ends_vertices[203] = {
#include "boss_last1/terrain/bridge_ends_vertices.inc"
};
static NJS_CNK_MODEL bridge_ends_model = {
#include "boss_last1/terrain/bridge_ends_model.inc"
};
static NJS_CNK_OBJECT bridge_ends = {
#include "boss_last1/terrain/bridge_ends.inc"
};
static Sint16 outer_ring_polygons[585] = {
#include "boss_last1/terrain/outer_ring_polygons.inc"
};
static Sint32 outer_ring_vertices[451] = {
#include "boss_last1/terrain/outer_ring_vertices.inc"
};
static NJS_CNK_MODEL outer_ring_model = {
#include "boss_last1/terrain/outer_ring_model.inc"
};
static NJS_CNK_OBJECT outer_ring = {
#include "boss_last1/terrain/outer_ring.inc"
};
static Sint16 inner_ring_polygons[991] = {
#include "boss_last1/terrain/inner_ring_polygons.inc"
};
static Sint32 inner_ring_vertices[807] = {
#include "boss_last1/terrain/inner_ring_vertices.inc"
};
static NJS_CNK_MODEL inner_ring_model = {
#include "boss_last1/terrain/inner_ring_model.inc"
};
static NJS_CNK_OBJECT inner_ring = {
#include "boss_last1/terrain/inner_ring.inc"
};
static NJS_MATERIAL solid_floor_materials[1] = {
#include "boss_last1/terrain/solid_floor_materials.inc"
};
static Sint16 solid_floor_faces[230] = {
#include "boss_last1/terrain/solid_floor_faces.inc"
};
static NJS_MESHSET solid_floor_meshsets[1] = {
#include "boss_last1/terrain/solid_floor_meshsets.inc"
};
static NJS_POINT3 solid_floor_points[98] = {
#include "boss_last1/terrain/solid_floor_points.inc"
};
static NJS_MODEL solid_floor_model = {
#include "boss_last1/terrain/solid_floor_model.inc"
};
static NJS_OBJECT solid_floor = {
#include "boss_last1/terrain/solid_floor.inc"
};
static NJS_MATERIAL flat_floor_materials[1] = {
#include "boss_last1/terrain/flat_floor_materials.inc"
};
static Sint16 flat_floor_faces[45] = {
#include "boss_last1/terrain/flat_floor_faces.inc"
};
static NJS_MESHSET flat_floor_meshsets[1] = {
#include "boss_last1/terrain/flat_floor_meshsets.inc"
};
static NJS_POINT3 flat_floor_points[27] = {
#include "boss_last1/terrain/flat_floor_points.inc"
};
static NJS_MODEL flat_floor_model = {
#include "boss_last1/terrain/flat_floor_model.inc"
};
static NJS_OBJECT flat_floor = {
#include "boss_last1/terrain/flat_floor.inc"
};
