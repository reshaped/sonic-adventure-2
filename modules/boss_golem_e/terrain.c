#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "shared/golem_land.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char golem_land_filenames[127] = {
#include "boss_golem_e/terrain/golem_land_filenames.inc"
};

/* BOSS_GOLEM_E.PRS's land, texture lists and chunk models, from the disc.
 * its D lies between stage.c and object_textures.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[14];
struct land_table golem_land;
static NJS_TEXNAME textures[7];
NJS_TEXLIST golem_land_texlist;
static Sint16 draw_000_polygons[39];
static Sint32 draw_000_vertices[35];
static NJS_CNK_MODEL draw_000_model;
static NJS_CNK_OBJECT draw_000;
static Sint16 draw_001_polygons[121];
static Sint32 draw_001_vertices[79];
static NJS_CNK_MODEL draw_001_model;
static NJS_CNK_OBJECT draw_001;
static Sint16 draw_013_polygons[2135];
static Sint32 draw_013_vertices[1411];
static NJS_CNK_MODEL draw_013_model;
static NJS_CNK_OBJECT draw_013;
static NJS_CNK_OBJECT draw_002;
static NJS_CNK_OBJECT draw_003;
static NJS_CNK_OBJECT draw_004;
static NJS_CNK_OBJECT draw_005;
static NJS_CNK_OBJECT draw_006;
static NJS_CNK_OBJECT draw_007;
static NJS_CNK_OBJECT draw_008;
static NJS_CNK_OBJECT draw_009;
static NJS_CNK_OBJECT draw_010;
static NJS_CNK_OBJECT draw_011;
static NJS_CNK_OBJECT draw_012;

static struct land_entry entries[14] = {
#include "boss_golem_e/terrain/entries.inc"
};
struct land_table golem_land = {
#include "boss_golem_e/terrain/golem_land.inc"
};
static NJS_TEXNAME textures[7] = {
#include "boss_golem_e/terrain/textures.inc"
};
NJS_TEXLIST golem_land_texlist = {
#include "boss_golem_e/terrain/golem_land_texlist.inc"
};
static Sint16 draw_000_polygons[39] = {
#include "boss_golem_e/terrain/draw_000_polygons.inc"
};
static Sint32 draw_000_vertices[35] = {
#include "boss_golem_e/terrain/draw_000_vertices.inc"
};
static NJS_CNK_MODEL draw_000_model = {
#include "boss_golem_e/terrain/draw_000_model.inc"
};
static NJS_CNK_OBJECT draw_000 = {
#include "boss_golem_e/terrain/draw_000.inc"
};
static Sint16 draw_001_polygons[121] = {
#include "boss_golem_e/terrain/draw_001_polygons.inc"
};
static Sint32 draw_001_vertices[79] = {
#include "boss_golem_e/terrain/draw_001_vertices.inc"
};
static NJS_CNK_MODEL draw_001_model = {
#include "boss_golem_e/terrain/draw_001_model.inc"
};
static NJS_CNK_OBJECT draw_001 = {
#include "boss_golem_e/terrain/draw_001.inc"
};
static Sint16 draw_013_polygons[2135] = {
#include "boss_golem_e/terrain/draw_013_polygons.inc"
};
static Sint32 draw_013_vertices[1411] = {
#include "boss_golem_e/terrain/draw_013_vertices.inc"
};
static NJS_CNK_MODEL draw_013_model = {
#include "boss_golem_e/terrain/draw_013_model.inc"
};
static NJS_CNK_OBJECT draw_013 = {
#include "boss_golem_e/terrain/draw_013.inc"
};
static NJS_CNK_OBJECT draw_002 = {
#include "boss_golem_e/terrain/draw_002.inc"
};
static NJS_CNK_OBJECT draw_003 = {
#include "boss_golem_e/terrain/draw_003.inc"
};
static NJS_CNK_OBJECT draw_004 = {
#include "boss_golem_e/terrain/draw_004.inc"
};
static NJS_CNK_OBJECT draw_005 = {
#include "boss_golem_e/terrain/draw_005.inc"
};
static NJS_CNK_OBJECT draw_006 = {
#include "boss_golem_e/terrain/draw_006.inc"
};
static NJS_CNK_OBJECT draw_007 = {
#include "boss_golem_e/terrain/draw_007.inc"
};
static NJS_CNK_OBJECT draw_008 = {
#include "boss_golem_e/terrain/draw_008.inc"
};
static NJS_CNK_OBJECT draw_009 = {
#include "boss_golem_e/terrain/draw_009.inc"
};
static NJS_CNK_OBJECT draw_010 = {
#include "boss_golem_e/terrain/draw_010.inc"
};
static NJS_CNK_OBJECT draw_011 = {
#include "boss_golem_e/terrain/draw_011.inc"
};
static NJS_CNK_OBJECT draw_012 = {
#include "boss_golem_e/terrain/draw_012.inc"
};
