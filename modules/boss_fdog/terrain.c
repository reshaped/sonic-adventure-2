/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_fdog/terrain.h"
#include "collision/world.h"

const char fdog_land_filenames[168] = {
#include "boss_fdog/terrain/fdog_land_filenames.inc"
};

/* BOSS_FDOG.PRS's land, texture lists, chunk models and Basic models, from
 * the disc. its D lies between stage.c and fdog.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[27];
struct land_table fdog_land;
static NJS_TEXNAME textures[17];
static NJS_TEXLIST terrain_textures;
static Sint16 north_wall_end_1_polygons[605];
static Sint32 north_wall_end_1_vertices[643];
static NJS_CNK_MODEL north_wall_end_1_model;
static NJS_CNK_OBJECT north_wall_end_2;
static Sint16 wall_panel_1_polygons[435];
static Sint32 wall_panel_1_vertices[499];
static NJS_CNK_MODEL wall_panel_1_model;
static NJS_CNK_OBJECT wall_panel_2;
static Sint16 south_wall_end_1_polygons[605];
static Sint32 south_wall_end_1_vertices[643];
static NJS_CNK_MODEL south_wall_end_1_model;
static NJS_CNK_OBJECT south_wall_end_1;
static NJS_MATERIAL wall_collision_1_materials[1];
static Sint16 wall_collision_1_faces[18];
static NJS_MESHSET wall_collision_1_meshsets[1];
static NJS_POINT3 wall_collision_1_points[12];
static NJS_MODEL wall_collision_1_model;
static NJS_OBJECT wall_collision_1;
static NJS_MATERIAL floor_collision_materials[1];
static Sint16 floor_collision_faces[44];
static NJS_MESHSET floor_collision_meshsets[1];
static NJS_POINT3 floor_collision_points[25];
static NJS_MODEL floor_collision_model;
static NJS_OBJECT floor_collision;
static Sint16 hangar_polygons[397];
static Sint32 hangar_vertices[239];
static NJS_CNK_MODEL hangar_model;
static NJS_CNK_OBJECT hangar;
static Sint16 wall_base_polygons[357];
static Sint32 wall_base_vertices[387];
static NJS_CNK_MODEL wall_base_model;
static NJS_CNK_OBJECT wall_base;
static Sint16 platform_polygons[803];
static Sint32 platform_vertices[775];
static NJS_CNK_MODEL platform_model;
static NJS_CNK_OBJECT platform;
static Sint16 deck_panel_1_polygons[117];
static Sint32 deck_panel_1_vertices[131];
static NJS_CNK_MODEL deck_panel_1_model;
static NJS_CNK_OBJECT deck_panel_4;
static Sint16 deck_edge_1_polygons[161];
static Sint32 deck_edge_1_vertices[179];
static NJS_CNK_MODEL deck_edge_1_model;
static NJS_CNK_OBJECT deck_edge_10;
static Sint16 lower_floor_polygons[77];
static Sint32 lower_floor_vertices[51];
static NJS_CNK_MODEL lower_floor_model;
static NJS_CNK_OBJECT lower_floor;
static NJS_CNK_OBJECT north_wall_end_1;
static NJS_CNK_OBJECT wall_panel_1;
static NJS_CNK_OBJECT south_wall_end_2;
static NJS_OBJECT wall_collision_2;
static NJS_CNK_OBJECT deck_panel_1;
static NJS_CNK_OBJECT deck_panel_2;
static NJS_CNK_OBJECT deck_panel_3;
static NJS_CNK_OBJECT deck_edge_1;
static NJS_CNK_OBJECT deck_edge_2;
static NJS_CNK_OBJECT deck_edge_3;
static NJS_CNK_OBJECT deck_edge_4;
static NJS_CNK_OBJECT deck_edge_5;
static NJS_CNK_OBJECT deck_edge_6;
static NJS_CNK_OBJECT deck_edge_7;
static NJS_CNK_OBJECT deck_edge_8;
static NJS_CNK_OBJECT deck_edge_9;

static struct land_entry entries[27] = {
#include "boss_fdog/terrain/entries.inc"
};
struct land_table fdog_land = {
#include "boss_fdog/terrain/fdog_land.inc"
};
static NJS_TEXNAME textures[17] = {
#include "boss_fdog/terrain/textures.inc"
};
static NJS_TEXLIST terrain_textures = {
#include "boss_fdog/terrain/terrain_textures.inc"
};
static Sint16 north_wall_end_1_polygons[605] = {
#include "boss_fdog/terrain/north_wall_end_1_polygons.inc"
};
static Sint32 north_wall_end_1_vertices[643] = {
#include "boss_fdog/terrain/north_wall_end_1_vertices.inc"
};
static NJS_CNK_MODEL north_wall_end_1_model = {
#include "boss_fdog/terrain/north_wall_end_1_model.inc"
};
static NJS_CNK_OBJECT north_wall_end_2 = {
#include "boss_fdog/terrain/north_wall_end_2.inc"
};
static Sint16 wall_panel_1_polygons[435] = {
#include "boss_fdog/terrain/wall_panel_1_polygons.inc"
};
static Sint32 wall_panel_1_vertices[499] = {
#include "boss_fdog/terrain/wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL wall_panel_1_model = {
#include "boss_fdog/terrain/wall_panel_1_model.inc"
};
static NJS_CNK_OBJECT wall_panel_2 = {
#include "boss_fdog/terrain/wall_panel_2.inc"
};
static Sint16 south_wall_end_1_polygons[605] = {
#include "boss_fdog/terrain/south_wall_end_1_polygons.inc"
};
static Sint32 south_wall_end_1_vertices[643] = {
#include "boss_fdog/terrain/south_wall_end_1_vertices.inc"
};
static NJS_CNK_MODEL south_wall_end_1_model = {
#include "boss_fdog/terrain/south_wall_end_1_model.inc"
};
static NJS_CNK_OBJECT south_wall_end_1 = {
#include "boss_fdog/terrain/south_wall_end_1.inc"
};
static NJS_MATERIAL wall_collision_1_materials[1] = {
#include "boss_fdog/terrain/wall_collision_1_materials.inc"
};
static Sint16 wall_collision_1_faces[18] = {
#include "boss_fdog/terrain/wall_collision_1_faces.inc"
};
static NJS_MESHSET wall_collision_1_meshsets[1] = {
#include "boss_fdog/terrain/wall_collision_1_meshsets.inc"
};
static NJS_POINT3 wall_collision_1_points[12] = {
#include "boss_fdog/terrain/wall_collision_1_points.inc"
};
static NJS_MODEL wall_collision_1_model = {
#include "boss_fdog/terrain/wall_collision_1_model.inc"
};
static NJS_OBJECT wall_collision_1 = {
#include "boss_fdog/terrain/wall_collision_1.inc"
};
static NJS_MATERIAL floor_collision_materials[1] = {
#include "boss_fdog/terrain/floor_collision_materials.inc"
};
static Sint16 floor_collision_faces[44] = {
#include "boss_fdog/terrain/floor_collision_faces.inc"
};
static NJS_MESHSET floor_collision_meshsets[1] = {
#include "boss_fdog/terrain/floor_collision_meshsets.inc"
};
static NJS_POINT3 floor_collision_points[25] = {
#include "boss_fdog/terrain/floor_collision_points.inc"
};
static NJS_MODEL floor_collision_model = {
#include "boss_fdog/terrain/floor_collision_model.inc"
};
static NJS_OBJECT floor_collision = {
#include "boss_fdog/terrain/floor_collision.inc"
};
static Sint16 hangar_polygons[397] = {
#include "boss_fdog/terrain/hangar_polygons.inc"
};
static Sint32 hangar_vertices[239] = {
#include "boss_fdog/terrain/hangar_vertices.inc"
};
static NJS_CNK_MODEL hangar_model = {
#include "boss_fdog/terrain/hangar_model.inc"
};
static NJS_CNK_OBJECT hangar = {
#include "boss_fdog/terrain/hangar.inc"
};
static Sint16 wall_base_polygons[357] = {
#include "boss_fdog/terrain/wall_base_polygons.inc"
};
static Sint32 wall_base_vertices[387] = {
#include "boss_fdog/terrain/wall_base_vertices.inc"
};
static NJS_CNK_MODEL wall_base_model = {
#include "boss_fdog/terrain/wall_base_model.inc"
};
static NJS_CNK_OBJECT wall_base = {
#include "boss_fdog/terrain/wall_base.inc"
};
static Sint16 platform_polygons[803] = {
#include "boss_fdog/terrain/platform_polygons.inc"
};
static Sint32 platform_vertices[775] = {
#include "boss_fdog/terrain/platform_vertices.inc"
};
static NJS_CNK_MODEL platform_model = {
#include "boss_fdog/terrain/platform_model.inc"
};
static NJS_CNK_OBJECT platform = {
#include "boss_fdog/terrain/platform.inc"
};
static Sint16 deck_panel_1_polygons[117] = {
#include "boss_fdog/terrain/deck_panel_1_polygons.inc"
};
static Sint32 deck_panel_1_vertices[131] = {
#include "boss_fdog/terrain/deck_panel_1_vertices.inc"
};
static NJS_CNK_MODEL deck_panel_1_model = {
#include "boss_fdog/terrain/deck_panel_1_model.inc"
};
static NJS_CNK_OBJECT deck_panel_4 = {
#include "boss_fdog/terrain/deck_panel_4.inc"
};
static Sint16 deck_edge_1_polygons[161] = {
#include "boss_fdog/terrain/deck_edge_1_polygons.inc"
};
static Sint32 deck_edge_1_vertices[179] = {
#include "boss_fdog/terrain/deck_edge_1_vertices.inc"
};
static NJS_CNK_MODEL deck_edge_1_model = {
#include "boss_fdog/terrain/deck_edge_1_model.inc"
};
static NJS_CNK_OBJECT deck_edge_10 = {
#include "boss_fdog/terrain/deck_edge_10.inc"
};
static Sint16 lower_floor_polygons[77] = {
#include "boss_fdog/terrain/lower_floor_polygons.inc"
};
static Sint32 lower_floor_vertices[51] = {
#include "boss_fdog/terrain/lower_floor_vertices.inc"
};
static NJS_CNK_MODEL lower_floor_model = {
#include "boss_fdog/terrain/lower_floor_model.inc"
};
static NJS_CNK_OBJECT lower_floor = {
#include "boss_fdog/terrain/lower_floor.inc"
};
static NJS_CNK_OBJECT north_wall_end_1 = {
#include "boss_fdog/terrain/north_wall_end_1.inc"
};
static NJS_CNK_OBJECT wall_panel_1 = {
#include "boss_fdog/terrain/wall_panel_1.inc"
};
static NJS_CNK_OBJECT south_wall_end_2 = {
#include "boss_fdog/terrain/south_wall_end_2.inc"
};
static NJS_OBJECT wall_collision_2 = {
#include "boss_fdog/terrain/wall_collision_2.inc"
};
static NJS_CNK_OBJECT deck_panel_1 = {
#include "boss_fdog/terrain/deck_panel_1.inc"
};
static NJS_CNK_OBJECT deck_panel_2 = {
#include "boss_fdog/terrain/deck_panel_2.inc"
};
static NJS_CNK_OBJECT deck_panel_3 = {
#include "boss_fdog/terrain/deck_panel_3.inc"
};
static NJS_CNK_OBJECT deck_edge_1 = {
#include "boss_fdog/terrain/deck_edge_1.inc"
};
static NJS_CNK_OBJECT deck_edge_2 = {
#include "boss_fdog/terrain/deck_edge_2.inc"
};
static NJS_CNK_OBJECT deck_edge_3 = {
#include "boss_fdog/terrain/deck_edge_3.inc"
};
static NJS_CNK_OBJECT deck_edge_4 = {
#include "boss_fdog/terrain/deck_edge_4.inc"
};
static NJS_CNK_OBJECT deck_edge_5 = {
#include "boss_fdog/terrain/deck_edge_5.inc"
};
static NJS_CNK_OBJECT deck_edge_6 = {
#include "boss_fdog/terrain/deck_edge_6.inc"
};
static NJS_CNK_OBJECT deck_edge_7 = {
#include "boss_fdog/terrain/deck_edge_7.inc"
};
static NJS_CNK_OBJECT deck_edge_8 = {
#include "boss_fdog/terrain/deck_edge_8.inc"
};
static NJS_CNK_OBJECT deck_edge_9 = {
#include "boss_fdog/terrain/deck_edge_9.inc"
};
