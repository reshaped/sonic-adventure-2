#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg27/scenery_land.h"

/* The background land references its archive and each texture in this pool. */
const char colony_poison_land_filenames[221] = {
#include "stg27/poison_land/colony_poison_land_filenames.inc"
};

/* STG27.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between terrain.c and lamp_land.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry colony_poison_land_entries[3];
struct land_table colony_poison_land;
static NJS_TEXNAME colony_poison_land_textures[13];
NJS_TEXLIST colony_poison_texlist;
static Sint16 colony_poison_land_draw_2_polygons[23];
static Sint32 colony_poison_land_draw_2_vertices[19];
static NJS_CNK_MODEL colony_poison_land_draw_2_model;
static NJS_CNK_OBJECT colony_poison_land_draw_2;
static Sint16 colony_poison_land_draw_1_polygons[23];
static Sint32 colony_poison_land_draw_1_vertices[19];
static NJS_CNK_MODEL colony_poison_land_draw_1_model;
static NJS_CNK_OBJECT colony_poison_land_draw_1;
static Sint16 colony_poison_land_draw_0_polygons[23];
static Sint32 colony_poison_land_draw_0_vertices[19];
static NJS_CNK_MODEL colony_poison_land_draw_0_model;
static NJS_CNK_OBJECT colony_poison_land_draw_0;

static struct land_entry colony_poison_land_entries[3] = {
#include "stg27/poison_land/colony_poison_land_entries.inc"
};
struct land_table colony_poison_land = {
#include "stg27/poison_land/colony_poison_land.inc"
};
static NJS_TEXNAME colony_poison_land_textures[13] = {
#include "stg27/poison_land/colony_poison_land_textures.inc"
};
NJS_TEXLIST colony_poison_texlist = {
#include "stg27/poison_land/colony_poison_texlist.inc"
};
static Sint16 colony_poison_land_draw_2_polygons[23] = {
#include "stg27/poison_land/colony_poison_land_draw_2_polygons.inc"
};
static Sint32 colony_poison_land_draw_2_vertices[19] = {
#include "stg27/poison_land/colony_poison_land_draw_2_vertices.inc"
};
static NJS_CNK_MODEL colony_poison_land_draw_2_model = {
#include "stg27/poison_land/colony_poison_land_draw_2_model.inc"
};
static NJS_CNK_OBJECT colony_poison_land_draw_2 = {
#include "stg27/poison_land/colony_poison_land_draw_2.inc"
};
static Sint16 colony_poison_land_draw_1_polygons[23] = {
#include "stg27/poison_land/colony_poison_land_draw_1_polygons.inc"
};
static Sint32 colony_poison_land_draw_1_vertices[19] = {
#include "stg27/poison_land/colony_poison_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL colony_poison_land_draw_1_model = {
#include "stg27/poison_land/colony_poison_land_draw_1_model.inc"
};
static NJS_CNK_OBJECT colony_poison_land_draw_1 = {
#include "stg27/poison_land/colony_poison_land_draw_1.inc"
};
static Sint16 colony_poison_land_draw_0_polygons[23] = {
#include "stg27/poison_land/colony_poison_land_draw_0_polygons.inc"
};
static Sint32 colony_poison_land_draw_0_vertices[19] = {
#include "stg27/poison_land/colony_poison_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL colony_poison_land_draw_0_model = {
#include "stg27/poison_land/colony_poison_land_draw_0_model.inc"
};
static NJS_CNK_OBJECT colony_poison_land_draw_0 = {
#include "stg27/poison_land/colony_poison_land_draw_0.inc"
};
