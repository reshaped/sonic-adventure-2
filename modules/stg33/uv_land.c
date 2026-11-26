#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg33/uv_land.h"

/* the file names that uv_land and its texture list point at. each starts on a
 * 4-byte boundary */
const char uv_land_filenames[35] = {
#include "stg33/uv_land/uv_land_filenames.inc"
};

/* STG33.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between rival_hunter.c and land_draw.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry uv_land_entries[8];
struct land_table uv_land;
static NJS_TEXNAME uv_land_textures[2];
NJS_TEXLIST knuckles_rouge_animated_surface_texlist;
static Sint16 uv_land_draw_4_polygons[101];
static Sint32 uv_land_draw_4_vertices[83];
static NJS_CNK_MODEL uv_land_draw_4_model;
NJS_CNK_OBJECT uv_land_lower_object;
static Sint16 uv_land_draw_0_polygons[105];
static Sint32 uv_land_draw_0_vertices[43];
static NJS_CNK_MODEL uv_land_draw_0_model;
NJS_CNK_OBJECT uv_land_upper_object;
static NJS_CNK_OBJECT uv_land_draw_6;
static NJS_CNK_OBJECT uv_land_draw_5;
static NJS_CNK_OBJECT uv_land_draw_4;
static NJS_CNK_OBJECT uv_land_draw_2;
static NJS_CNK_OBJECT uv_land_draw_1;
static NJS_CNK_OBJECT uv_land_draw_0;

static struct land_entry uv_land_entries[8] = {
#include "stg33/uv_land/uv_land_entries.inc"
};
struct land_table uv_land = {
#include "stg33/uv_land/uv_land.inc"
};
static NJS_TEXNAME uv_land_textures[2] = {
#include "stg33/uv_land/uv_land_textures.inc"
};
NJS_TEXLIST knuckles_rouge_animated_surface_texlist = {
#include "stg33/uv_land/knuckles_rouge_animated_surface_texlist.inc"
};
static Sint16 uv_land_draw_4_polygons[101] = {
#include "stg33/uv_land/uv_land_draw_4_polygons.inc"
};
static Sint32 uv_land_draw_4_vertices[83] = {
#include "stg33/uv_land/uv_land_draw_4_vertices.inc"
};
static NJS_CNK_MODEL uv_land_draw_4_model = {
#include "stg33/uv_land/uv_land_draw_4_model.inc"
};
NJS_CNK_OBJECT uv_land_lower_object = {
#include "stg33/uv_land/uv_land_lower_object.inc"
};
static Sint16 uv_land_draw_0_polygons[105] = {
#include "stg33/uv_land/uv_land_draw_0_polygons.inc"
};
static Sint32 uv_land_draw_0_vertices[43] = {
#include "stg33/uv_land/uv_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL uv_land_draw_0_model = {
#include "stg33/uv_land/uv_land_draw_0_model.inc"
};
NJS_CNK_OBJECT uv_land_upper_object = {
#include "stg33/uv_land/uv_land_upper_object.inc"
};
static NJS_CNK_OBJECT uv_land_draw_6 = {
#include "stg33/uv_land/uv_land_draw_6.inc"
};
static NJS_CNK_OBJECT uv_land_draw_5 = {
#include "stg33/uv_land/uv_land_draw_5.inc"
};
static NJS_CNK_OBJECT uv_land_draw_4 = {
#include "stg33/uv_land/uv_land_draw_4.inc"
};
static NJS_CNK_OBJECT uv_land_draw_2 = {
#include "stg33/uv_land/uv_land_draw_2.inc"
};
static NJS_CNK_OBJECT uv_land_draw_1 = {
#include "stg33/uv_land/uv_land_draw_1.inc"
};
static NJS_CNK_OBJECT uv_land_draw_0 = {
#include "stg33/uv_land/uv_land_draw_0.inc"
};
