#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg32/sky.h"

/* the file names that meteor_herd_background_land and its texture list point
 * at. each starts on a 4-byte boundary */
const char meteor_herd_background_land_filenames[48] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_filenames.inc"
};

/* STG32.PRS's texture lists, chunk models and background land, from the
 * disc. its D lies between sky_changes.c and sky_warning.c */
static NJS_TEXNAME meteor_herd_background_land_textures[3] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_textures.inc"
};
NJS_TEXLIST meteor_herd_animated_surface_texlist = {
#include "stg32/sky_surfaces/meteor_herd_animated_surface_texlist.inc"
};
static Sint16 meteor_herd_background_land_draw_13_polygons[101] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_13_polygons.inc"
};
static Sint32 meteor_herd_background_land_draw_13_vertices[83] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_13_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_background_land_draw_13_model = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_13_model.inc"
};
NJS_CNK_OBJECT meteor_herd_surface_0 = {
#include "stg32/sky_surfaces/meteor_herd_surface_0.inc"
};
static Sint16 meteor_herd_background_land_draw_9_polygons[105] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_9_polygons.inc"
};
static Sint32 meteor_herd_background_land_draw_9_vertices[43] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_9_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_background_land_draw_9_model = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_9_model.inc"
};
NJS_CNK_OBJECT meteor_herd_surface_1 = {
#include "stg32/sky_surfaces/meteor_herd_surface_1.inc"
};
static Sint16 meteor_herd_background_land_draw_5_polygons[93] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_5_polygons.inc"
};
static Sint32 meteor_herd_background_land_draw_5_vertices[67] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_background_land_draw_5_model = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_5_model.inc"
};
NJS_CNK_OBJECT meteor_herd_surface_2 = {
#include "stg32/sky_surfaces/meteor_herd_surface_2.inc"
};
static Sint16 meteor_herd_background_land_draw_1_polygons[133] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_1_polygons.inc"
};
static Sint32 meteor_herd_background_land_draw_1_vertices[95] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_background_land_draw_1_model = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_1_model.inc"
};
NJS_CNK_OBJECT meteor_herd_surface_3 = {
#include "stg32/sky_surfaces/meteor_herd_surface_3.inc"
};
static Sint16 meteor_herd_background_land_draw_0_polygons[133] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_0_polygons.inc"
};
static Sint32 meteor_herd_background_land_draw_0_vertices[67] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_background_land_draw_0_model = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_0_model.inc"
};
NJS_CNK_OBJECT meteor_herd_surface_4 = {
#include "stg32/sky_surfaces/meteor_herd_surface_4.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_15 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_15.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_14 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_14.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_13 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_13.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_11 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_11.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_10 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_10.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_9 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_9.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_7 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_7.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_6 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_6.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_5 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_5.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_3 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_3.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_2 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_2.inc"
};
static NJS_CNK_OBJECT meteor_herd_background_land_draw_1 = {
#include "stg32/sky_surfaces/meteor_herd_background_land_draw_1.inc"
};
static struct land_entry meteor_herd_background_land_entries[17] = {
#include "stg32/sky_surfaces/meteor_herd_background_land_entries.inc"
};
struct land_table meteor_herd_background_land = {
#include "stg32/sky_surfaces/meteor_herd_background_land.inc"
};
