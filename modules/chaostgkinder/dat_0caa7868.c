/* CHAOSTGKINDER.PRS's texture lists, chunk models and land, from the disc.
 * its D lies between dat_0ca984f4.c and dat_0cab1714.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/land_data.h"
#include "chaostgkinder/land_textures.h"
#include "collision/world.h"

static NJS_TEXNAME health_center_textures[35] = {
#include "chaostgkinder/dat_0caa7868/health_center_textures.inc"
};
NJS_TEXLIST kinder_health_center_texlist = {
#include "chaostgkinder/dat_0caa7868/kinder_health_center_texlist.inc"
};
static Sint16 health_center_stool_polygons[935] = {
#include "chaostgkinder/dat_0caa7868/health_center_stool_polygons.inc"
};
static Sint32 health_center_stool_vertices[921] = {
#include "chaostgkinder/dat_0caa7868/health_center_stool_vertices.inc"
};
static NJS_CNK_MODEL health_center_stool_model = {
#include "chaostgkinder/dat_0caa7868/health_center_stool_model.inc"
};
static NJS_CNK_OBJECT health_center_stool = {
#include "chaostgkinder/dat_0caa7868/health_center_stool.inc"
};
static Sint16 health_center_bed_polygons[1163] = {
#include "chaostgkinder/dat_0caa7868/health_center_bed_polygons.inc"
};
static Sint32 health_center_bed_vertices[731] = {
#include "chaostgkinder/dat_0caa7868/health_center_bed_vertices.inc"
};
static NJS_CNK_MODEL health_center_bed_model = {
#include "chaostgkinder/dat_0caa7868/health_center_bed_model.inc"
};
static NJS_CNK_OBJECT health_center_bed = {
#include "chaostgkinder/dat_0caa7868/health_center_bed.inc"
};
static Sint16 health_center_bedside_drawers_polygons[261] = {
#include "chaostgkinder/dat_0caa7868/health_center_bedside_drawers_polygons.inc"
};
static Sint32 health_center_bedside_drawers_vertices[147] = {
#include "chaostgkinder/dat_0caa7868/health_center_bedside_drawers_vertices.inc"
};
static NJS_CNK_MODEL health_center_drawers_model = {
#include "chaostgkinder/dat_0caa7868/health_center_drawers_model.inc"
};
static NJS_CNK_OBJECT health_center_drawers = {
#include "chaostgkinder/dat_0caa7868/health_center_drawers.inc"
};
static Sint16 health_center_curtain_polygons[419] = {
#include "chaostgkinder/dat_0caa7868/health_center_curtain_polygons.inc"
};
static Sint32 health_center_curtain_vertices[339] = {
#include "chaostgkinder/dat_0caa7868/health_center_curtain_vertices.inc"
};
static NJS_CNK_MODEL health_center_curtain_model = {
#include "chaostgkinder/dat_0caa7868/health_center_curtain_model.inc"
};
static NJS_CNK_OBJECT health_center_curtain = {
#include "chaostgkinder/dat_0caa7868/health_center_curtain.inc"
};
static Sint16 health_center_exam_table_polygons[655] = {
#include "chaostgkinder/dat_0caa7868/health_center_exam_table_polygons.inc"
};
static Sint32 health_center_exam_table_vertices[463] = {
#include "chaostgkinder/dat_0caa7868/health_center_exam_table_vertices.inc"
};
static NJS_CNK_MODEL health_center_exam_table_model = {
#include "chaostgkinder/dat_0caa7868/health_center_exam_table_model.inc"
};
static NJS_CNK_OBJECT health_center_exam_table = {
#include "chaostgkinder/dat_0caa7868/health_center_exam_table.inc"
};
static Sint16 health_center_bedside_tray_polygons[259] = {
#include "chaostgkinder/dat_0caa7868/health_center_bedside_tray_polygons.inc"
};
static Sint32 health_center_bedside_tray_vertices[251] = {
#include "chaostgkinder/dat_0caa7868/health_center_bedside_tray_vertices.inc"
};
static NJS_CNK_MODEL health_center_bedside_tray_model = {
#include "chaostgkinder/dat_0caa7868/health_center_bedside_tray_model.inc"
};
static NJS_CNK_OBJECT health_center_bedside_tray = {
#include "chaostgkinder/dat_0caa7868/health_center_bedside_tray.inc"
};
static Sint16 health_center_cabinet_glass_polygons[25] = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet_glass_polygons.inc"
};
static Sint32 health_center_cabinet_glass_vertices[51] = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet_glass_vertices.inc"
};
static NJS_CNK_MODEL health_center_cabinet_glass_model = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet_glass_model.inc"
};
static NJS_CNK_OBJECT health_center_cabinet_glass = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet_glass.inc"
};
static Sint16 health_center_cabinet_polygons[1039] = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet_polygons.inc"
};
static Sint32 health_center_cabinet_vertices[743] = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet_vertices.inc"
};
static NJS_CNK_MODEL health_center_cabinet_model = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet_model.inc"
};
static NJS_CNK_OBJECT health_center_cabinet = {
#include "chaostgkinder/dat_0caa7868/health_center_cabinet.inc"
};
static Sint16 health_center_floor_polygons[123] = {
#include "chaostgkinder/dat_0caa7868/health_center_floor_polygons.inc"
};
static Sint32 health_center_floor_vertices[99] = {
#include "chaostgkinder/dat_0caa7868/health_center_floor_vertices.inc"
};
static NJS_CNK_MODEL health_center_floor_model = {
#include "chaostgkinder/dat_0caa7868/health_center_floor_model.inc"
};
static NJS_CNK_OBJECT health_center_tiled_floor = {
#include "chaostgkinder/dat_0caa7868/health_center_tiled_floor.inc"
};
static Sint16 health_center_desk_supplies_polygons[611] = {
#include "chaostgkinder/dat_0caa7868/health_center_desk_supplies_polygons.inc"
};
static Sint32 health_center_desk_supplies_vertices[507] = {
#include "chaostgkinder/dat_0caa7868/health_center_desk_supplies_vertices.inc"
};
static NJS_CNK_MODEL health_center_desk_supplies_model = {
#include "chaostgkinder/dat_0caa7868/health_center_desk_supplies_model.inc"
};
static NJS_CNK_OBJECT health_center_desk_supplies = {
#include "chaostgkinder/dat_0caa7868/health_center_desk_supplies.inc"
};
static Sint16 health_center_ceiling_light_polygons[167] = {
#include "chaostgkinder/dat_0caa7868/health_center_ceiling_light_polygons.inc"
};
static Sint32 health_center_ceiling_light_vertices[103] = {
#include "chaostgkinder/dat_0caa7868/health_center_ceiling_light_vertices.inc"
};
static NJS_CNK_MODEL health_center_ceiling_light_model = {
#include "chaostgkinder/dat_0caa7868/health_center_ceiling_light_model.inc"
};
static NJS_CNK_OBJECT health_center_left_light = {
#include "chaostgkinder/dat_0caa7868/health_center_left_light.inc"
};
static Sint16 health_center_desk_polygons[739] = {
#include "chaostgkinder/dat_0caa7868/health_center_desk_polygons.inc"
};
static Sint32 health_center_desk_vertices[635] = {
#include "chaostgkinder/dat_0caa7868/health_center_desk_vertices.inc"
};
static NJS_CNK_MODEL health_center_desk_model = {
#include "chaostgkinder/dat_0caa7868/health_center_desk_model.inc"
};
static NJS_CNK_OBJECT health_center_desk = {
#include "chaostgkinder/dat_0caa7868/health_center_desk.inc"
};
static Sint16 health_center_posters_polygons[43] = {
#include "chaostgkinder/dat_0caa7868/health_center_posters_polygons.inc"
};
static Sint32 health_center_posters_vertices[35] = {
#include "chaostgkinder/dat_0caa7868/health_center_posters_vertices.inc"
};
static NJS_CNK_MODEL health_center_posters_model = {
#include "chaostgkinder/dat_0caa7868/health_center_posters_model.inc"
};
static NJS_CNK_OBJECT health_center_posters = {
#include "chaostgkinder/dat_0caa7868/health_center_posters.inc"
};
static Sint16 health_center_walls_polygons[689] = {
#include "chaostgkinder/dat_0caa7868/health_center_walls_polygons.inc"
};
static Sint32 health_center_walls_vertices[491] = {
#include "chaostgkinder/dat_0caa7868/health_center_walls_vertices.inc"
};
static NJS_CNK_MODEL health_center_walls_model = {
#include "chaostgkinder/dat_0caa7868/health_center_walls_model.inc"
};
static NJS_CNK_OBJECT health_center_walls = {
#include "chaostgkinder/dat_0caa7868/health_center_walls.inc"
};
static Sint16 health_center_window_polygons[331] = {
#include "chaostgkinder/dat_0caa7868/health_center_window_polygons.inc"
};
static Sint32 health_center_window_vertices[319] = {
#include "chaostgkinder/dat_0caa7868/health_center_window_vertices.inc"
};
static NJS_CNK_MODEL health_center_window_model = {
#include "chaostgkinder/dat_0caa7868/health_center_window_model.inc"
};
static NJS_CNK_OBJECT health_center_window = {
#include "chaostgkinder/dat_0caa7868/health_center_window.inc"
};
static NJS_CNK_OBJECT health_center_second_bed = {
#include "chaostgkinder/dat_0caa7868/health_center_second_bed.inc"
};
static NJS_CNK_OBJECT health_center_second_drawers = {
#include "chaostgkinder/dat_0caa7868/health_center_second_drawers.inc"
};
static NJS_CNK_OBJECT health_center_right_light = {
#include "chaostgkinder/dat_0caa7868/health_center_right_light.inc"
};
struct land_entry kinder_health_center_entries[18] = {
#include "chaostgkinder/dat_0caa7868/kinder_health_center_entries.inc"
};
struct land_table kinder_health_center_land = {
#include "chaostgkinder/dat_0caa7868/kinder_health_center_land.inc"
};
