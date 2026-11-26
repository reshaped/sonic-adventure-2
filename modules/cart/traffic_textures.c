#include <ninja.h>
#include <sg_xpt.h>

#include "cart/kart_art.h"
#include "cart/objects.h"
#include "cart/traffic_lanes.h"

/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
const char cart_traffic_texture_filenames[1440] = {
#include "cart/traffic_textures/cart_traffic_texture_filenames.inc"
};

/* CART.PRS's texture lists and chunk models, from the disc. its D lies
 * between race_clock.c and traffic.c */
static NJS_TEXNAME story_slots[45] = {
#include "cart/traffic_textures/story_slots.inc"
};
NJS_TEXLIST cart_story_textures = {
#include "cart/traffic_textures/cart_story_textures.inc"
};
static Sint16 cart_car_0_rear_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_0_rear_axle_polygons.inc"
};
static Sint32 cart_car_0_rear_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_0_rear_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_0_rear_axle_model = {
#include "cart/traffic_textures/cart_car_0_rear_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_0_rear_axle = {
#include "cart/traffic_textures/cart_car_0_rear_axle.inc"
};
static Sint16 cart_car_0_front_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_0_front_axle_polygons.inc"
};
static Sint32 cart_car_0_front_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_0_front_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_0_front_axle_model = {
#include "cart/traffic_textures/cart_car_0_front_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_0_front_axle = {
#include "cart/traffic_textures/cart_car_0_front_axle.inc"
};
static Sint16 cart_car_0_body_polygons[959] = {
#include "cart/traffic_textures/cart_car_0_body_polygons.inc"
};
static Sint32 cart_car_0_body_vertices[731] = {
#include "cart/traffic_textures/cart_car_0_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_0_body_model = {
#include "cart/traffic_textures/cart_car_0_body_model.inc"
};
NJS_CNK_OBJECT cart_traffic_model_0 = {
#include "cart/traffic_textures/cart_traffic_model_0.inc"
};
static Sint16 cart_car_0_combined_body_polygons[1017] = {
#include "cart/traffic_textures/cart_car_0_combined_body_polygons.inc"
};
static Sint32 cart_car_0_combined_body_vertices[795] = {
#include "cart/traffic_textures/cart_car_0_combined_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_0_combined_body_model = {
#include "cart/traffic_textures/cart_car_0_combined_body_model.inc"
};
static NJS_CNK_OBJECT cart_car_0_combined_body = {
#include "cart/traffic_textures/cart_car_0_combined_body.inc"
};
static Sint16 cart_car_1_front_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_1_front_axle_polygons.inc"
};
static Sint32 cart_car_1_front_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_1_front_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_1_front_axle_model = {
#include "cart/traffic_textures/cart_car_1_front_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_1_front_axle = {
#include "cart/traffic_textures/cart_car_1_front_axle.inc"
};
static Sint16 cart_car_1_rear_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_1_rear_axle_polygons.inc"
};
static Sint32 cart_car_1_rear_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_1_rear_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_1_rear_axle_model = {
#include "cart/traffic_textures/cart_car_1_rear_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_1_rear_axle = {
#include "cart/traffic_textures/cart_car_1_rear_axle.inc"
};
static Sint16 cart_car_1_body_polygons[941] = {
#include "cart/traffic_textures/cart_car_1_body_polygons.inc"
};
static Sint32 cart_car_1_body_vertices[675] = {
#include "cart/traffic_textures/cart_car_1_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_1_body_model = {
#include "cart/traffic_textures/cart_car_1_body_model.inc"
};
NJS_CNK_OBJECT cart_traffic_model_1 = {
#include "cart/traffic_textures/cart_traffic_model_1.inc"
};
static Sint16 cart_car_1_combined_body_polygons[991] = {
#include "cart/traffic_textures/cart_car_1_combined_body_polygons.inc"
};
static Sint32 cart_car_1_combined_body_vertices[739] = {
#include "cart/traffic_textures/cart_car_1_combined_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_1_combined_body_model = {
#include "cart/traffic_textures/cart_car_1_combined_body_model.inc"
};
static NJS_CNK_OBJECT cart_car_1_combined_body = {
#include "cart/traffic_textures/cart_car_1_combined_body.inc"
};
static Sint16 cart_car_2_front_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_2_front_axle_polygons.inc"
};
static Sint32 cart_car_2_front_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_2_front_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_2_front_axle_model = {
#include "cart/traffic_textures/cart_car_2_front_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_2_front_axle = {
#include "cart/traffic_textures/cart_car_2_front_axle.inc"
};
static Sint16 cart_car_2_rear_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_2_rear_axle_polygons.inc"
};
static Sint32 cart_car_2_rear_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_2_rear_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_2_rear_axle_model = {
#include "cart/traffic_textures/cart_car_2_rear_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_2_rear_axle = {
#include "cart/traffic_textures/cart_car_2_rear_axle.inc"
};
static Sint16 cart_car_2_body_polygons[921] = {
#include "cart/traffic_textures/cart_car_2_body_polygons.inc"
};
static Sint32 cart_car_2_body_vertices[667] = {
#include "cart/traffic_textures/cart_car_2_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_2_body_model = {
#include "cart/traffic_textures/cart_car_2_body_model.inc"
};
NJS_CNK_OBJECT cart_traffic_model_2 = {
#include "cart/traffic_textures/cart_traffic_model_2.inc"
};
static Sint16 cart_car_2_combined_body_polygons[979] = {
#include "cart/traffic_textures/cart_car_2_combined_body_polygons.inc"
};
static Sint32 cart_car_2_combined_body_vertices[731] = {
#include "cart/traffic_textures/cart_car_2_combined_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_2_combined_body_model = {
#include "cart/traffic_textures/cart_car_2_combined_body_model.inc"
};
static NJS_CNK_OBJECT cart_car_2_combined_body = {
#include "cart/traffic_textures/cart_car_2_combined_body.inc"
};
static Sint16 cart_car_3_front_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_3_front_axle_polygons.inc"
};
static Sint32 cart_car_3_front_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_3_front_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_3_front_axle_model = {
#include "cart/traffic_textures/cart_car_3_front_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_3_front_axle = {
#include "cart/traffic_textures/cart_car_3_front_axle.inc"
};
static Sint16 cart_car_3_rear_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_3_rear_axle_polygons.inc"
};
static Sint32 cart_car_3_rear_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_3_rear_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_3_rear_axle_model = {
#include "cart/traffic_textures/cart_car_3_rear_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_3_rear_axle = {
#include "cart/traffic_textures/cart_car_3_rear_axle.inc"
};
static Sint16 cart_car_3_body_polygons[947] = {
#include "cart/traffic_textures/cart_car_3_body_polygons.inc"
};
static Sint32 cart_car_3_body_vertices[795] = {
#include "cart/traffic_textures/cart_car_3_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_3_body_model = {
#include "cart/traffic_textures/cart_car_3_body_model.inc"
};
NJS_CNK_OBJECT cart_traffic_model_3 = {
#include "cart/traffic_textures/cart_traffic_model_3.inc"
};
static Sint16 cart_car_3_combined_body_polygons[1003] = {
#include "cart/traffic_textures/cart_car_3_combined_body_polygons.inc"
};
static Sint32 cart_car_3_combined_body_vertices[859] = {
#include "cart/traffic_textures/cart_car_3_combined_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_3_combined_body_model = {
#include "cart/traffic_textures/cart_car_3_combined_body_model.inc"
};
static NJS_CNK_OBJECT cart_car_3_combined_body = {
#include "cart/traffic_textures/cart_car_3_combined_body.inc"
};
static Sint16 cart_car_4_front_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_4_front_axle_polygons.inc"
};
static Sint32 cart_car_4_front_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_4_front_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_4_front_axle_model = {
#include "cart/traffic_textures/cart_car_4_front_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_4_front_axle = {
#include "cart/traffic_textures/cart_car_4_front_axle.inc"
};
static Sint16 cart_car_4_rear_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_4_rear_axle_polygons.inc"
};
static Sint32 cart_car_4_rear_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_4_rear_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_4_rear_axle_model = {
#include "cart/traffic_textures/cart_car_4_rear_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_4_rear_axle = {
#include "cart/traffic_textures/cart_car_4_rear_axle.inc"
};
static Sint16 cart_car_4_body_polygons[943] = {
#include "cart/traffic_textures/cart_car_4_body_polygons.inc"
};
static Sint32 cart_car_4_body_vertices[675] = {
#include "cart/traffic_textures/cart_car_4_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_4_body_model = {
#include "cart/traffic_textures/cart_car_4_body_model.inc"
};
NJS_CNK_OBJECT cart_traffic_model_4 = {
#include "cart/traffic_textures/cart_traffic_model_4.inc"
};
static Sint16 cart_car_4_combined_body_polygons[1001] = {
#include "cart/traffic_textures/cart_car_4_combined_body_polygons.inc"
};
static Sint32 cart_car_4_combined_body_vertices[739] = {
#include "cart/traffic_textures/cart_car_4_combined_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_4_combined_body_model = {
#include "cart/traffic_textures/cart_car_4_combined_body_model.inc"
};
static NJS_CNK_OBJECT cart_car_4_combined_body = {
#include "cart/traffic_textures/cart_car_4_combined_body.inc"
};
static Sint16 cart_car_5_front_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_5_front_axle_polygons.inc"
};
static Sint32 cart_car_5_front_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_5_front_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_5_front_axle_model = {
#include "cart/traffic_textures/cart_car_5_front_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_5_front_axle = {
#include "cart/traffic_textures/cart_car_5_front_axle.inc"
};
static Sint16 cart_car_5_rear_axle_polygons[39] = {
#include "cart/traffic_textures/cart_car_5_rear_axle_polygons.inc"
};
static Sint32 cart_car_5_rear_axle_vertices[35] = {
#include "cart/traffic_textures/cart_car_5_rear_axle_vertices.inc"
};
static NJS_CNK_MODEL cart_car_5_rear_axle_model = {
#include "cart/traffic_textures/cart_car_5_rear_axle_model.inc"
};
static NJS_CNK_OBJECT cart_car_5_rear_axle = {
#include "cart/traffic_textures/cart_car_5_rear_axle.inc"
};
static Sint16 cart_car_5_body_polygons[851] = {
#include "cart/traffic_textures/cart_car_5_body_polygons.inc"
};
static Sint32 cart_car_5_body_vertices[683] = {
#include "cart/traffic_textures/cart_car_5_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_5_body_model = {
#include "cart/traffic_textures/cart_car_5_body_model.inc"
};
NJS_CNK_OBJECT cart_traffic_model_5 = {
#include "cart/traffic_textures/cart_traffic_model_5.inc"
};
static Sint16 cart_car_5_combined_body_polygons[909] = {
#include "cart/traffic_textures/cart_car_5_combined_body_polygons.inc"
};
static Sint32 cart_car_5_combined_body_vertices[747] = {
#include "cart/traffic_textures/cart_car_5_combined_body_vertices.inc"
};
static NJS_CNK_MODEL cart_car_5_combined_body_model = {
#include "cart/traffic_textures/cart_car_5_combined_body_model.inc"
};
static NJS_CNK_OBJECT cart_car_5_combined_body = {
#include "cart/traffic_textures/cart_car_5_combined_body.inc"
};
NJS_POINT3 cart_tails_lane_0[12234] = {
#include "cart/traffic_textures/cart_tails_lane_0.inc"
};
NJS_POINT3 cart_tails_lane_1[10199] = {
#include "cart/traffic_textures/cart_tails_lane_1.inc"
};
NJS_POINT3 cart_tails_hd_lane_0[9201] = {
#include "cart/traffic_textures/cart_tails_hd_lane_0.inc"
};
NJS_POINT3 cart_tails_hd_lane_1[8757] = {
#include "cart/traffic_textures/cart_tails_hd_lane_1.inc"
};
NJS_POINT3 cart_rouge_lane_0[13209] = {
#include "cart/traffic_textures/cart_rouge_lane_0.inc"
};
NJS_POINT3 cart_rouge_lane_1[12900] = {
#include "cart/traffic_textures/cart_rouge_lane_1.inc"
};
NJS_POINT3 cart_rouge_hd_lane_0[10764] = {
#include "cart/traffic_textures/cart_rouge_hd_lane_0.inc"
};
NJS_POINT3 cart_rouge_hd_lane_1[10700] = {
#include "cart/traffic_textures/cart_rouge_hd_lane_1.inc"
};
