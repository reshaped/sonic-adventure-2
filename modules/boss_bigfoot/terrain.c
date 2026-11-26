/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigfoot/model_art.h"
#include "boss_bigfoot/motion_art.h"
#include "boss_bigfoot/terrain.h"
#include "collision/world.h"

const char bigfoot_land_filenames[659] = {
#include "boss_bigfoot/terrain/bigfoot_land_filenames.inc"
};

/* BOSS_BIGFOOT.PRS's land, texture lists, Basic models, chunk models and
 * motions, from the disc. its D lies between stage.c and bigfoot.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[67];
struct land_table bigfoot_land;
static NJS_TEXNAME textures[53];
static NJS_TEXLIST terrain_textures;
static NJS_MATERIAL boundary_materials[1];
static Sint16 boundary_faces[54];
static NJS_MESHSET boundary_meshsets[1];
static NJS_POINT3 boundary_points[36];
static NJS_MODEL boundary_model;
static NJS_OBJECT boundary;
static NJS_MATERIAL floor_collision_materials[1];
static Sint16 floor_collision_faces[118];
static NJS_MESHSET floor_collision_meshsets[1];
static NJS_POINT3 floor_collision_points[48];
static NJS_MODEL floor_collision_model;
static NJS_OBJECT floor_collision;
static Sint16 street_front_1_polygons[265];
static Sint32 street_front_1_vertices[195];
static NJS_CNK_MODEL street_front_1_model;
static NJS_CNK_OBJECT street_front_1;
static Sint16 walkway_1_polygons[33];
static Sint32 walkway_1_vertices[35];
static NJS_CNK_MODEL walkway_1_model;
static NJS_CNK_OBJECT walkway_1;
static Sint16 side_front_1_polygons[73];
static Sint32 side_front_1_vertices[51];
static NJS_CNK_MODEL side_front_1_model;
static NJS_CNK_OBJECT side_front_1;
static Sint16 corner_paving_1_polygons[55];
static Sint32 corner_paving_1_vertices[67];
static NJS_CNK_MODEL corner_paving_1_model;
static NJS_CNK_OBJECT corner_paving_1;
static Sint16 road_stripe_1_polygons[19];
static Sint32 road_stripe_1_vertices[19];
static NJS_CNK_MODEL road_stripe_1_model;
static NJS_CNK_OBJECT road_stripe_1;
static Sint16 corner_shop_1_polygons[191];
static Sint32 corner_shop_1_vertices[127];
static NJS_CNK_MODEL corner_shop_1_model;
static NJS_CNK_OBJECT corner_shop_1;
static Sint16 crossing_1_polygons[207];
static Sint32 crossing_1_vertices[111];
static NJS_CNK_MODEL crossing_1_model;
static NJS_CNK_OBJECT crossing_1;
static Sint16 shopfront_1_polygons[89];
static Sint32 shopfront_1_vertices[59];
static NJS_CNK_MODEL shopfront_1_model;
static NJS_CNK_OBJECT shopfront_1;
static Sint16 storefront_1_polygons[89];
static Sint32 storefront_1_vertices[59];
static NJS_CNK_MODEL storefront_1_model;
static NJS_CNK_OBJECT storefront_1;
static Sint16 junction_polygons[109];
static Sint32 junction_vertices[131];
static NJS_CNK_MODEL junction_model;
static NJS_CNK_OBJECT junction;
static Sint16 street_polygons[19];
static Sint32 street_vertices[19];
static NJS_CNK_MODEL street_model;
static NJS_CNK_OBJECT street;
static Sint16 roadbed_1_polygons[19];
static Sint32 roadbed_1_vertices[19];
static NJS_CNK_MODEL roadbed_1_model;
static NJS_CNK_OBJECT roadbed_1;
static Sint16 corner_marking_1_polygons[43];
static Sint32 corner_marking_1_vertices[31];
static NJS_CNK_MODEL corner_marking_1_model;
static NJS_CNK_OBJECT corner_marking_1;
static Sint16 corner_curb_1_polygons[81];
static Sint32 corner_curb_1_vertices[75];
static NJS_CNK_MODEL corner_curb_1_model;
static NJS_CNK_OBJECT corner_curb_1;
static Sint16 shops_1_polygons[93];
static Sint32 shops_1_vertices[83];
static NJS_CNK_MODEL shops_1_model;
static NJS_CNK_OBJECT shops_1;
static Sint16 side_facade_1_polygons[217];
static Sint32 side_facade_1_vertices[127];
static NJS_CNK_MODEL side_facade_1_model;
static NJS_CNK_OBJECT side_facade_1;
static Sint16 end_facade_1_polygons[241];
static Sint32 end_facade_1_vertices[183];
static NJS_CNK_MODEL end_facade_1_model;
static NJS_CNK_OBJECT end_facade_1;
static Sint16 side_towers_1_polygons[543];
static Sint32 side_towers_1_vertices[399];
static NJS_CNK_MODEL side_towers_1_model;
static NJS_CNK_OBJECT side_towers_1;
static Sint16 end_towers_1_polygons[515];
static Sint32 end_towers_1_vertices[447];
static NJS_CNK_MODEL end_towers_1_model;
static NJS_CNK_OBJECT end_towers_1;
static Sint16 outer_street_1_polygons[151];
static Sint32 outer_street_1_vertices[103];
static NJS_CNK_MODEL outer_street_1_model;
static NJS_CNK_OBJECT outer_street_1;
static Sint16 side_streets_1_polygons[197];
static Sint32 side_streets_1_vertices[139];
static NJS_CNK_MODEL side_streets_1_model;
static NJS_CNK_OBJECT side_streets_1;
static Sint16 sidewalk_1_polygons[33];
static Sint32 sidewalk_1_vertices[27];
static NJS_CNK_MODEL sidewalk_1_model;
static NJS_CNK_OBJECT sidewalk_1;
static Sint16 corner_towers_1_polygons[1413];
static Sint32 corner_towers_1_vertices[1235];
static NJS_CNK_MODEL corner_towers_1_model;
static NJS_CNK_OBJECT corner_towers_1;
static Sint16 road_marking_1_polygons[93];
static Sint32 road_marking_1_vertices[115];
static NJS_CNK_MODEL road_marking_1_model;
static NJS_CNK_OBJECT road_marking_1;
static Sint16 city_blocks_1_polygons[1245];
static Sint32 city_blocks_1_vertices[1247];
static NJS_CNK_MODEL city_blocks_1_model;
static NJS_CNK_OBJECT city_blocks_1;
static Sint16 lane_marking_1_polygons[33];
static Sint32 lane_marking_1_vertices[35];
static NJS_CNK_MODEL lane_marking_1_model;
static NJS_CNK_OBJECT lane_marking_1;
static Sint16 sign_line_1_polygons[121];
static Sint32 sign_line_1_vertices[67];
static NJS_CNK_MODEL sign_line_1_model;
static NJS_CNK_OBJECT sign_line_1;
static Sint16 upper_sign_line_1_polygons[107];
static Sint32 upper_sign_line_1_vertices[67];
static NJS_CNK_MODEL upper_sign_line_1_model;
static NJS_CNK_OBJECT upper_sign_line_1;
static Sint16 side_front_end_polygons[73];
static Sint32 side_front_end_vertices[51];
static NJS_CNK_MODEL side_front_end_model;
static NJS_CNK_OBJECT side_front_end;
static Sint16 skyline_polygons[907];
static Sint32 skyline_vertices[459];
static NJS_CNK_MODEL skyline_model;
static NJS_CNK_OBJECT skyline;
static NJS_CNK_OBJECT street_front_2;
static NJS_CNK_OBJECT walkway_2;
static NJS_CNK_OBJECT walkway_3;
static NJS_CNK_OBJECT walkway_4;
static NJS_CNK_OBJECT side_front_2;
static NJS_CNK_OBJECT side_front_3;
static NJS_CNK_OBJECT corner_paving_2;
static NJS_CNK_OBJECT road_stripe_2;
static NJS_CNK_OBJECT road_stripe_3;
static NJS_CNK_OBJECT road_stripe_4;
static NJS_CNK_OBJECT corner_shop_2;
static NJS_CNK_OBJECT crossing_2;
static NJS_CNK_OBJECT shopfront_2;
static NJS_CNK_OBJECT storefront_2;
static NJS_CNK_OBJECT roadbed_2;
static NJS_CNK_OBJECT corner_marking_2;
static NJS_CNK_OBJECT corner_curb_2;
static NJS_CNK_OBJECT shops_2;
static NJS_CNK_OBJECT side_facade_2;
static NJS_CNK_OBJECT end_facade_2;
static NJS_CNK_OBJECT side_towers_2;
static NJS_CNK_OBJECT end_towers_2;
static NJS_CNK_OBJECT outer_street_2;
static NJS_CNK_OBJECT side_streets_2;
static NJS_CNK_OBJECT sidewalk_2;
static NJS_CNK_OBJECT corner_towers_2;
static NJS_CNK_OBJECT corner_towers_3;
static NJS_CNK_OBJECT corner_towers_4;
static NJS_CNK_OBJECT road_marking_2;
static NJS_CNK_OBJECT city_blocks_2;
static NJS_CNK_OBJECT city_blocks_3;
static NJS_CNK_OBJECT city_blocks_4;
static NJS_CNK_OBJECT lane_marking_2;
static NJS_CNK_OBJECT sign_line_2;
static NJS_CNK_OBJECT upper_sign_line_2;

static struct land_entry entries[67] = {
#include "boss_bigfoot/terrain/entries.inc"
};
struct land_table bigfoot_land = {
#include "boss_bigfoot/terrain/bigfoot_land.inc"
};
static NJS_TEXNAME textures[53] = {
#include "boss_bigfoot/terrain/textures.inc"
};
static NJS_TEXLIST terrain_textures = {
#include "boss_bigfoot/terrain/terrain_textures.inc"
};
static NJS_MATERIAL boundary_materials[1] = {
#include "boss_bigfoot/terrain/boundary_materials.inc"
};
static Sint16 boundary_faces[54] = {
#include "boss_bigfoot/terrain/boundary_faces.inc"
};
static NJS_MESHSET boundary_meshsets[1] = {
#include "boss_bigfoot/terrain/boundary_meshsets.inc"
};
static NJS_POINT3 boundary_points[36] = {
#include "boss_bigfoot/terrain/boundary_points.inc"
};
static NJS_MODEL boundary_model = {
#include "boss_bigfoot/terrain/boundary_model.inc"
};
static NJS_OBJECT boundary = {
#include "boss_bigfoot/terrain/boundary.inc"
};
static NJS_MATERIAL floor_collision_materials[1] = {
#include "boss_bigfoot/terrain/floor_collision_materials.inc"
};
static Sint16 floor_collision_faces[118] = {
#include "boss_bigfoot/terrain/floor_collision_faces.inc"
};
static NJS_MESHSET floor_collision_meshsets[1] = {
#include "boss_bigfoot/terrain/floor_collision_meshsets.inc"
};
static NJS_POINT3 floor_collision_points[48] = {
#include "boss_bigfoot/terrain/floor_collision_points.inc"
};
static NJS_MODEL floor_collision_model = {
#include "boss_bigfoot/terrain/floor_collision_model.inc"
};
static NJS_OBJECT floor_collision = {
#include "boss_bigfoot/terrain/floor_collision.inc"
};
static Sint16 street_front_1_polygons[265] = {
#include "boss_bigfoot/terrain/street_front_1_polygons.inc"
};
static Sint32 street_front_1_vertices[195] = {
#include "boss_bigfoot/terrain/street_front_1_vertices.inc"
};
static NJS_CNK_MODEL street_front_1_model = {
#include "boss_bigfoot/terrain/street_front_1_model.inc"
};
static NJS_CNK_OBJECT street_front_1 = {
#include "boss_bigfoot/terrain/street_front_1.inc"
};
static Sint16 walkway_1_polygons[33] = {
#include "boss_bigfoot/terrain/walkway_1_polygons.inc"
};
static Sint32 walkway_1_vertices[35] = {
#include "boss_bigfoot/terrain/walkway_1_vertices.inc"
};
static NJS_CNK_MODEL walkway_1_model = {
#include "boss_bigfoot/terrain/walkway_1_model.inc"
};
static NJS_CNK_OBJECT walkway_1 = {
#include "boss_bigfoot/terrain/walkway_1.inc"
};
static Sint16 side_front_1_polygons[73] = {
#include "boss_bigfoot/terrain/side_front_1_polygons.inc"
};
static Sint32 side_front_1_vertices[51] = {
#include "boss_bigfoot/terrain/side_front_1_vertices.inc"
};
static NJS_CNK_MODEL side_front_1_model = {
#include "boss_bigfoot/terrain/side_front_1_model.inc"
};
static NJS_CNK_OBJECT side_front_1 = {
#include "boss_bigfoot/terrain/side_front_1.inc"
};
static Sint16 corner_paving_1_polygons[55] = {
#include "boss_bigfoot/terrain/corner_paving_1_polygons.inc"
};
static Sint32 corner_paving_1_vertices[67] = {
#include "boss_bigfoot/terrain/corner_paving_1_vertices.inc"
};
static NJS_CNK_MODEL corner_paving_1_model = {
#include "boss_bigfoot/terrain/corner_paving_1_model.inc"
};
static NJS_CNK_OBJECT corner_paving_1 = {
#include "boss_bigfoot/terrain/corner_paving_1.inc"
};
static Sint16 road_stripe_1_polygons[19] = {
#include "boss_bigfoot/terrain/road_stripe_1_polygons.inc"
};
static Sint32 road_stripe_1_vertices[19] = {
#include "boss_bigfoot/terrain/road_stripe_1_vertices.inc"
};
static NJS_CNK_MODEL road_stripe_1_model = {
#include "boss_bigfoot/terrain/road_stripe_1_model.inc"
};
static NJS_CNK_OBJECT road_stripe_1 = {
#include "boss_bigfoot/terrain/road_stripe_1.inc"
};
static Sint16 corner_shop_1_polygons[191] = {
#include "boss_bigfoot/terrain/corner_shop_1_polygons.inc"
};
static Sint32 corner_shop_1_vertices[127] = {
#include "boss_bigfoot/terrain/corner_shop_1_vertices.inc"
};
static NJS_CNK_MODEL corner_shop_1_model = {
#include "boss_bigfoot/terrain/corner_shop_1_model.inc"
};
static NJS_CNK_OBJECT corner_shop_1 = {
#include "boss_bigfoot/terrain/corner_shop_1.inc"
};
static Sint16 crossing_1_polygons[207] = {
#include "boss_bigfoot/terrain/crossing_1_polygons.inc"
};
static Sint32 crossing_1_vertices[111] = {
#include "boss_bigfoot/terrain/crossing_1_vertices.inc"
};
static NJS_CNK_MODEL crossing_1_model = {
#include "boss_bigfoot/terrain/crossing_1_model.inc"
};
static NJS_CNK_OBJECT crossing_1 = {
#include "boss_bigfoot/terrain/crossing_1.inc"
};
static Sint16 shopfront_1_polygons[89] = {
#include "boss_bigfoot/terrain/shopfront_1_polygons.inc"
};
static Sint32 shopfront_1_vertices[59] = {
#include "boss_bigfoot/terrain/shopfront_1_vertices.inc"
};
static NJS_CNK_MODEL shopfront_1_model = {
#include "boss_bigfoot/terrain/shopfront_1_model.inc"
};
static NJS_CNK_OBJECT shopfront_1 = {
#include "boss_bigfoot/terrain/shopfront_1.inc"
};
static Sint16 storefront_1_polygons[89] = {
#include "boss_bigfoot/terrain/storefront_1_polygons.inc"
};
static Sint32 storefront_1_vertices[59] = {
#include "boss_bigfoot/terrain/storefront_1_vertices.inc"
};
static NJS_CNK_MODEL storefront_1_model = {
#include "boss_bigfoot/terrain/storefront_1_model.inc"
};
static NJS_CNK_OBJECT storefront_1 = {
#include "boss_bigfoot/terrain/storefront_1.inc"
};
static Sint16 junction_polygons[109] = {
#include "boss_bigfoot/terrain/junction_polygons.inc"
};
static Sint32 junction_vertices[131] = {
#include "boss_bigfoot/terrain/junction_vertices.inc"
};
static NJS_CNK_MODEL junction_model = {
#include "boss_bigfoot/terrain/junction_model.inc"
};
static NJS_CNK_OBJECT junction = {
#include "boss_bigfoot/terrain/junction.inc"
};
static Sint16 street_polygons[19] = {
#include "boss_bigfoot/terrain/street_polygons.inc"
};
static Sint32 street_vertices[19] = {
#include "boss_bigfoot/terrain/street_vertices.inc"
};
static NJS_CNK_MODEL street_model = {
#include "boss_bigfoot/terrain/street_model.inc"
};
static NJS_CNK_OBJECT street = {
#include "boss_bigfoot/terrain/street.inc"
};
static Sint16 roadbed_1_polygons[19] = {
#include "boss_bigfoot/terrain/roadbed_1_polygons.inc"
};
static Sint32 roadbed_1_vertices[19] = {
#include "boss_bigfoot/terrain/roadbed_1_vertices.inc"
};
static NJS_CNK_MODEL roadbed_1_model = {
#include "boss_bigfoot/terrain/roadbed_1_model.inc"
};
static NJS_CNK_OBJECT roadbed_1 = {
#include "boss_bigfoot/terrain/roadbed_1.inc"
};
static Sint16 corner_marking_1_polygons[43] = {
#include "boss_bigfoot/terrain/corner_marking_1_polygons.inc"
};
static Sint32 corner_marking_1_vertices[31] = {
#include "boss_bigfoot/terrain/corner_marking_1_vertices.inc"
};
static NJS_CNK_MODEL corner_marking_1_model = {
#include "boss_bigfoot/terrain/corner_marking_1_model.inc"
};
static NJS_CNK_OBJECT corner_marking_1 = {
#include "boss_bigfoot/terrain/corner_marking_1.inc"
};
static Sint16 corner_curb_1_polygons[81] = {
#include "boss_bigfoot/terrain/corner_curb_1_polygons.inc"
};
static Sint32 corner_curb_1_vertices[75] = {
#include "boss_bigfoot/terrain/corner_curb_1_vertices.inc"
};
static NJS_CNK_MODEL corner_curb_1_model = {
#include "boss_bigfoot/terrain/corner_curb_1_model.inc"
};
static NJS_CNK_OBJECT corner_curb_1 = {
#include "boss_bigfoot/terrain/corner_curb_1.inc"
};
static Sint16 shops_1_polygons[93] = {
#include "boss_bigfoot/terrain/shops_1_polygons.inc"
};
static Sint32 shops_1_vertices[83] = {
#include "boss_bigfoot/terrain/shops_1_vertices.inc"
};
static NJS_CNK_MODEL shops_1_model = {
#include "boss_bigfoot/terrain/shops_1_model.inc"
};
static NJS_CNK_OBJECT shops_1 = {
#include "boss_bigfoot/terrain/shops_1.inc"
};
static Sint16 side_facade_1_polygons[217] = {
#include "boss_bigfoot/terrain/side_facade_1_polygons.inc"
};
static Sint32 side_facade_1_vertices[127] = {
#include "boss_bigfoot/terrain/side_facade_1_vertices.inc"
};
static NJS_CNK_MODEL side_facade_1_model = {
#include "boss_bigfoot/terrain/side_facade_1_model.inc"
};
static NJS_CNK_OBJECT side_facade_1 = {
#include "boss_bigfoot/terrain/side_facade_1.inc"
};
static Sint16 end_facade_1_polygons[241] = {
#include "boss_bigfoot/terrain/end_facade_1_polygons.inc"
};
static Sint32 end_facade_1_vertices[183] = {
#include "boss_bigfoot/terrain/end_facade_1_vertices.inc"
};
static NJS_CNK_MODEL end_facade_1_model = {
#include "boss_bigfoot/terrain/end_facade_1_model.inc"
};
static NJS_CNK_OBJECT end_facade_1 = {
#include "boss_bigfoot/terrain/end_facade_1.inc"
};
static Sint16 side_towers_1_polygons[543] = {
#include "boss_bigfoot/terrain/side_towers_1_polygons.inc"
};
static Sint32 side_towers_1_vertices[399] = {
#include "boss_bigfoot/terrain/side_towers_1_vertices.inc"
};
static NJS_CNK_MODEL side_towers_1_model = {
#include "boss_bigfoot/terrain/side_towers_1_model.inc"
};
static NJS_CNK_OBJECT side_towers_1 = {
#include "boss_bigfoot/terrain/side_towers_1.inc"
};
static Sint16 end_towers_1_polygons[515] = {
#include "boss_bigfoot/terrain/end_towers_1_polygons.inc"
};
static Sint32 end_towers_1_vertices[447] = {
#include "boss_bigfoot/terrain/end_towers_1_vertices.inc"
};
static NJS_CNK_MODEL end_towers_1_model = {
#include "boss_bigfoot/terrain/end_towers_1_model.inc"
};
static NJS_CNK_OBJECT end_towers_1 = {
#include "boss_bigfoot/terrain/end_towers_1.inc"
};
static Sint16 outer_street_1_polygons[151] = {
#include "boss_bigfoot/terrain/outer_street_1_polygons.inc"
};
static Sint32 outer_street_1_vertices[103] = {
#include "boss_bigfoot/terrain/outer_street_1_vertices.inc"
};
static NJS_CNK_MODEL outer_street_1_model = {
#include "boss_bigfoot/terrain/outer_street_1_model.inc"
};
static NJS_CNK_OBJECT outer_street_1 = {
#include "boss_bigfoot/terrain/outer_street_1.inc"
};
static Sint16 side_streets_1_polygons[197] = {
#include "boss_bigfoot/terrain/side_streets_1_polygons.inc"
};
static Sint32 side_streets_1_vertices[139] = {
#include "boss_bigfoot/terrain/side_streets_1_vertices.inc"
};
static NJS_CNK_MODEL side_streets_1_model = {
#include "boss_bigfoot/terrain/side_streets_1_model.inc"
};
static NJS_CNK_OBJECT side_streets_1 = {
#include "boss_bigfoot/terrain/side_streets_1.inc"
};
static Sint16 sidewalk_1_polygons[33] = {
#include "boss_bigfoot/terrain/sidewalk_1_polygons.inc"
};
static Sint32 sidewalk_1_vertices[27] = {
#include "boss_bigfoot/terrain/sidewalk_1_vertices.inc"
};
static NJS_CNK_MODEL sidewalk_1_model = {
#include "boss_bigfoot/terrain/sidewalk_1_model.inc"
};
static NJS_CNK_OBJECT sidewalk_1 = {
#include "boss_bigfoot/terrain/sidewalk_1.inc"
};
static Sint16 corner_towers_1_polygons[1413] = {
#include "boss_bigfoot/terrain/corner_towers_1_polygons.inc"
};
static Sint32 corner_towers_1_vertices[1235] = {
#include "boss_bigfoot/terrain/corner_towers_1_vertices.inc"
};
static NJS_CNK_MODEL corner_towers_1_model = {
#include "boss_bigfoot/terrain/corner_towers_1_model.inc"
};
static NJS_CNK_OBJECT corner_towers_1 = {
#include "boss_bigfoot/terrain/corner_towers_1.inc"
};
static Sint16 road_marking_1_polygons[93] = {
#include "boss_bigfoot/terrain/road_marking_1_polygons.inc"
};
static Sint32 road_marking_1_vertices[115] = {
#include "boss_bigfoot/terrain/road_marking_1_vertices.inc"
};
static NJS_CNK_MODEL road_marking_1_model = {
#include "boss_bigfoot/terrain/road_marking_1_model.inc"
};
static NJS_CNK_OBJECT road_marking_1 = {
#include "boss_bigfoot/terrain/road_marking_1.inc"
};
static Sint16 city_blocks_1_polygons[1245] = {
#include "boss_bigfoot/terrain/city_blocks_1_polygons.inc"
};
static Sint32 city_blocks_1_vertices[1247] = {
#include "boss_bigfoot/terrain/city_blocks_1_vertices.inc"
};
static NJS_CNK_MODEL city_blocks_1_model = {
#include "boss_bigfoot/terrain/city_blocks_1_model.inc"
};
static NJS_CNK_OBJECT city_blocks_1 = {
#include "boss_bigfoot/terrain/city_blocks_1.inc"
};
static Sint16 lane_marking_1_polygons[33] = {
#include "boss_bigfoot/terrain/lane_marking_1_polygons.inc"
};
static Sint32 lane_marking_1_vertices[35] = {
#include "boss_bigfoot/terrain/lane_marking_1_vertices.inc"
};
static NJS_CNK_MODEL lane_marking_1_model = {
#include "boss_bigfoot/terrain/lane_marking_1_model.inc"
};
static NJS_CNK_OBJECT lane_marking_1 = {
#include "boss_bigfoot/terrain/lane_marking_1.inc"
};
static Sint16 sign_line_1_polygons[121] = {
#include "boss_bigfoot/terrain/sign_line_1_polygons.inc"
};
static Sint32 sign_line_1_vertices[67] = {
#include "boss_bigfoot/terrain/sign_line_1_vertices.inc"
};
static NJS_CNK_MODEL sign_line_1_model = {
#include "boss_bigfoot/terrain/sign_line_1_model.inc"
};
static NJS_CNK_OBJECT sign_line_1 = {
#include "boss_bigfoot/terrain/sign_line_1.inc"
};
static Sint16 upper_sign_line_1_polygons[107] = {
#include "boss_bigfoot/terrain/upper_sign_line_1_polygons.inc"
};
static Sint32 upper_sign_line_1_vertices[67] = {
#include "boss_bigfoot/terrain/upper_sign_line_1_vertices.inc"
};
static NJS_CNK_MODEL upper_sign_line_1_model = {
#include "boss_bigfoot/terrain/upper_sign_line_1_model.inc"
};
static NJS_CNK_OBJECT upper_sign_line_1 = {
#include "boss_bigfoot/terrain/upper_sign_line_1.inc"
};
static Sint16 side_front_end_polygons[73] = {
#include "boss_bigfoot/terrain/side_front_end_polygons.inc"
};
static Sint32 side_front_end_vertices[51] = {
#include "boss_bigfoot/terrain/side_front_end_vertices.inc"
};
static NJS_CNK_MODEL side_front_end_model = {
#include "boss_bigfoot/terrain/side_front_end_model.inc"
};
static NJS_CNK_OBJECT side_front_end = {
#include "boss_bigfoot/terrain/side_front_end.inc"
};
static Sint16 skyline_polygons[907] = {
#include "boss_bigfoot/terrain/skyline_polygons.inc"
};
static Sint32 skyline_vertices[459] = {
#include "boss_bigfoot/terrain/skyline_vertices.inc"
};
static NJS_CNK_MODEL skyline_model = {
#include "boss_bigfoot/terrain/skyline_model.inc"
};
static NJS_CNK_OBJECT skyline = {
#include "boss_bigfoot/terrain/skyline.inc"
};
static NJS_CNK_OBJECT street_front_2 = {
#include "boss_bigfoot/terrain/street_front_2.inc"
};
static NJS_CNK_OBJECT walkway_2 = {
#include "boss_bigfoot/terrain/walkway_2.inc"
};
static NJS_CNK_OBJECT walkway_3 = {
#include "boss_bigfoot/terrain/walkway_3.inc"
};
static NJS_CNK_OBJECT walkway_4 = {
#include "boss_bigfoot/terrain/walkway_4.inc"
};
static NJS_CNK_OBJECT side_front_2 = {
#include "boss_bigfoot/terrain/side_front_2.inc"
};
static NJS_CNK_OBJECT side_front_3 = {
#include "boss_bigfoot/terrain/side_front_3.inc"
};
static NJS_CNK_OBJECT corner_paving_2 = {
#include "boss_bigfoot/terrain/corner_paving_2.inc"
};
static NJS_CNK_OBJECT road_stripe_2 = {
#include "boss_bigfoot/terrain/road_stripe_2.inc"
};
static NJS_CNK_OBJECT road_stripe_3 = {
#include "boss_bigfoot/terrain/road_stripe_3.inc"
};
static NJS_CNK_OBJECT road_stripe_4 = {
#include "boss_bigfoot/terrain/road_stripe_4.inc"
};
static NJS_CNK_OBJECT corner_shop_2 = {
#include "boss_bigfoot/terrain/corner_shop_2.inc"
};
static NJS_CNK_OBJECT crossing_2 = {
#include "boss_bigfoot/terrain/crossing_2.inc"
};
static NJS_CNK_OBJECT shopfront_2 = {
#include "boss_bigfoot/terrain/shopfront_2.inc"
};
static NJS_CNK_OBJECT storefront_2 = {
#include "boss_bigfoot/terrain/storefront_2.inc"
};
static NJS_CNK_OBJECT roadbed_2 = {
#include "boss_bigfoot/terrain/roadbed_2.inc"
};
static NJS_CNK_OBJECT corner_marking_2 = {
#include "boss_bigfoot/terrain/corner_marking_2.inc"
};
static NJS_CNK_OBJECT corner_curb_2 = {
#include "boss_bigfoot/terrain/corner_curb_2.inc"
};
static NJS_CNK_OBJECT shops_2 = {
#include "boss_bigfoot/terrain/shops_2.inc"
};
static NJS_CNK_OBJECT side_facade_2 = {
#include "boss_bigfoot/terrain/side_facade_2.inc"
};
static NJS_CNK_OBJECT end_facade_2 = {
#include "boss_bigfoot/terrain/end_facade_2.inc"
};
static NJS_CNK_OBJECT side_towers_2 = {
#include "boss_bigfoot/terrain/side_towers_2.inc"
};
static NJS_CNK_OBJECT end_towers_2 = {
#include "boss_bigfoot/terrain/end_towers_2.inc"
};
static NJS_CNK_OBJECT outer_street_2 = {
#include "boss_bigfoot/terrain/outer_street_2.inc"
};
static NJS_CNK_OBJECT side_streets_2 = {
#include "boss_bigfoot/terrain/side_streets_2.inc"
};
static NJS_CNK_OBJECT sidewalk_2 = {
#include "boss_bigfoot/terrain/sidewalk_2.inc"
};
static NJS_CNK_OBJECT corner_towers_2 = {
#include "boss_bigfoot/terrain/corner_towers_2.inc"
};
static NJS_CNK_OBJECT corner_towers_3 = {
#include "boss_bigfoot/terrain/corner_towers_3.inc"
};
static NJS_CNK_OBJECT corner_towers_4 = {
#include "boss_bigfoot/terrain/corner_towers_4.inc"
};
static NJS_CNK_OBJECT road_marking_2 = {
#include "boss_bigfoot/terrain/road_marking_2.inc"
};
static NJS_CNK_OBJECT city_blocks_2 = {
#include "boss_bigfoot/terrain/city_blocks_2.inc"
};
static NJS_CNK_OBJECT city_blocks_3 = {
#include "boss_bigfoot/terrain/city_blocks_3.inc"
};
static NJS_CNK_OBJECT city_blocks_4 = {
#include "boss_bigfoot/terrain/city_blocks_4.inc"
};
static NJS_CNK_OBJECT lane_marking_2 = {
#include "boss_bigfoot/terrain/lane_marking_2.inc"
};
static NJS_CNK_OBJECT sign_line_2 = {
#include "boss_bigfoot/terrain/sign_line_2.inc"
};
static NJS_CNK_OBJECT upper_sign_line_2 = {
#include "boss_bigfoot/terrain/upper_sign_line_2.inc"
};
static Sint16 rear_toe_b_polygons[135] = {
#include "boss_bigfoot/terrain/rear_toe_b_polygons.inc"
};
static Sint32 rear_toe_b_vertices[75] = {
#include "boss_bigfoot/terrain/rear_toe_b_vertices.inc"
};
static NJS_CNK_MODEL rear_toe_b_mesh = {
#include "boss_bigfoot/terrain/rear_toe_b_mesh.inc"
};
static NJS_CNK_OBJECT rear_toe_b = {
#include "boss_bigfoot/terrain/rear_toe_b.inc"
};
static Sint16 outer_toe_b_polygons[135] = {
#include "boss_bigfoot/terrain/outer_toe_b_polygons.inc"
};
static Sint32 outer_toe_b_vertices[75] = {
#include "boss_bigfoot/terrain/outer_toe_b_vertices.inc"
};
static NJS_CNK_MODEL outer_toe_b_mesh = {
#include "boss_bigfoot/terrain/outer_toe_b_mesh.inc"
};
static NJS_CNK_OBJECT outer_toe_b = {
#include "boss_bigfoot/terrain/outer_toe_b.inc"
};
static Sint16 inner_toe_b_polygons[135] = {
#include "boss_bigfoot/terrain/inner_toe_b_polygons.inc"
};
static Sint32 inner_toe_b_vertices[75] = {
#include "boss_bigfoot/terrain/inner_toe_b_vertices.inc"
};
static NJS_CNK_MODEL inner_toe_b_mesh = {
#include "boss_bigfoot/terrain/inner_toe_b_mesh.inc"
};
static NJS_CNK_OBJECT inner_toe_b = {
#include "boss_bigfoot/terrain/inner_toe_b.inc"
};
static Sint16 foot_b_polygons[281] = {
#include "boss_bigfoot/terrain/foot_b_polygons.inc"
};
static Sint32 foot_b_vertices[195] = {
#include "boss_bigfoot/terrain/foot_b_vertices.inc"
};
static NJS_CNK_MODEL foot_b_mesh = {
#include "boss_bigfoot/terrain/foot_b_mesh.inc"
};
NJS_CNK_OBJECT bigfoot_foot_b_node = {
#include "boss_bigfoot/terrain/bigfoot_foot_b_node.inc"
};
static Sint16 shin_b_polygons[387] = {
#include "boss_bigfoot/terrain/shin_b_polygons.inc"
};
static Sint32 shin_b_vertices[255] = {
#include "boss_bigfoot/terrain/shin_b_vertices.inc"
};
static NJS_CNK_MODEL shin_b_mesh = {
#include "boss_bigfoot/terrain/shin_b_mesh.inc"
};
static NJS_CNK_OBJECT shin_b = {
#include "boss_bigfoot/terrain/shin_b.inc"
};
static Sint16 thigh_b_polygons[181] = {
#include "boss_bigfoot/terrain/thigh_b_polygons.inc"
};
static Sint32 thigh_b_vertices[99] = {
#include "boss_bigfoot/terrain/thigh_b_vertices.inc"
};
static NJS_CNK_MODEL thigh_b_mesh = {
#include "boss_bigfoot/terrain/thigh_b_mesh.inc"
};
static NJS_CNK_OBJECT thigh_b = {
#include "boss_bigfoot/terrain/thigh_b.inc"
};
static Sint16 hip_b_polygons[303] = {
#include "boss_bigfoot/terrain/hip_b_polygons.inc"
};
static Sint32 hip_b_vertices[219] = {
#include "boss_bigfoot/terrain/hip_b_vertices.inc"
};
static NJS_CNK_MODEL hip_b_mesh = {
#include "boss_bigfoot/terrain/hip_b_mesh.inc"
};
static NJS_CNK_OBJECT hip_b = {
#include "boss_bigfoot/terrain/hip_b.inc"
};
static Sint16 rear_toe_a_polygons[135] = {
#include "boss_bigfoot/terrain/rear_toe_a_polygons.inc"
};
static Sint32 rear_toe_a_vertices[75] = {
#include "boss_bigfoot/terrain/rear_toe_a_vertices.inc"
};
static NJS_CNK_MODEL rear_toe_a_mesh = {
#include "boss_bigfoot/terrain/rear_toe_a_mesh.inc"
};
static NJS_CNK_OBJECT rear_toe_a = {
#include "boss_bigfoot/terrain/rear_toe_a.inc"
};
static Sint16 outer_toe_a_polygons[135] = {
#include "boss_bigfoot/terrain/outer_toe_a_polygons.inc"
};
static Sint32 outer_toe_a_vertices[75] = {
#include "boss_bigfoot/terrain/outer_toe_a_vertices.inc"
};
static NJS_CNK_MODEL outer_toe_a_mesh = {
#include "boss_bigfoot/terrain/outer_toe_a_mesh.inc"
};
static NJS_CNK_OBJECT outer_toe_a = {
#include "boss_bigfoot/terrain/outer_toe_a.inc"
};
static Sint16 inner_toe_a_polygons[135] = {
#include "boss_bigfoot/terrain/inner_toe_a_polygons.inc"
};
static Sint32 inner_toe_a_vertices[75] = {
#include "boss_bigfoot/terrain/inner_toe_a_vertices.inc"
};
static NJS_CNK_MODEL inner_toe_a_mesh = {
#include "boss_bigfoot/terrain/inner_toe_a_mesh.inc"
};
static NJS_CNK_OBJECT inner_toe_a = {
#include "boss_bigfoot/terrain/inner_toe_a.inc"
};
static Sint16 foot_a_polygons[281] = {
#include "boss_bigfoot/terrain/foot_a_polygons.inc"
};
static Sint32 foot_a_vertices[195] = {
#include "boss_bigfoot/terrain/foot_a_vertices.inc"
};
static NJS_CNK_MODEL foot_a_mesh = {
#include "boss_bigfoot/terrain/foot_a_mesh.inc"
};
NJS_CNK_OBJECT bigfoot_foot_a_node = {
#include "boss_bigfoot/terrain/bigfoot_foot_a_node.inc"
};
static Sint16 shin_a_polygons[387] = {
#include "boss_bigfoot/terrain/shin_a_polygons.inc"
};
static Sint32 shin_a_vertices[255] = {
#include "boss_bigfoot/terrain/shin_a_vertices.inc"
};
static NJS_CNK_MODEL shin_a_mesh = {
#include "boss_bigfoot/terrain/shin_a_mesh.inc"
};
static NJS_CNK_OBJECT shin_a = {
#include "boss_bigfoot/terrain/shin_a.inc"
};
static Sint16 thigh_a_polygons[181] = {
#include "boss_bigfoot/terrain/thigh_a_polygons.inc"
};
static Sint32 thigh_a_vertices[99] = {
#include "boss_bigfoot/terrain/thigh_a_vertices.inc"
};
static NJS_CNK_MODEL thigh_a_mesh = {
#include "boss_bigfoot/terrain/thigh_a_mesh.inc"
};
static NJS_CNK_OBJECT thigh_a = {
#include "boss_bigfoot/terrain/thigh_a.inc"
};
static Sint16 hip_a_polygons[303] = {
#include "boss_bigfoot/terrain/hip_a_polygons.inc"
};
static Sint32 hip_a_vertices[219] = {
#include "boss_bigfoot/terrain/hip_a_vertices.inc"
};
static NJS_CNK_MODEL hip_a_mesh = {
#include "boss_bigfoot/terrain/hip_a_mesh.inc"
};
static NJS_CNK_OBJECT hip_a = {
#include "boss_bigfoot/terrain/hip_a.inc"
};
static Sint16 pod_a_polygons[275] = {
#include "boss_bigfoot/terrain/pod_a_polygons.inc"
};
static Sint32 pod_a_vertices[147] = {
#include "boss_bigfoot/terrain/pod_a_vertices.inc"
};
static NJS_CNK_MODEL pod_a_mesh = {
#include "boss_bigfoot/terrain/pod_a_mesh.inc"
};
static NJS_CNK_OBJECT pod_a = {
#include "boss_bigfoot/terrain/pod_a.inc"
};
NJS_CNK_OBJECT bigfoot_laser_node = {
#include "boss_bigfoot/terrain/bigfoot_laser_node.inc"
};
static Sint16 turret_polygons[203] = {
#include "boss_bigfoot/terrain/turret_polygons.inc"
};
static Sint32 turret_vertices[147] = {
#include "boss_bigfoot/terrain/turret_vertices.inc"
};
static NJS_CNK_MODEL turret_mesh = {
#include "boss_bigfoot/terrain/turret_mesh.inc"
};
NJS_CNK_OBJECT bigfoot_turret_node = {
#include "boss_bigfoot/terrain/bigfoot_turret_node.inc"
};
static Sint16 pod_b_polygons[275] = {
#include "boss_bigfoot/terrain/pod_b_polygons.inc"
};
static Sint32 pod_b_vertices[147] = {
#include "boss_bigfoot/terrain/pod_b_vertices.inc"
};
static NJS_CNK_MODEL pod_b_mesh = {
#include "boss_bigfoot/terrain/pod_b_mesh.inc"
};
static NJS_CNK_OBJECT pod_b = {
#include "boss_bigfoot/terrain/pod_b.inc"
};
static Sint16 body_polygons[4259] = {
#include "boss_bigfoot/terrain/body_polygons.inc"
};
static Sint32 body_vertices[3141] = {
#include "boss_bigfoot/terrain/body_vertices.inc"
};
static NJS_CNK_MODEL body_mesh = {
#include "boss_bigfoot/terrain/body_mesh.inc"
};
NJS_CNK_OBJECT bigfoot_body_node = {
#include "boss_bigfoot/terrain/bigfoot_body_node.inc"
};
NJS_CNK_OBJECT bigfoot_object = {
#include "boss_bigfoot/terrain/bigfoot_object.inc"
};
static NJS_MKEY_F bigfoot_hit_motion_joint_0_position[33] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_0_position.inc"
};
static NJS_MKEY_F bigfoot_hit_motion_joint_1_position[33] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_1_angles[28] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_1_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_2_angles[2] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_5_angles[2] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_6_angles[31] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_8_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_9_angles[33] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_13_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_14_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_15_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_15_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_16_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_16_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_17_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_18_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_motion_joint_19_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_joint_19_angles.inc"
};
static NJS_MDATA2 bigfoot_hit_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion_tracks.inc"
};
NJS_MOTION bigfoot_hit_motion = {
#include "boss_bigfoot/terrain/bigfoot_hit_motion.inc"
};
static NJS_MKEY_F bigfoot_collapse_motion_joint_0_position[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_0_position.inc"
};
static NJS_MKEY_F bigfoot_collapse_motion_joint_1_position[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_1_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_1_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_2_angles[10] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_5_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_6_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_7_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_8_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_9_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_10_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_10_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_11_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_12_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_13_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_14_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_15_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_15_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_16_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_16_angles.inc"
};
static NJS_MKEY_A bigfoot_collapse_motion_joint_17_angles[17] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_joint_17_angles.inc"
};
static NJS_MDATA2 bigfoot_collapse_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion_tracks.inc"
};
NJS_MOTION bigfoot_collapse_motion = {
#include "boss_bigfoot/terrain/bigfoot_collapse_motion.inc"
};
static NJS_MKEY_F bigfoot_lean_motion_joint_1_position[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_1_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_1_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_2_angles[2] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_5_angles[2] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_6_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_7_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_8_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_9_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_10_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_10_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_11_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_12_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_13_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_14_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_15_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_15_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_16_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_16_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_17_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_18_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigfoot_lean_motion_joint_19_angles[12] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_joint_19_angles.inc"
};
static NJS_MDATA2 bigfoot_lean_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion_tracks.inc"
};
NJS_MOTION bigfoot_lean_motion = {
#include "boss_bigfoot/terrain/bigfoot_lean_motion.inc"
};
static NJS_MKEY_F unused_a_joint_1_position[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_1_position.inc"
};
static NJS_MKEY_A unused_a_joint_1_angles[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_1_angles.inc"
};
static NJS_MKEY_A unused_a_joint_2_angles[2] = {
#include "boss_bigfoot/terrain/unused_a_joint_2_angles.inc"
};
static NJS_MKEY_A unused_a_joint_5_angles[2] = {
#include "boss_bigfoot/terrain/unused_a_joint_5_angles.inc"
};
static NJS_MKEY_A unused_a_joint_7_angles[2] = {
#include "boss_bigfoot/terrain/unused_a_joint_7_angles.inc"
};
static NJS_MKEY_A unused_a_joint_10_angles[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_10_angles.inc"
};
static NJS_MKEY_A unused_a_joint_11_angles[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_11_angles.inc"
};
static NJS_MKEY_A unused_a_joint_12_angles[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_12_angles.inc"
};
static NJS_MKEY_A unused_a_joint_17_angles[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_17_angles.inc"
};
static NJS_MKEY_A unused_a_joint_18_angles[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_18_angles.inc"
};
static NJS_MKEY_A unused_a_joint_19_angles[8] = {
#include "boss_bigfoot/terrain/unused_a_joint_19_angles.inc"
};
static NJS_MDATA2 unused_a_tracks[20] = {
#include "boss_bigfoot/terrain/unused_a_tracks.inc"
};
static NJS_MOTION unused_a = {
#include "boss_bigfoot/terrain/unused_a.inc"
};
static NJS_MKEY_F unused_b_joint_1_position[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_1_position.inc"
};
static NJS_MKEY_A unused_b_joint_1_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_1_angles.inc"
};
static NJS_MKEY_A unused_b_joint_6_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_6_angles.inc"
};
static NJS_MKEY_A unused_b_joint_7_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_7_angles.inc"
};
static NJS_MKEY_A unused_b_joint_8_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_8_angles.inc"
};
static NJS_MKEY_A unused_b_joint_9_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_9_angles.inc"
};
static NJS_MKEY_A unused_b_joint_10_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_10_angles.inc"
};
static NJS_MKEY_A unused_b_joint_11_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_11_angles.inc"
};
static NJS_MKEY_A unused_b_joint_12_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_12_angles.inc"
};
static NJS_MKEY_A unused_b_joint_13_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_13_angles.inc"
};
static NJS_MKEY_A unused_b_joint_14_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_14_angles.inc"
};
static NJS_MKEY_A unused_b_joint_15_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_15_angles.inc"
};
static NJS_MKEY_A unused_b_joint_16_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_16_angles.inc"
};
static NJS_MKEY_A unused_b_joint_17_angles[2] = {
#include "boss_bigfoot/terrain/unused_b_joint_17_angles.inc"
};
static NJS_MKEY_A unused_b_joint_18_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_18_angles.inc"
};
static NJS_MKEY_A unused_b_joint_19_angles[8] = {
#include "boss_bigfoot/terrain/unused_b_joint_19_angles.inc"
};
static NJS_MDATA2 unused_b_tracks[20] = {
#include "boss_bigfoot/terrain/unused_b_tracks.inc"
};
static NJS_MOTION unused_b = {
#include "boss_bigfoot/terrain/unused_b.inc"
};
static NJS_MKEY_F unused_c_joint_1_position[13] = {
#include "boss_bigfoot/terrain/unused_c_joint_1_position.inc"
};
static NJS_MKEY_A unused_c_joint_1_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_1_angles.inc"
};
static NJS_MKEY_A unused_c_joint_6_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_6_angles.inc"
};
static NJS_MKEY_A unused_c_joint_7_angles[11] = {
#include "boss_bigfoot/terrain/unused_c_joint_7_angles.inc"
};
static NJS_MKEY_A unused_c_joint_8_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_8_angles.inc"
};
static NJS_MKEY_A unused_c_joint_9_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_9_angles.inc"
};
static NJS_MKEY_A unused_c_joint_10_angles[6] = {
#include "boss_bigfoot/terrain/unused_c_joint_10_angles.inc"
};
static NJS_MKEY_A unused_c_joint_11_angles[6] = {
#include "boss_bigfoot/terrain/unused_c_joint_11_angles.inc"
};
static NJS_MKEY_A unused_c_joint_12_angles[6] = {
#include "boss_bigfoot/terrain/unused_c_joint_12_angles.inc"
};
static NJS_MKEY_A unused_c_joint_13_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_13_angles.inc"
};
static NJS_MKEY_A unused_c_joint_14_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_14_angles.inc"
};
static NJS_MKEY_A unused_c_joint_15_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_15_angles.inc"
};
static NJS_MKEY_A unused_c_joint_16_angles[10] = {
#include "boss_bigfoot/terrain/unused_c_joint_16_angles.inc"
};
static NJS_MKEY_A unused_c_joint_17_angles[6] = {
#include "boss_bigfoot/terrain/unused_c_joint_17_angles.inc"
};
static NJS_MKEY_A unused_c_joint_18_angles[6] = {
#include "boss_bigfoot/terrain/unused_c_joint_18_angles.inc"
};
static NJS_MKEY_A unused_c_joint_19_angles[6] = {
#include "boss_bigfoot/terrain/unused_c_joint_19_angles.inc"
};
static NJS_MDATA2 unused_c_tracks[20] = {
#include "boss_bigfoot/terrain/unused_c_tracks.inc"
};
static NJS_MOTION unused_c = {
#include "boss_bigfoot/terrain/unused_c.inc"
};
static NJS_MKEY_F bigfoot_stomp_motion_joint_1_position[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_1_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_1_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_2_angles[10] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_2_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_5_angles[10] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_5_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_6_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_7_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_8_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_9_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_10_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_10_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_11_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_12_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_13_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_14_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_15_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_15_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_16_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_16_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_17_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_18_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigfoot_stomp_motion_joint_19_angles[14] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_joint_19_angles.inc"
};
static NJS_MDATA2 bigfoot_stomp_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion_tracks.inc"
};
NJS_MOTION bigfoot_stomp_motion = {
#include "boss_bigfoot/terrain/bigfoot_stomp_motion.inc"
};
static NJS_MKEY_F bigfoot_stand_motion_joint_1_position[4] = {
#include "boss_bigfoot/terrain/bigfoot_stand_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_stand_motion_joint_6_angles[4] = {
#include "boss_bigfoot/terrain/bigfoot_stand_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_stand_motion_joint_7_angles[4] = {
#include "boss_bigfoot/terrain/bigfoot_stand_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigfoot_stand_motion_joint_13_angles[4] = {
#include "boss_bigfoot/terrain/bigfoot_stand_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_stand_motion_joint_14_angles[4] = {
#include "boss_bigfoot/terrain/bigfoot_stand_motion_joint_14_angles.inc"
};
static NJS_MDATA2 bigfoot_stand_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_stand_motion_tracks.inc"
};
NJS_MOTION bigfoot_stand_motion = {
#include "boss_bigfoot/terrain/bigfoot_stand_motion.inc"
};
static NJS_MKEY_F bigfoot_step_motion_joint_1_position[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_6_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_7_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_8_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_9_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_10_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_10_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_11_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_12_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_13_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_14_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_15_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_15_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_16_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_16_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_17_angles[26] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_18_angles[26] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigfoot_step_motion_joint_19_angles[26] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_joint_19_angles.inc"
};
static NJS_MDATA2 bigfoot_step_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_step_motion_tracks.inc"
};
NJS_MOTION bigfoot_step_motion = {
#include "boss_bigfoot/terrain/bigfoot_step_motion.inc"
};
static NJS_MKEY_F bigfoot_take_off_motion_joint_0_position[9] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_0_position.inc"
};
static NJS_MKEY_F bigfoot_take_off_motion_joint_1_position[17] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_1_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_1_angles.inc"
};
static NJS_MKEY_F bigfoot_take_off_motion_joint_4_position[2] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_4_position.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_6_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_7_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_7_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_8_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_9_angles[21] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_10_angles[15] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_10_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_11_angles[15] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_11_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_12_angles[15] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_12_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_13_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_14_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_15_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_15_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_16_angles[21] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_16_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_17_angles[15] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_18_angles[15] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigfoot_take_off_motion_joint_19_angles[15] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_joint_19_angles.inc"
};
static NJS_MDATA2 bigfoot_take_off_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion_tracks.inc"
};
NJS_MOTION bigfoot_take_off_motion = {
#include "boss_bigfoot/terrain/bigfoot_take_off_motion.inc"
};
static NJS_MKEY_F bigfoot_hit_empty_motion_joint_0_position[33] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_0_position.inc"
};
static NJS_MKEY_F bigfoot_hit_empty_motion_joint_1_position[33] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_1_position.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_1_angles[28] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_1_angles.inc"
};
static NJS_MKEY_F bigfoot_hit_empty_motion_joint_4_position[2] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_4_position.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_6_angles[31] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_6_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_8_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_8_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_9_angles[33] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_9_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_13_angles[32] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_13_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_14_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_14_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_15_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_15_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_16_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_16_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_17_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_17_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_18_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_18_angles.inc"
};
static NJS_MKEY_A bigfoot_hit_empty_motion_joint_19_angles[18] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_joint_19_angles.inc"
};
static NJS_MDATA2 bigfoot_hit_empty_motion_tracks[20] = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion_tracks.inc"
};
NJS_MOTION bigfoot_hit_empty_motion = {
#include "boss_bigfoot/terrain/bigfoot_hit_empty_motion.inc"
};
