#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg32/sky.h"

/* the file names that meteor_herd_warning_land and its texture list point at.
 * each starts on a 4-byte boundary */
const char meteor_herd_warning_land_filenames[222] = {
#include "stg32/sky_warning/meteor_herd_warning_land_filenames.inc"
};

/* STG32.PRS's texture lists, chunk models and background land, from the
 * disc. its D lies between sky_surfaces.c and object_archives.c */
static NJS_TEXNAME meteor_herd_warning_land_textures[13] = {
#include "stg32/sky_warning/meteor_herd_warning_land_textures.inc"
};
NJS_TEXLIST meteor_warning_texlist = {
#include "stg32/sky_warning/meteor_warning_texlist.inc"
};
static Sint16 meteor_herd_warning_land_draw_2_polygons[23] = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_2_polygons.inc"
};
static Sint32 meteor_herd_warning_land_draw_2_vertices[19] = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_2_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_warning_land_draw_2_model = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_2_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_warning_land_draw_2 = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_2.inc"
};
static Sint16 meteor_herd_warning_land_draw_1_polygons[23] = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_1_polygons.inc"
};
static Sint32 meteor_herd_warning_land_draw_1_vertices[19] = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_warning_land_draw_1_model = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_1_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_warning_land_draw_1 = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_1.inc"
};
static Sint16 meteor_herd_warning_land_draw_0_polygons[23] = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_0_polygons.inc"
};
static Sint32 meteor_herd_warning_land_draw_0_vertices[19] = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_warning_land_draw_0_model = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_0_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_warning_land_draw_0 = {
#include "stg32/sky_warning/meteor_herd_warning_land_draw_0.inc"
};
static struct land_entry meteor_herd_warning_land_entries[3] = {
#include "stg32/sky_warning/meteor_herd_warning_land_entries.inc"
};
struct land_table meteor_herd_warning_land = {
#include "stg32/sky_warning/meteor_herd_warning_land.inc"
};
