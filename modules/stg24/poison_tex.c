#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg24/scenery.h"

/* The background land references its archive and each texture in this pool. */
const char engine_poison_land_filenames[221] = {
#include "stg24/poison_tex/engine_poison_land_filenames.inc"
};

/* STG24.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between terrain.c and pulse_tex.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry engine_poison_land_entries[1];
struct land_table engine_poison_land;
static NJS_TEXNAME engine_poison_land_textures[13];
NJS_TEXLIST engine_poison_texlist;
static Sint16 engine_poison_land_draw_0_polygons[19];
static Sint32 engine_poison_land_draw_0_vertices[19];
static NJS_CNK_MODEL engine_poison_land_draw_0_model;
static NJS_CNK_OBJECT engine_poison_land_draw_0;

static struct land_entry engine_poison_land_entries[1] = {
#include "stg24/poison_tex/engine_poison_land_entries.inc"
};
struct land_table engine_poison_land = {
#include "stg24/poison_tex/engine_poison_land.inc"
};
static NJS_TEXNAME engine_poison_land_textures[13] = {
#include "stg24/poison_tex/engine_poison_land_textures.inc"
};
NJS_TEXLIST engine_poison_texlist = {
#include "stg24/poison_tex/engine_poison_texlist.inc"
};
static Sint16 engine_poison_land_draw_0_polygons[19] = {
#include "stg24/poison_tex/engine_poison_land_draw_0_polygons.inc"
};
static Sint32 engine_poison_land_draw_0_vertices[19] = {
#include "stg24/poison_tex/engine_poison_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL engine_poison_land_draw_0_model = {
#include "stg24/poison_tex/engine_poison_land_draw_0_model.inc"
};
static NJS_CNK_OBJECT engine_poison_land_draw_0 = {
#include "stg24/poison_tex/engine_poison_land_draw_0.inc"
};
