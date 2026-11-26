/* STG38.PRS's texture lists and chunk models, from the disc. its D lies
 * between poison_scenery.c and shaft_inner_wall_models.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_scenery_art.h"
#include "shared/shaft_wall_art.h"
#include "stg38/water_scenery.h"

static NJS_TEXNAME inner_wall_entry_slot[1] = {
#include "stg38/wall_placements/inner_wall_entry_slot.inc"
};
static NJS_TEXLIST inner_wall_entry_binding = {
#include "stg38/wall_placements/inner_wall_entry_binding.inc"
};
static Sint16 inner_wall_entry_panel_1_polygons[95] = {
#include "stg38/wall_placements/inner_wall_entry_panel_1_polygons.inc"
};
static Sint32 inner_wall_entry_panel_1_vertices[67] = {
#include "stg38/wall_placements/inner_wall_entry_panel_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_entry_panel_1_mesh = {
#include "stg38/wall_placements/inner_wall_entry_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_entry_panel_1 = {
#include "stg38/wall_placements/inner_wall_entry_panel_1.inc"
};
NJS_CNK_OBJECT inner_wall_entry = {
#include "stg38/wall_placements/inner_wall_entry.inc"
};
static NJS_TEXNAME inner_wall_shaft_slot[1] = {
#include "stg38/wall_placements/inner_wall_shaft_slot.inc"
};
static NJS_TEXLIST inner_wall_shaft_binding = {
#include "stg38/wall_placements/inner_wall_shaft_binding.inc"
};
static Sint16 inner_wall_shaft_panel_3_polygons[95] = {
#include "stg38/wall_placements/inner_wall_shaft_panel_3_polygons.inc"
};
static Sint32 inner_wall_shaft_panel_3_vertices[67] = {
#include "stg38/wall_placements/inner_wall_shaft_panel_3_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_shaft_panel_3_mesh = {
#include "stg38/wall_placements/inner_wall_shaft_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_shaft_panel_3 = {
#include "stg38/wall_placements/inner_wall_shaft_panel_3.inc"
};
static Sint16 inner_wall_shaft_panel_2_polygons[95] = {
#include "stg38/wall_placements/inner_wall_shaft_panel_2_polygons.inc"
};
static Sint32 inner_wall_shaft_panel_2_vertices[67] = {
#include "stg38/wall_placements/inner_wall_shaft_panel_2_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_shaft_panel_2_mesh = {
#include "stg38/wall_placements/inner_wall_shaft_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_shaft_panel_2 = {
#include "stg38/wall_placements/inner_wall_shaft_panel_2.inc"
};
static Sint16 inner_wall_shaft_panel_1_polygons[95] = {
#include "stg38/wall_placements/inner_wall_shaft_panel_1_polygons.inc"
};
static Sint32 inner_wall_shaft_panel_1_vertices[67] = {
#include "stg38/wall_placements/inner_wall_shaft_panel_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_shaft_panel_1_mesh = {
#include "stg38/wall_placements/inner_wall_shaft_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_shaft_panel_1 = {
#include "stg38/wall_placements/inner_wall_shaft_panel_1.inc"
};
NJS_CNK_OBJECT inner_wall_shaft = {
#include "stg38/wall_placements/inner_wall_shaft.inc"
};
static NJS_TEXNAME inner_wall_channel_slot[1] = {
#include "stg38/wall_placements/inner_wall_channel_slot.inc"
};
static NJS_TEXLIST inner_wall_channel_binding = {
#include "stg38/wall_placements/inner_wall_channel_binding.inc"
};
static Sint16 inner_wall_channel_panel_3_polygons[95] = {
#include "stg38/wall_placements/inner_wall_channel_panel_3_polygons.inc"
};
static Sint32 inner_wall_channel_panel_3_vertices[67] = {
#include "stg38/wall_placements/inner_wall_channel_panel_3_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_channel_panel_3_mesh = {
#include "stg38/wall_placements/inner_wall_channel_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_channel_panel_3 = {
#include "stg38/wall_placements/inner_wall_channel_panel_3.inc"
};
static Sint16 inner_wall_channel_panel_2_polygons[95] = {
#include "stg38/wall_placements/inner_wall_channel_panel_2_polygons.inc"
};
static Sint32 inner_wall_channel_panel_2_vertices[67] = {
#include "stg38/wall_placements/inner_wall_channel_panel_2_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_channel_panel_2_mesh = {
#include "stg38/wall_placements/inner_wall_channel_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_channel_panel_2 = {
#include "stg38/wall_placements/inner_wall_channel_panel_2.inc"
};
static Sint16 inner_wall_channel_panel_1_polygons[95] = {
#include "stg38/wall_placements/inner_wall_channel_panel_1_polygons.inc"
};
static Sint32 inner_wall_channel_panel_1_vertices[67] = {
#include "stg38/wall_placements/inner_wall_channel_panel_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_channel_panel_1_mesh = {
#include "stg38/wall_placements/inner_wall_channel_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_channel_panel_1 = {
#include "stg38/wall_placements/inner_wall_channel_panel_1.inc"
};
NJS_CNK_OBJECT inner_wall_channel = {
#include "stg38/wall_placements/inner_wall_channel.inc"
};
static NJS_TEXNAME inner_wall_exit_slot[1] = {
#include "stg38/wall_placements/inner_wall_exit_slot.inc"
};
static NJS_TEXLIST inner_wall_exit_binding = {
#include "stg38/wall_placements/inner_wall_exit_binding.inc"
};
static Sint16 inner_wall_exit_panel_2_polygons[95] = {
#include "stg38/wall_placements/inner_wall_exit_panel_2_polygons.inc"
};
static Sint32 inner_wall_exit_panel_2_vertices[67] = {
#include "stg38/wall_placements/inner_wall_exit_panel_2_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_exit_panel_2_mesh = {
#include "stg38/wall_placements/inner_wall_exit_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_exit_panel_2 = {
#include "stg38/wall_placements/inner_wall_exit_panel_2.inc"
};
static Sint16 inner_wall_exit_panel_1_polygons[95] = {
#include "stg38/wall_placements/inner_wall_exit_panel_1_polygons.inc"
};
static Sint32 inner_wall_exit_panel_1_vertices[67] = {
#include "stg38/wall_placements/inner_wall_exit_panel_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_exit_panel_1_mesh = {
#include "stg38/wall_placements/inner_wall_exit_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT inner_wall_exit_panel_1 = {
#include "stg38/wall_placements/inner_wall_exit_panel_1.inc"
};
NJS_CNK_OBJECT inner_wall_exit = {
#include "stg38/wall_placements/inner_wall_exit.inc"
};
static NJS_TEXNAME middle_wall_entry_slot[1] = {
#include "stg38/wall_placements/middle_wall_entry_slot.inc"
};
static NJS_TEXLIST middle_wall_entry_binding = {
#include "stg38/wall_placements/middle_wall_entry_binding.inc"
};
static Sint16 middle_wall_entry_panel_1_polygons[95] = {
#include "stg38/wall_placements/middle_wall_entry_panel_1_polygons.inc"
};
static Sint32 middle_wall_entry_panel_1_vertices[67] = {
#include "stg38/wall_placements/middle_wall_entry_panel_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_entry_panel_1_mesh = {
#include "stg38/wall_placements/middle_wall_entry_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_entry_panel_1 = {
#include "stg38/wall_placements/middle_wall_entry_panel_1.inc"
};
NJS_CNK_OBJECT middle_wall_entry = {
#include "stg38/wall_placements/middle_wall_entry.inc"
};
static NJS_TEXNAME middle_wall_shaft_slot[1] = {
#include "stg38/wall_placements/middle_wall_shaft_slot.inc"
};
static NJS_TEXLIST middle_wall_shaft_binding = {
#include "stg38/wall_placements/middle_wall_shaft_binding.inc"
};
static Sint16 middle_wall_shaft_panel_1_polygons[95] = {
#include "stg38/wall_placements/middle_wall_shaft_panel_1_polygons.inc"
};
static Sint32 middle_wall_shaft_panel_1_vertices[67] = {
#include "stg38/wall_placements/middle_wall_shaft_panel_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_shaft_panel_1_mesh = {
#include "stg38/wall_placements/middle_wall_shaft_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_shaft_panel_1 = {
#include "stg38/wall_placements/middle_wall_shaft_panel_1.inc"
};
NJS_CNK_OBJECT middle_wall_shaft = {
#include "stg38/wall_placements/middle_wall_shaft.inc"
};
static NJS_TEXNAME middle_wall_lower_slot[1] = {
#include "stg38/wall_placements/middle_wall_lower_slot.inc"
};
static NJS_TEXLIST middle_wall_lower_binding = {
#include "stg38/wall_placements/middle_wall_lower_binding.inc"
};
static Sint16 middle_wall_lower_panel_1_polygons[95] = {
#include "stg38/wall_placements/middle_wall_lower_panel_1_polygons.inc"
};
static Sint32 middle_wall_lower_panel_1_vertices[67] = {
#include "stg38/wall_placements/middle_wall_lower_panel_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_lower_panel_1_mesh = {
#include "stg38/wall_placements/middle_wall_lower_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_lower_panel_1 = {
#include "stg38/wall_placements/middle_wall_lower_panel_1.inc"
};
NJS_CNK_OBJECT middle_wall_lower = {
#include "stg38/wall_placements/middle_wall_lower.inc"
};
static NJS_TEXNAME middle_wall_channel_slot[1] = {
#include "stg38/wall_placements/middle_wall_channel_slot.inc"
};
static NJS_TEXLIST middle_wall_channel_binding = {
#include "stg38/wall_placements/middle_wall_channel_binding.inc"
};
static Sint16 middle_wall_channel_panel_1_polygons[95] = {
#include "stg38/wall_placements/middle_wall_channel_panel_1_polygons.inc"
};
static Sint32 middle_wall_channel_panel_1_vertices[67] = {
#include "stg38/wall_placements/middle_wall_channel_panel_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_channel_panel_1_mesh = {
#include "stg38/wall_placements/middle_wall_channel_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_channel_panel_1 = {
#include "stg38/wall_placements/middle_wall_channel_panel_1.inc"
};
NJS_CNK_OBJECT middle_wall_channel = {
#include "stg38/wall_placements/middle_wall_channel.inc"
};
static NJS_TEXNAME middle_wall_exit_slot[1] = {
#include "stg38/wall_placements/middle_wall_exit_slot.inc"
};
static NJS_TEXLIST middle_wall_exit_binding = {
#include "stg38/wall_placements/middle_wall_exit_binding.inc"
};
static Sint16 middle_wall_exit_panel_2_polygons[95] = {
#include "stg38/wall_placements/middle_wall_exit_panel_2_polygons.inc"
};
static Sint32 middle_wall_exit_panel_2_vertices[67] = {
#include "stg38/wall_placements/middle_wall_exit_panel_2_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_exit_panel_2_mesh = {
#include "stg38/wall_placements/middle_wall_exit_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_exit_panel_2 = {
#include "stg38/wall_placements/middle_wall_exit_panel_2.inc"
};
static Sint16 middle_wall_exit_panel_1_polygons[95] = {
#include "stg38/wall_placements/middle_wall_exit_panel_1_polygons.inc"
};
static Sint32 middle_wall_exit_panel_1_vertices[67] = {
#include "stg38/wall_placements/middle_wall_exit_panel_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_exit_panel_1_mesh = {
#include "stg38/wall_placements/middle_wall_exit_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT middle_wall_exit_panel_1 = {
#include "stg38/wall_placements/middle_wall_exit_panel_1.inc"
};
NJS_CNK_OBJECT middle_wall_exit = {
#include "stg38/wall_placements/middle_wall_exit.inc"
};
static NJS_TEXNAME outer_wall_entry_slot[1] = {
#include "stg38/wall_placements/outer_wall_entry_slot.inc"
};
static NJS_TEXLIST outer_wall_entry_binding = {
#include "stg38/wall_placements/outer_wall_entry_binding.inc"
};
static Sint16 outer_wall_entry_panel_1_polygons[95] = {
#include "stg38/wall_placements/outer_wall_entry_panel_1_polygons.inc"
};
static Sint32 outer_wall_entry_panel_1_vertices[67] = {
#include "stg38/wall_placements/outer_wall_entry_panel_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_entry_panel_1_mesh = {
#include "stg38/wall_placements/outer_wall_entry_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_entry_panel_1 = {
#include "stg38/wall_placements/outer_wall_entry_panel_1.inc"
};
NJS_CNK_OBJECT outer_wall_entry = {
#include "stg38/wall_placements/outer_wall_entry.inc"
};
static NJS_TEXNAME outer_wall_shaft_slot[1] = {
#include "stg38/wall_placements/outer_wall_shaft_slot.inc"
};
static NJS_TEXLIST outer_wall_shaft_binding = {
#include "stg38/wall_placements/outer_wall_shaft_binding.inc"
};
static Sint16 outer_wall_shaft_panel_1_polygons[95] = {
#include "stg38/wall_placements/outer_wall_shaft_panel_1_polygons.inc"
};
static Sint32 outer_wall_shaft_panel_1_vertices[67] = {
#include "stg38/wall_placements/outer_wall_shaft_panel_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_shaft_panel_1_mesh = {
#include "stg38/wall_placements/outer_wall_shaft_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_shaft_panel_1 = {
#include "stg38/wall_placements/outer_wall_shaft_panel_1.inc"
};
NJS_CNK_OBJECT outer_wall_shaft = {
#include "stg38/wall_placements/outer_wall_shaft.inc"
};
static NJS_TEXNAME outer_wall_lower_slot[1] = {
#include "stg38/wall_placements/outer_wall_lower_slot.inc"
};
static NJS_TEXLIST outer_wall_lower_binding = {
#include "stg38/wall_placements/outer_wall_lower_binding.inc"
};
static Sint16 outer_wall_lower_panel_1_polygons[95] = {
#include "stg38/wall_placements/outer_wall_lower_panel_1_polygons.inc"
};
static Sint32 outer_wall_lower_panel_1_vertices[67] = {
#include "stg38/wall_placements/outer_wall_lower_panel_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_lower_panel_1_mesh = {
#include "stg38/wall_placements/outer_wall_lower_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_lower_panel_1 = {
#include "stg38/wall_placements/outer_wall_lower_panel_1.inc"
};
NJS_CNK_OBJECT outer_wall_lower = {
#include "stg38/wall_placements/outer_wall_lower.inc"
};
static NJS_TEXNAME outer_wall_channel_slot[1] = {
#include "stg38/wall_placements/outer_wall_channel_slot.inc"
};
static NJS_TEXLIST outer_wall_channel_binding = {
#include "stg38/wall_placements/outer_wall_channel_binding.inc"
};
static Sint16 outer_wall_channel_panel_1_polygons[95] = {
#include "stg38/wall_placements/outer_wall_channel_panel_1_polygons.inc"
};
static Sint32 outer_wall_channel_panel_1_vertices[67] = {
#include "stg38/wall_placements/outer_wall_channel_panel_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_channel_panel_1_mesh = {
#include "stg38/wall_placements/outer_wall_channel_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_channel_panel_1 = {
#include "stg38/wall_placements/outer_wall_channel_panel_1.inc"
};
NJS_CNK_OBJECT outer_wall_channel = {
#include "stg38/wall_placements/outer_wall_channel.inc"
};
static NJS_TEXNAME outer_wall_exit_slot[1] = {
#include "stg38/wall_placements/outer_wall_exit_slot.inc"
};
static NJS_TEXLIST outer_wall_exit_binding = {
#include "stg38/wall_placements/outer_wall_exit_binding.inc"
};
static Sint16 outer_wall_exit_panel_2_polygons[95] = {
#include "stg38/wall_placements/outer_wall_exit_panel_2_polygons.inc"
};
static Sint32 outer_wall_exit_panel_2_vertices[67] = {
#include "stg38/wall_placements/outer_wall_exit_panel_2_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_exit_panel_2_mesh = {
#include "stg38/wall_placements/outer_wall_exit_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_exit_panel_2 = {
#include "stg38/wall_placements/outer_wall_exit_panel_2.inc"
};
static Sint16 outer_wall_exit_panel_1_polygons[95] = {
#include "stg38/wall_placements/outer_wall_exit_panel_1_polygons.inc"
};
static Sint32 outer_wall_exit_panel_1_vertices[67] = {
#include "stg38/wall_placements/outer_wall_exit_panel_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_exit_panel_1_mesh = {
#include "stg38/wall_placements/outer_wall_exit_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT outer_wall_exit_panel_1 = {
#include "stg38/wall_placements/outer_wall_exit_panel_1.inc"
};
NJS_CNK_OBJECT outer_wall_exit = {
#include "stg38/wall_placements/outer_wall_exit.inc"
};
