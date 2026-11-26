#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg27/scenery_land.h"

/* The background land references its archive and each texture in this pool. */
const char colony_sky_land_filenames[25] = {
#include "stg27/sky_land/colony_sky_land_filenames.inc"
};

/* STG27.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between lamp_land.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry colony_sky_land_entries[15];
struct land_table colony_sky_land;
static NJS_TEXNAME colony_sky_land_textures[1];
NJS_TEXLIST colony_sky_land_texlist;
static Sint16 colony_sky_land_draw_5_polygons[75];
static Sint32 colony_sky_land_draw_5_vertices[83];
static NJS_CNK_MODEL colony_sky_land_draw_5_model;
NJS_CNK_OBJECT colony_pulse_1;
static Sint16 colony_sky_land_draw_9_polygons[89];
static Sint32 colony_sky_land_draw_9_vertices[99];
static NJS_CNK_MODEL colony_sky_land_draw_9_model;
NJS_CNK_OBJECT colony_pulse_0;
static Sint16 colony_sky_land_draw_0_polygons[75];
static Sint32 colony_sky_land_draw_0_vertices[83];
static NJS_CNK_MODEL colony_sky_land_draw_0_model;
NJS_CNK_OBJECT colony_pulse_2;
static NJS_CNK_OBJECT colony_sky_land_draw_12;
static NJS_CNK_OBJECT colony_sky_land_draw_11;
static NJS_CNK_OBJECT colony_sky_land_draw_10;
static NJS_CNK_OBJECT colony_sky_land_draw_9;
static NJS_CNK_OBJECT colony_sky_land_draw_8;
static NJS_CNK_OBJECT colony_sky_land_draw_7;
static NJS_CNK_OBJECT colony_sky_land_draw_6;
static NJS_CNK_OBJECT colony_sky_land_draw_5;
static NJS_CNK_OBJECT colony_sky_land_draw_3;
static NJS_CNK_OBJECT colony_sky_land_draw_2;
static NJS_CNK_OBJECT colony_sky_land_draw_1;
static NJS_CNK_OBJECT colony_sky_land_draw_0;

static struct land_entry colony_sky_land_entries[15] = {
#include "stg27/sky_land/colony_sky_land_entries.inc"
};
struct land_table colony_sky_land = {
#include "stg27/sky_land/colony_sky_land.inc"
};
static NJS_TEXNAME colony_sky_land_textures[1] = {
#include "stg27/sky_land/colony_sky_land_textures.inc"
};
NJS_TEXLIST colony_sky_land_texlist = {
#include "stg27/sky_land/colony_sky_land_texlist.inc"
};
static Sint16 colony_sky_land_draw_5_polygons[75] = {
#include "stg27/sky_land/colony_sky_land_draw_5_polygons.inc"
};
static Sint32 colony_sky_land_draw_5_vertices[83] = {
#include "stg27/sky_land/colony_sky_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL colony_sky_land_draw_5_model = {
#include "stg27/sky_land/colony_sky_land_draw_5_model.inc"
};
NJS_CNK_OBJECT colony_pulse_1 = {
#include "stg27/sky_land/colony_pulse_1.inc"
};
static Sint16 colony_sky_land_draw_9_polygons[89] = {
#include "stg27/sky_land/colony_sky_land_draw_9_polygons.inc"
};
static Sint32 colony_sky_land_draw_9_vertices[99] = {
#include "stg27/sky_land/colony_sky_land_draw_9_vertices.inc"
};
static NJS_CNK_MODEL colony_sky_land_draw_9_model = {
#include "stg27/sky_land/colony_sky_land_draw_9_model.inc"
};
NJS_CNK_OBJECT colony_pulse_0 = {
#include "stg27/sky_land/colony_pulse_0.inc"
};
static Sint16 colony_sky_land_draw_0_polygons[75] = {
#include "stg27/sky_land/colony_sky_land_draw_0_polygons.inc"
};
static Sint32 colony_sky_land_draw_0_vertices[83] = {
#include "stg27/sky_land/colony_sky_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL colony_sky_land_draw_0_model = {
#include "stg27/sky_land/colony_sky_land_draw_0_model.inc"
};
NJS_CNK_OBJECT colony_pulse_2 = {
#include "stg27/sky_land/colony_pulse_2.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_12 = {
#include "stg27/sky_land/colony_sky_land_draw_12.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_11 = {
#include "stg27/sky_land/colony_sky_land_draw_11.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_10 = {
#include "stg27/sky_land/colony_sky_land_draw_10.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_9 = {
#include "stg27/sky_land/colony_sky_land_draw_9.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_8 = {
#include "stg27/sky_land/colony_sky_land_draw_8.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_7 = {
#include "stg27/sky_land/colony_sky_land_draw_7.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_6 = {
#include "stg27/sky_land/colony_sky_land_draw_6.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_5 = {
#include "stg27/sky_land/colony_sky_land_draw_5.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_3 = {
#include "stg27/sky_land/colony_sky_land_draw_3.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_2 = {
#include "stg27/sky_land/colony_sky_land_draw_2.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_1 = {
#include "stg27/sky_land/colony_sky_land_draw_1.inc"
};
static NJS_CNK_OBJECT colony_sky_land_draw_0 = {
#include "stg27/sky_land/colony_sky_land_draw_0.inc"
};
