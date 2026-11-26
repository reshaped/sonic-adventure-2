#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_scenery_art.h"
#include "shared/water_shaft/textures.h"
#include "stg38/water_scenery.h"

/* the texture name of the poison's two surfaces */
const char knuckles_poison_frame_name[15] = {
#include "stg38/poison_scenery/knuckles_poison_frame_name.inc"
};

/* STG38.PRS's texture lists and chunk models, from the disc. its D lies
 * between core_shaft_rotor.c and wall_placements.c */
static NJS_TEXNAME lower_poison_textures_slot[1] = {
#include "stg38/poison_scenery/lower_poison_textures_slot.inc"
};
static NJS_TEXLIST lower_poison_textures = {
#include "stg38/poison_scenery/lower_poison_textures.inc"
};
static Sint16 lower_poison_panel_2_polygons[37] = {
#include "stg38/poison_scenery/lower_poison_panel_2_polygons.inc"
};
static Sint32 lower_poison_panel_2_vertices[35] = {
#include "stg38/poison_scenery/lower_poison_panel_2_vertices.inc"
};
static NJS_CNK_MODEL lower_poison_panel_2_mesh = {
#include "stg38/poison_scenery/lower_poison_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT lower_poison_panel_2 = {
#include "stg38/poison_scenery/lower_poison_panel_2.inc"
};
static Sint16 lower_poison_panel_1_polygons[37] = {
#include "stg38/poison_scenery/lower_poison_panel_1_polygons.inc"
};
static Sint32 lower_poison_panel_1_vertices[35] = {
#include "stg38/poison_scenery/lower_poison_panel_1_vertices.inc"
};
static NJS_CNK_MODEL lower_poison_panel_1_mesh = {
#include "stg38/poison_scenery/lower_poison_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT lower_poison_panel_1 = {
#include "stg38/poison_scenery/lower_poison_panel_1.inc"
};
NJS_CNK_OBJECT lower_poison = {
#include "stg38/poison_scenery/lower_poison.inc"
};
static NJS_TEXNAME upper_poison_textures_slot[1] = {
#include "stg38/poison_scenery/upper_poison_textures_slot.inc"
};
static NJS_TEXLIST upper_poison_textures = {
#include "stg38/poison_scenery/upper_poison_textures.inc"
};
static Sint16 upper_poison_panel_2_polygons[37] = {
#include "stg38/poison_scenery/upper_poison_panel_2_polygons.inc"
};
static Sint32 upper_poison_panel_2_vertices[35] = {
#include "stg38/poison_scenery/upper_poison_panel_2_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_panel_2_mesh = {
#include "stg38/poison_scenery/upper_poison_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_panel_2 = {
#include "stg38/poison_scenery/upper_poison_panel_2.inc"
};
static Sint16 upper_poison_panel_1_polygons[37] = {
#include "stg38/poison_scenery/upper_poison_panel_1_polygons.inc"
};
static Sint32 upper_poison_panel_1_vertices[35] = {
#include "stg38/poison_scenery/upper_poison_panel_1_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_panel_1_mesh = {
#include "stg38/poison_scenery/upper_poison_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_panel_1 = {
#include "stg38/poison_scenery/upper_poison_panel_1.inc"
};
NJS_CNK_OBJECT upper_poison = {
#include "stg38/poison_scenery/upper_poison.inc"
};
static NJS_TEXNAME poison_wall_textures_slot[1] = {
#include "stg38/poison_scenery/poison_wall_textures_slot.inc"
};
NJS_TEXLIST poison_wall_textures = {
#include "stg38/poison_scenery/poison_wall_textures.inc"
};
static Sint16 lower_poison_wall_panel_4_polygons[169] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_4_polygons.inc"
};
static Sint32 lower_poison_wall_panel_4_vertices[123] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_4_vertices.inc"
};
static NJS_CNK_MODEL lower_poison_wall_panel_4_mesh = {
#include "stg38/poison_scenery/lower_poison_wall_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT lower_poison_wall_panel_4 = {
#include "stg38/poison_scenery/lower_poison_wall_panel_4.inc"
};
static Sint16 lower_poison_wall_panel_3_polygons[169] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_3_polygons.inc"
};
static Sint32 lower_poison_wall_panel_3_vertices[123] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_3_vertices.inc"
};
static NJS_CNK_MODEL lower_poison_wall_panel_3_mesh = {
#include "stg38/poison_scenery/lower_poison_wall_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT lower_poison_wall_panel_3 = {
#include "stg38/poison_scenery/lower_poison_wall_panel_3.inc"
};
static Sint16 lower_poison_wall_panel_2_polygons[169] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_2_polygons.inc"
};
static Sint32 lower_poison_wall_panel_2_vertices[123] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL lower_poison_wall_panel_2_mesh = {
#include "stg38/poison_scenery/lower_poison_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT lower_poison_wall_panel_2 = {
#include "stg38/poison_scenery/lower_poison_wall_panel_2.inc"
};
static Sint16 lower_poison_wall_panel_1_polygons[169] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_1_polygons.inc"
};
static Sint32 lower_poison_wall_panel_1_vertices[123] = {
#include "stg38/poison_scenery/lower_poison_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL lower_poison_wall_panel_1_mesh = {
#include "stg38/poison_scenery/lower_poison_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT lower_poison_wall_panel_1 = {
#include "stg38/poison_scenery/lower_poison_wall_panel_1.inc"
};
NJS_CNK_OBJECT lower_poison_walls = {
#include "stg38/poison_scenery/lower_poison_walls.inc"
};
static NJS_TEXNAME upper_poison_walls_textures_slot[1] = {
#include "stg38/poison_scenery/upper_poison_walls_textures_slot.inc"
};
static NJS_TEXLIST upper_poison_walls_textures = {
#include "stg38/poison_scenery/upper_poison_walls_textures.inc"
};
static Sint16 upper_poison_wall_panel_6_polygons[169] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_6_polygons.inc"
};
static Sint32 upper_poison_wall_panel_6_vertices[123] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_6_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_wall_panel_6_mesh = {
#include "stg38/poison_scenery/upper_poison_wall_panel_6_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_wall_panel_6 = {
#include "stg38/poison_scenery/upper_poison_wall_panel_6.inc"
};
static Sint16 upper_poison_wall_panel_5_polygons[169] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_5_polygons.inc"
};
static Sint32 upper_poison_wall_panel_5_vertices[123] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_5_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_wall_panel_5_mesh = {
#include "stg38/poison_scenery/upper_poison_wall_panel_5_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_wall_panel_5 = {
#include "stg38/poison_scenery/upper_poison_wall_panel_5.inc"
};
static Sint16 upper_poison_wall_panel_4_polygons[169] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_4_polygons.inc"
};
static Sint32 upper_poison_wall_panel_4_vertices[123] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_4_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_wall_panel_4_mesh = {
#include "stg38/poison_scenery/upper_poison_wall_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_wall_panel_4 = {
#include "stg38/poison_scenery/upper_poison_wall_panel_4.inc"
};
static Sint16 upper_poison_wall_panel_3_polygons[169] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_3_polygons.inc"
};
static Sint32 upper_poison_wall_panel_3_vertices[123] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_3_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_wall_panel_3_mesh = {
#include "stg38/poison_scenery/upper_poison_wall_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_wall_panel_3 = {
#include "stg38/poison_scenery/upper_poison_wall_panel_3.inc"
};
static Sint16 upper_poison_wall_panel_2_polygons[169] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_2_polygons.inc"
};
static Sint32 upper_poison_wall_panel_2_vertices[123] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_wall_panel_2_mesh = {
#include "stg38/poison_scenery/upper_poison_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_wall_panel_2 = {
#include "stg38/poison_scenery/upper_poison_wall_panel_2.inc"
};
static Sint16 upper_poison_wall_panel_1_polygons[169] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_1_polygons.inc"
};
static Sint32 upper_poison_wall_panel_1_vertices[123] = {
#include "stg38/poison_scenery/upper_poison_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL upper_poison_wall_panel_1_mesh = {
#include "stg38/poison_scenery/upper_poison_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT upper_poison_wall_panel_1 = {
#include "stg38/poison_scenery/upper_poison_wall_panel_1.inc"
};
NJS_CNK_OBJECT upper_poison_walls = {
#include "stg38/poison_scenery/upper_poison_walls.inc"
};
