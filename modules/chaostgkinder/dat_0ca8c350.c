/* CHAOSTGKINDER.PRS's texture lists, Basic models, chunk models and land,
 * from the disc. its D lies between terrain.c and dat_0ca984f4.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/land_data.h"
#include "chaostgkinder/land_textures.h"
#include "collision/world.h"

static NJS_TEXNAME corridor_textures[46] = {
#include "chaostgkinder/dat_0ca8c350/corridor_textures.inc"
};
NJS_TEXLIST kinder_corridor_texlist = {
#include "chaostgkinder/dat_0ca8c350/kinder_corridor_texlist.inc"
};
static NJS_MATERIAL corridor_collision_0_materials[1] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_0_materials.inc"
};
static Sint16 corridor_collision_0_faces[12] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_0_faces.inc"
};
static NJS_MESHSET corridor_collision_0_meshsets[1] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_0_meshsets.inc"
};
static NJS_POINT3 corridor_collision_0_points[7] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_0_points.inc"
};
static NJS_MODEL corridor_table_collision_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_collision_model.inc"
};
static NJS_OBJECT corridor_table_collision = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_collision.inc"
};
static NJS_MATERIAL corridor_collision_1_materials[1] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_1_materials.inc"
};
static Sint16 corridor_collision_1_faces[128] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_1_faces.inc"
};
static NJS_MESHSET corridor_collision_1_meshsets[1] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_1_meshsets.inc"
};
static NJS_POINT3 corridor_collision_1_points[64] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_1_points.inc"
};
static NJS_MODEL corridor_floor_and_ceiling_collision_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_floor_and_ceiling_collision_model.inc"
};
static NJS_OBJECT corridor_floor_and_ceiling_collision = {
#include "chaostgkinder/dat_0ca8c350/corridor_floor_and_ceiling_collision.inc"
};
static NJS_MATERIAL corridor_collision_2_materials[1] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_2_materials.inc"
};
static Sint16 corridor_collision_2_faces[38] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_2_faces.inc"
};
static NJS_MESHSET corridor_collision_2_meshsets[1] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_2_meshsets.inc"
};
static NJS_POINT3 corridor_collision_2_points[32] = {
#include "chaostgkinder/dat_0ca8c350/corridor_collision_2_points.inc"
};
static NJS_MODEL corridor_perimeter_collision_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_perimeter_collision_model.inc"
};
static NJS_OBJECT corridor_perimeter_collision = {
#include "chaostgkinder/dat_0ca8c350/corridor_perimeter_collision.inc"
};
static Sint16 corridor_ceiling_polygons[831] = {
#include "chaostgkinder/dat_0ca8c350/corridor_ceiling_polygons.inc"
};
static Sint32 corridor_ceiling_vertices[687] = {
#include "chaostgkinder/dat_0ca8c350/corridor_ceiling_vertices.inc"
};
static NJS_CNK_MODEL corridor_ceiling_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_ceiling_model.inc"
};
static NJS_CNK_OBJECT corridor_ceiling = {
#include "chaostgkinder/dat_0ca8c350/corridor_ceiling.inc"
};
static Sint16 corridor_classroom_sign_polygons[31] = {
#include "chaostgkinder/dat_0ca8c350/corridor_classroom_sign_polygons.inc"
};
static Sint32 corridor_classroom_sign_vertices[27] = {
#include "chaostgkinder/dat_0ca8c350/corridor_classroom_sign_vertices.inc"
};
static NJS_CNK_MODEL corridor_classroom_sign_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_classroom_sign_model.inc"
};
static NJS_CNK_OBJECT corridor_classroom_sign = {
#include "chaostgkinder/dat_0ca8c350/corridor_classroom_sign.inc"
};
static Sint16 corridor_exit_sign_polygons[31] = {
#include "chaostgkinder/dat_0ca8c350/corridor_exit_sign_polygons.inc"
};
static Sint32 corridor_exit_sign_vertices[27] = {
#include "chaostgkinder/dat_0ca8c350/corridor_exit_sign_vertices.inc"
};
static NJS_CNK_MODEL corridor_exit_sign_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_exit_sign_model.inc"
};
static NJS_CNK_OBJECT corridor_exit_sign = {
#include "chaostgkinder/dat_0ca8c350/corridor_exit_sign.inc"
};
static Sint16 corridor_floor_and_carpet_polygons[579] = {
#include "chaostgkinder/dat_0ca8c350/corridor_floor_and_carpet_polygons.inc"
};
static Sint32 corridor_floor_and_carpet_vertices[463] = {
#include "chaostgkinder/dat_0ca8c350/corridor_floor_and_carpet_vertices.inc"
};
static NJS_CNK_MODEL corridor_floor_and_carpet_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_floor_and_carpet_model.inc"
};
static NJS_CNK_OBJECT corridor_floor_and_carpet = {
#include "chaostgkinder/dat_0ca8c350/corridor_floor_and_carpet.inc"
};
static Sint16 corridor_table_flowers_polygons[1117] = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_flowers_polygons.inc"
};
static Sint32 corridor_table_flowers_vertices[795] = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_flowers_vertices.inc"
};
static NJS_CNK_MODEL corridor_table_flowers_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_flowers_model.inc"
};
static NJS_CNK_OBJECT corridor_table_flowers = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_flowers.inc"
};
static Sint16 corridor_health_center_sign_polygons[31] = {
#include "chaostgkinder/dat_0ca8c350/corridor_health_center_sign_polygons.inc"
};
static Sint32 corridor_health_center_sign_vertices[27] = {
#include "chaostgkinder/dat_0ca8c350/corridor_health_center_sign_vertices.inc"
};
static NJS_CNK_MODEL corridor_health_center_sign_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_health_center_sign_model.inc"
};
static NJS_CNK_OBJECT corridor_health_center_sign = {
#include "chaostgkinder/dat_0ca8c350/corridor_health_center_sign.inc"
};
static Sint16 corridor_east_ceiling_lamp_polygons[309] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_ceiling_lamp_polygons.inc"
};
static Sint32 corridor_east_ceiling_lamp_vertices[167] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_ceiling_lamp_vertices.inc"
};
static NJS_CNK_MODEL corridor_east_ceiling_lamp_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_ceiling_lamp_model.inc"
};
static NJS_CNK_OBJECT corridor_east_ceiling_lamp = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_ceiling_lamp.inc"
};
static Sint16 corridor_table_display_polygons[223] = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_display_polygons.inc"
};
static Sint32 corridor_table_display_vertices[159] = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_display_vertices.inc"
};
static NJS_CNK_MODEL corridor_table_display_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_display_model.inc"
};
static NJS_CNK_OBJECT corridor_table_display = {
#include "chaostgkinder/dat_0ca8c350/corridor_table_display.inc"
};
static Sint16 corridor_notice_board_polygons[255] = {
#include "chaostgkinder/dat_0ca8c350/corridor_notice_board_polygons.inc"
};
static Sint32 corridor_notice_board_vertices[219] = {
#include "chaostgkinder/dat_0ca8c350/corridor_notice_board_vertices.inc"
};
static NJS_CNK_MODEL corridor_notice_board_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_notice_board_model.inc"
};
static NJS_CNK_OBJECT corridor_notice_board = {
#include "chaostgkinder/dat_0ca8c350/corridor_notice_board.inc"
};
static Sint16 corridor_framed_photograph_polygons[239] = {
#include "chaostgkinder/dat_0ca8c350/corridor_framed_photograph_polygons.inc"
};
static Sint32 corridor_framed_photograph_vertices[227] = {
#include "chaostgkinder/dat_0ca8c350/corridor_framed_photograph_vertices.inc"
};
static NJS_CNK_MODEL corridor_framed_photograph_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_framed_photograph_model.inc"
};
static NJS_CNK_OBJECT corridor_framed_photograph = {
#include "chaostgkinder/dat_0ca8c350/corridor_framed_photograph.inc"
};
static Sint16 corridor_gallery_pictures_polygons[383] = {
#include "chaostgkinder/dat_0ca8c350/corridor_gallery_pictures_polygons.inc"
};
static Sint32 corridor_gallery_pictures_vertices[355] = {
#include "chaostgkinder/dat_0ca8c350/corridor_gallery_pictures_vertices.inc"
};
static NJS_CNK_MODEL corridor_gallery_pictures_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_gallery_pictures_model.inc"
};
static NJS_CNK_OBJECT corridor_gallery_pictures = {
#include "chaostgkinder/dat_0ca8c350/corridor_gallery_pictures.inc"
};
static Sint16 corridor_principal_sign_polygons[31] = {
#include "chaostgkinder/dat_0ca8c350/corridor_principal_sign_polygons.inc"
};
static Sint32 corridor_principal_sign_vertices[27] = {
#include "chaostgkinder/dat_0ca8c350/corridor_principal_sign_vertices.inc"
};
static NJS_CNK_MODEL corridor_principal_sign_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_principal_sign_model.inc"
};
static NJS_CNK_OBJECT corridor_principal_sign = {
#include "chaostgkinder/dat_0ca8c350/corridor_principal_sign.inc"
};
static Sint16 corridor_sky_polygons[25] = {
#include "chaostgkinder/dat_0ca8c350/corridor_sky_polygons.inc"
};
static Sint32 corridor_sky_vertices[19] = {
#include "chaostgkinder/dat_0ca8c350/corridor_sky_vertices.inc"
};
static NJS_CNK_MODEL corridor_sky_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_sky_model.inc"
};
static NJS_CNK_OBJECT corridor_sky = {
#include "chaostgkinder/dat_0ca8c350/corridor_sky.inc"
};
static Sint16 corridor_center_table_polygons[245] = {
#include "chaostgkinder/dat_0ca8c350/corridor_center_table_polygons.inc"
};
static Sint32 corridor_center_table_vertices[175] = {
#include "chaostgkinder/dat_0ca8c350/corridor_center_table_vertices.inc"
};
static NJS_CNK_MODEL corridor_center_table_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_center_table_model.inc"
};
static NJS_CNK_OBJECT corridor_center_table = {
#include "chaostgkinder/dat_0ca8c350/corridor_center_table.inc"
};
static Sint16 corridor_south_wall_polygons[1585] = {
#include "chaostgkinder/dat_0ca8c350/corridor_south_wall_polygons.inc"
};
static Sint32 corridor_south_wall_vertices[1371] = {
#include "chaostgkinder/dat_0ca8c350/corridor_south_wall_vertices.inc"
};
static NJS_CNK_MODEL corridor_south_wall_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_south_wall_model.inc"
};
static NJS_CNK_OBJECT corridor_south_wall = {
#include "chaostgkinder/dat_0ca8c350/corridor_south_wall.inc"
};
static Sint16 corridor_west_wall_polygons[979] = {
#include "chaostgkinder/dat_0ca8c350/corridor_west_wall_polygons.inc"
};
static Sint32 corridor_west_wall_vertices[727] = {
#include "chaostgkinder/dat_0ca8c350/corridor_west_wall_vertices.inc"
};
static NJS_CNK_MODEL corridor_west_wall_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_west_wall_model.inc"
};
static NJS_CNK_OBJECT corridor_west_wall = {
#include "chaostgkinder/dat_0ca8c350/corridor_west_wall.inc"
};
static Sint16 corridor_east_wall_polygons[1139] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_wall_polygons.inc"
};
static Sint32 corridor_east_wall_vertices[847] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_wall_vertices.inc"
};
static NJS_CNK_MODEL corridor_east_wall_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_wall_model.inc"
};
static NJS_CNK_OBJECT corridor_east_wall = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_wall.inc"
};
static Sint16 corridor_east_window_light_polygons[19] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window_light_polygons.inc"
};
static Sint32 corridor_east_window_light_vertices[27] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window_light_vertices.inc"
};
static NJS_CNK_MODEL corridor_east_window_light_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window_light_model.inc"
};
static NJS_CNK_OBJECT corridor_east_window_light = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window_light.inc"
};
static Sint16 corridor_east_window_polygons[483] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window_polygons.inc"
};
static Sint32 corridor_east_window_vertices[459] = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window_vertices.inc"
};
static NJS_CNK_MODEL corridor_east_window_model = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window_model.inc"
};
static NJS_CNK_OBJECT corridor_east_window = {
#include "chaostgkinder/dat_0ca8c350/corridor_east_window.inc"
};
static NJS_CNK_OBJECT corridor_northeast_ceiling_lamp = {
#include "chaostgkinder/dat_0ca8c350/corridor_northeast_ceiling_lamp.inc"
};
static NJS_CNK_OBJECT corridor_north_ceiling_lamp = {
#include "chaostgkinder/dat_0ca8c350/corridor_north_ceiling_lamp.inc"
};
static NJS_CNK_OBJECT corridor_northwest_ceiling_lamp = {
#include "chaostgkinder/dat_0ca8c350/corridor_northwest_ceiling_lamp.inc"
};
static NJS_CNK_OBJECT corridor_west_ceiling_lamp = {
#include "chaostgkinder/dat_0ca8c350/corridor_west_ceiling_lamp.inc"
};
static NJS_CNK_OBJECT corridor_west_window_light = {
#include "chaostgkinder/dat_0ca8c350/corridor_west_window_light.inc"
};
static NJS_CNK_OBJECT corridor_west_window = {
#include "chaostgkinder/dat_0ca8c350/corridor_west_window.inc"
};
struct land_entry kinder_corridor_entries[28] = {
#include "chaostgkinder/dat_0ca8c350/kinder_corridor_entries.inc"
};
struct land_table kinder_corridor_land = {
#include "chaostgkinder/dat_0ca8c350/kinder_corridor_land.inc"
};
