/* CHAOSTGKINDER.PRS's texture lists, Basic models, chunk models and land,
 * from the disc. its D lies between dat_0ca8c350.c and dat_0caa7868.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/land_data.h"
#include "chaostgkinder/land_textures.h"
#include "collision/world.h"

static NJS_TEXNAME classroom_textures[25] = {
#include "chaostgkinder/dat_0ca984f4/classroom_textures.inc"
};
NJS_TEXLIST kinder_classroom_texlist = {
#include "chaostgkinder/dat_0ca984f4/kinder_classroom_texlist.inc"
};
static NJS_MATERIAL classroom_collision_0_materials[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_0_materials.inc"
};
static Sint16 classroom_collision_0_faces[40] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_0_faces.inc"
};
static NJS_MESHSET classroom_collision_0_meshsets[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_0_meshsets.inc"
};
static NJS_POINT3 classroom_collision_0_points[14] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_0_points.inc"
};
static NJS_MODEL classroom_pink_chair_collision_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_chair_collision_model.inc"
};
static NJS_OBJECT classroom_pink_chair_collision = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_chair_collision.inc"
};
static NJS_MATERIAL classroom_collision_1_materials[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_1_materials.inc"
};
static Sint16 classroom_collision_1_faces[40] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_1_faces.inc"
};
static NJS_MESHSET classroom_collision_1_meshsets[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_1_meshsets.inc"
};
static NJS_POINT3 classroom_collision_1_points[14] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_1_points.inc"
};
static NJS_MODEL classroom_teal_chair_collision_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_teal_chair_collision_model.inc"
};
static NJS_OBJECT classroom_teal_chair_collision = {
#include "chaostgkinder/dat_0ca984f4/classroom_teal_chair_collision.inc"
};
static NJS_MATERIAL classroom_collision_2_materials[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_2_materials.inc"
};
static Sint16 classroom_collision_2_faces[19] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_2_faces.inc"
};
static NJS_MESHSET classroom_collision_2_meshsets[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_2_meshsets.inc"
};
static NJS_POINT3 classroom_collision_2_points[8] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_2_points.inc"
};
static NJS_MODEL classroom_bench_collision_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_bench_collision_model.inc"
};
static NJS_OBJECT classroom_right_bench_collision = {
#include "chaostgkinder/dat_0ca984f4/classroom_right_bench_collision.inc"
};
static NJS_MATERIAL classroom_collision_4_materials[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_4_materials.inc"
};
static Sint16 classroom_collision_4_faces[19] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_4_faces.inc"
};
static NJS_MESHSET classroom_collision_4_meshsets[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_4_meshsets.inc"
};
static NJS_POINT3 classroom_collision_4_points[8] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_4_points.inc"
};
static NJS_MODEL classroom_left_bench_side_collision_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_bench_side_collision_model.inc"
};
static NJS_OBJECT classroom_left_bench_side_collision = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_bench_side_collision.inc"
};
static NJS_MATERIAL classroom_collision_5_materials[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_5_materials.inc"
};
static Sint16 classroom_collision_5_faces[19] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_5_faces.inc"
};
static NJS_MESHSET classroom_collision_5_meshsets[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_5_meshsets.inc"
};
static NJS_POINT3 classroom_collision_5_points[8] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_5_points.inc"
};
static NJS_MODEL classroom_bench_side_collision_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_bench_side_collision_model.inc"
};
static NJS_OBJECT classroom_right_bench_side_collision = {
#include "chaostgkinder/dat_0ca984f4/classroom_right_bench_side_collision.inc"
};
static NJS_MATERIAL classroom_collision_6_materials[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_6_materials.inc"
};
static Sint16 classroom_collision_6_faces[29] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_6_faces.inc"
};
static NJS_MESHSET classroom_collision_6_meshsets[1] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_6_meshsets.inc"
};
static NJS_POINT3 classroom_collision_6_points[12] = {
#include "chaostgkinder/dat_0ca984f4/classroom_collision_6_points.inc"
};
static NJS_MODEL classroom_room_collision_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_room_collision_model.inc"
};
static NJS_OBJECT classroom_room_collision = {
#include "chaostgkinder/dat_0ca984f4/classroom_room_collision.inc"
};
static Sint16 classroom_pink_chair_polygons[427] = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_chair_polygons.inc"
};
static Sint32 classroom_pink_chair_vertices[319] = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_chair_vertices.inc"
};
static NJS_CNK_MODEL classroom_pink_chair_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_chair_model.inc"
};
static NJS_CNK_OBJECT classroom_pink_chair = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_chair.inc"
};
static Sint16 classroom_teal_chair_polygons[435] = {
#include "chaostgkinder/dat_0ca984f4/classroom_teal_chair_polygons.inc"
};
static Sint32 classroom_teal_chair_vertices[251] = {
#include "chaostgkinder/dat_0ca984f4/classroom_teal_chair_vertices.inc"
};
static NJS_CNK_MODEL classroom_teal_chair_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_teal_chair_model.inc"
};
static NJS_CNK_OBJECT classroom_teal_chair = {
#include "chaostgkinder/dat_0ca984f4/classroom_teal_chair.inc"
};
static Sint16 classroom_bench_polygons[279] = {
#include "chaostgkinder/dat_0ca984f4/classroom_bench_polygons.inc"
};
static Sint32 classroom_bench_vertices[171] = {
#include "chaostgkinder/dat_0ca984f4/classroom_bench_vertices.inc"
};
static NJS_CNK_MODEL classroom_bench_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_bench_model.inc"
};
static NJS_CNK_OBJECT classroom_right_bench = {
#include "chaostgkinder/dat_0ca984f4/classroom_right_bench.inc"
};
static Sint16 classroom_floor_surface_polygons[123] = {
#include "chaostgkinder/dat_0ca984f4/classroom_floor_surface_polygons.inc"
};
static Sint32 classroom_floor_surface_vertices[99] = {
#include "chaostgkinder/dat_0ca984f4/classroom_floor_surface_vertices.inc"
};
static NJS_CNK_MODEL classroom_floor_surface_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_floor_surface_model.inc"
};
static NJS_CNK_OBJECT classroom_floor_surface = {
#include "chaostgkinder/dat_0ca984f4/classroom_floor_surface.inc"
};
static Sint16 classroom_blue_steps_polygons[225] = {
#include "chaostgkinder/dat_0ca984f4/classroom_blue_steps_polygons.inc"
};
static Sint32 classroom_blue_steps_vertices[171] = {
#include "chaostgkinder/dat_0ca984f4/classroom_blue_steps_vertices.inc"
};
static NJS_CNK_MODEL classroom_blue_steps_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_blue_steps_model.inc"
};
static NJS_CNK_OBJECT classroom_blue_steps = {
#include "chaostgkinder/dat_0ca984f4/classroom_blue_steps.inc"
};
static Sint16 classroom_yellow_steps_polygons[231] = {
#include "chaostgkinder/dat_0ca984f4/classroom_yellow_steps_polygons.inc"
};
static Sint32 classroom_yellow_steps_vertices[167] = {
#include "chaostgkinder/dat_0ca984f4/classroom_yellow_steps_vertices.inc"
};
static NJS_CNK_MODEL classroom_yellow_steps_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_yellow_steps_model.inc"
};
static NJS_CNK_OBJECT classroom_yellow_steps = {
#include "chaostgkinder/dat_0ca984f4/classroom_yellow_steps.inc"
};
static Sint16 classroom_pine_tree_polygons[685] = {
#include "chaostgkinder/dat_0ca984f4/classroom_pine_tree_polygons.inc"
};
static Sint32 classroom_pine_tree_vertices[539] = {
#include "chaostgkinder/dat_0ca984f4/classroom_pine_tree_vertices.inc"
};
static NJS_CNK_MODEL classroom_pine_tree_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_pine_tree_model.inc"
};
static NJS_CNK_OBJECT classroom_pine_tree = {
#include "chaostgkinder/dat_0ca984f4/classroom_pine_tree.inc"
};
static Sint16 classroom_chao_letters_polygons[1151] = {
#include "chaostgkinder/dat_0ca984f4/classroom_chao_letters_polygons.inc"
};
static Sint32 classroom_chao_letters_vertices[1267] = {
#include "chaostgkinder/dat_0ca984f4/classroom_chao_letters_vertices.inc"
};
static NJS_CNK_MODEL classroom_chao_letters_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_chao_letters_model.inc"
};
static NJS_CNK_OBJECT classroom_chao_letters = {
#include "chaostgkinder/dat_0ca984f4/classroom_chao_letters.inc"
};
static Sint16 classroom_round_tree_polygons[645] = {
#include "chaostgkinder/dat_0ca984f4/classroom_round_tree_polygons.inc"
};
static Sint32 classroom_round_tree_vertices[503] = {
#include "chaostgkinder/dat_0ca984f4/classroom_round_tree_vertices.inc"
};
static NJS_CNK_MODEL classroom_round_tree_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_round_tree_model.inc"
};
static NJS_CNK_OBJECT classroom_round_tree = {
#include "chaostgkinder/dat_0ca984f4/classroom_round_tree.inc"
};
static Sint16 classroom_purple_car_polygons[1007] = {
#include "chaostgkinder/dat_0ca984f4/classroom_purple_car_polygons.inc"
};
static Sint32 classroom_purple_car_vertices[631] = {
#include "chaostgkinder/dat_0ca984f4/classroom_purple_car_vertices.inc"
};
static NJS_CNK_MODEL classroom_purple_car_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_purple_car_model.inc"
};
static NJS_CNK_OBJECT classroom_purple_car = {
#include "chaostgkinder/dat_0ca984f4/classroom_purple_car.inc"
};
static Sint16 classroom_pink_car_polygons[943] = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_car_polygons.inc"
};
static Sint32 classroom_pink_car_vertices[603] = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_car_vertices.inc"
};
static NJS_CNK_MODEL classroom_pink_car_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_car_model.inc"
};
static NJS_CNK_OBJECT classroom_pink_car = {
#include "chaostgkinder/dat_0ca984f4/classroom_pink_car.inc"
};
static Sint16 classroom_back_blocks_polygons[3195] = {
#include "chaostgkinder/dat_0ca984f4/classroom_back_blocks_polygons.inc"
};
static Sint32 classroom_back_blocks_vertices[2387] = {
#include "chaostgkinder/dat_0ca984f4/classroom_back_blocks_vertices.inc"
};
static NJS_CNK_MODEL classroom_back_blocks_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_back_blocks_model.inc"
};
static NJS_CNK_OBJECT classroom_back_blocks = {
#include "chaostgkinder/dat_0ca984f4/classroom_back_blocks.inc"
};
static Sint16 classroom_left_blocks_polygons[789] = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_blocks_polygons.inc"
};
static Sint32 classroom_left_blocks_vertices[587] = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_blocks_vertices.inc"
};
static NJS_CNK_MODEL classroom_left_blocks_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_blocks_model.inc"
};
static NJS_CNK_OBJECT classroom_left_blocks = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_blocks.inc"
};
static Sint16 classroom_right_blocks_polygons[683] = {
#include "chaostgkinder/dat_0ca984f4/classroom_right_blocks_polygons.inc"
};
static Sint32 classroom_right_blocks_vertices[451] = {
#include "chaostgkinder/dat_0ca984f4/classroom_right_blocks_vertices.inc"
};
static NJS_CNK_MODEL classroom_right_blocks_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_right_blocks_model.inc"
};
static NJS_CNK_OBJECT classroom_right_blocks = {
#include "chaostgkinder/dat_0ca984f4/classroom_right_blocks.inc"
};
static Sint16 classroom_sky_polygons[733] = {
#include "chaostgkinder/dat_0ca984f4/classroom_sky_polygons.inc"
};
static Sint32 classroom_sky_vertices[591] = {
#include "chaostgkinder/dat_0ca984f4/classroom_sky_vertices.inc"
};
static NJS_CNK_MODEL classroom_sky_model = {
#include "chaostgkinder/dat_0ca984f4/classroom_sky_model.inc"
};
static NJS_CNK_OBJECT classroom_sky = {
#include "chaostgkinder/dat_0ca984f4/classroom_sky.inc"
};
static NJS_OBJECT classroom_left_bench_collision = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_bench_collision.inc"
};
static NJS_CNK_OBJECT classroom_left_bench = {
#include "chaostgkinder/dat_0ca984f4/classroom_left_bench.inc"
};
struct land_entry kinder_classroom_entries[23] = {
#include "chaostgkinder/dat_0ca984f4/kinder_classroom_entries.inc"
};
struct land_table kinder_classroom_land = {
#include "chaostgkinder/dat_0ca984f4/kinder_classroom_land.inc"
};
