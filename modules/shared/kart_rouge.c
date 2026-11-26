/* the chunk models of ADVERTISE.PRS and CART.PRS, from the disc. its D's
 * neighbours differ by image */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/kart_models.h"

static Sint16 kart_rouge_driver_detail_polygons[2265] = {
#include "shared/kart_rouge/kart_rouge_driver_detail_polygons.inc"
};
static Sint32 kart_rouge_driver_detail_vertices[2139] = {
#include "shared/kart_rouge/kart_rouge_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_driver_detail_model = {
#include "shared/kart_rouge/kart_rouge_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_driver_detail = {
#include "shared/kart_rouge/kart_rouge_driver_detail.inc"
};
static Sint16 kart_rouge_driver_polygons[1403] = {
#include "shared/kart_rouge/kart_rouge_driver_polygons.inc"
};
static Sint32 kart_rouge_driver_vertices[1005] = {
#include "shared/kart_rouge/kart_rouge_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_driver_model = {
#include "shared/kart_rouge/kart_rouge_driver_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_driver = {
#include "shared/kart_rouge/kart_rouge_driver.inc"
};
static Sint16 kart_rouge_chassis_polygons[2451] = {
#include "shared/kart_rouge/kart_rouge_chassis_polygons.inc"
};
static Sint32 kart_rouge_chassis_vertices[2223] = {
#include "shared/kart_rouge/kart_rouge_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_chassis_model = {
#include "shared/kart_rouge/kart_rouge_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_chassis = {
#include "shared/kart_rouge/kart_rouge_chassis.inc"
};
static Sint16 kart_rouge_paired_wheels_polygons[289] = {
#include "shared/kart_rouge/kart_rouge_paired_wheels_polygons.inc"
};
static Sint32 kart_rouge_paired_wheels_vertices[219] = {
#include "shared/kart_rouge/kart_rouge_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_paired_wheels_model = {
#include "shared/kart_rouge/kart_rouge_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_paired_wheels = {
#include "shared/kart_rouge/kart_rouge_paired_wheels.inc"
};
static Sint16 kart_rouge_right_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_rouge_right_wheel_polygons.inc"
};
static Sint32 kart_rouge_right_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_rouge_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_right_wheel_model = {
#include "shared/kart_rouge/kart_rouge_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_right_wheel = {
#include "shared/kart_rouge/kart_rouge_right_wheel.inc"
};
static Sint16 kart_rouge_left_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_rouge_left_wheel_polygons.inc"
};
static Sint32 kart_rouge_left_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_rouge_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_rouge_left_wheel_model = {
#include "shared/kart_rouge/kart_rouge_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_rouge_left_wheel = {
#include "shared/kart_rouge/kart_rouge_left_wheel.inc"
};
NJS_CNK_OBJECT cart_rouge_model = {
#include "shared/kart_rouge/cart_rouge_model.inc"
};
static Sint16 kart_shadow_driver_detail_polygons[3257] = {
#include "shared/kart_rouge/kart_shadow_driver_detail_polygons.inc"
};
static Sint32 kart_shadow_driver_detail_vertices[2157] = {
#include "shared/kart_rouge/kart_shadow_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_shadow_driver_detail_model = {
#include "shared/kart_rouge/kart_shadow_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_shadow_driver_detail = {
#include "shared/kart_rouge/kart_shadow_driver_detail.inc"
};
static Sint16 kart_shadow_driver_polygons[1875] = {
#include "shared/kart_rouge/kart_shadow_driver_polygons.inc"
};
static Sint32 kart_shadow_driver_vertices[1473] = {
#include "shared/kart_rouge/kart_shadow_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_shadow_driver_model = {
#include "shared/kart_rouge/kart_shadow_driver_model.inc"
};
static NJS_CNK_OBJECT kart_shadow_driver = {
#include "shared/kart_rouge/kart_shadow_driver.inc"
};
static Sint16 kart_shadow_chassis_polygons[2451] = {
#include "shared/kart_rouge/kart_shadow_chassis_polygons.inc"
};
static Sint32 kart_shadow_chassis_vertices[2223] = {
#include "shared/kart_rouge/kart_shadow_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_shadow_chassis_model = {
#include "shared/kart_rouge/kart_shadow_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_shadow_chassis = {
#include "shared/kart_rouge/kart_shadow_chassis.inc"
};
static Sint16 kart_shadow_paired_wheels_polygons[289] = {
#include "shared/kart_rouge/kart_shadow_paired_wheels_polygons.inc"
};
static Sint32 kart_shadow_paired_wheels_vertices[219] = {
#include "shared/kart_rouge/kart_shadow_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_shadow_paired_wheels_model = {
#include "shared/kart_rouge/kart_shadow_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_shadow_paired_wheels = {
#include "shared/kart_rouge/kart_shadow_paired_wheels.inc"
};
static Sint16 kart_shadow_right_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_shadow_right_wheel_polygons.inc"
};
static Sint32 kart_shadow_right_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_shadow_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_shadow_right_wheel_model = {
#include "shared/kart_rouge/kart_shadow_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_shadow_right_wheel = {
#include "shared/kart_rouge/kart_shadow_right_wheel.inc"
};
static Sint16 kart_shadow_left_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_shadow_left_wheel_polygons.inc"
};
static Sint32 kart_shadow_left_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_shadow_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_shadow_left_wheel_model = {
#include "shared/kart_rouge/kart_shadow_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_shadow_left_wheel = {
#include "shared/kart_rouge/kart_shadow_left_wheel.inc"
};
NJS_CNK_OBJECT cart_shadow_model = {
#include "shared/kart_rouge/cart_shadow_model.inc"
};
static Sint16 kart_sonic_driver_detail_polygons[2385] = {
#include "shared/kart_rouge/kart_sonic_driver_detail_polygons.inc"
};
static Sint32 kart_sonic_driver_detail_vertices[1941] = {
#include "shared/kart_rouge/kart_sonic_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_sonic_driver_detail_model = {
#include "shared/kart_rouge/kart_sonic_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_sonic_driver_detail = {
#include "shared/kart_rouge/kart_sonic_driver_detail.inc"
};
static Sint16 kart_sonic_driver_polygons[1351] = {
#include "shared/kart_rouge/kart_sonic_driver_polygons.inc"
};
static Sint32 kart_sonic_driver_vertices[1083] = {
#include "shared/kart_rouge/kart_sonic_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_sonic_driver_model = {
#include "shared/kart_rouge/kart_sonic_driver_model.inc"
};
static NJS_CNK_OBJECT kart_sonic_driver = {
#include "shared/kart_rouge/kart_sonic_driver.inc"
};
static Sint16 kart_sonic_chassis_polygons[2451] = {
#include "shared/kart_rouge/kart_sonic_chassis_polygons.inc"
};
static Sint32 kart_sonic_chassis_vertices[2223] = {
#include "shared/kart_rouge/kart_sonic_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_sonic_chassis_model = {
#include "shared/kart_rouge/kart_sonic_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_sonic_chassis = {
#include "shared/kart_rouge/kart_sonic_chassis.inc"
};
static Sint16 kart_sonic_paired_wheels_polygons[289] = {
#include "shared/kart_rouge/kart_sonic_paired_wheels_polygons.inc"
};
static Sint32 kart_sonic_paired_wheels_vertices[219] = {
#include "shared/kart_rouge/kart_sonic_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_sonic_paired_wheels_model = {
#include "shared/kart_rouge/kart_sonic_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_sonic_paired_wheels = {
#include "shared/kart_rouge/kart_sonic_paired_wheels.inc"
};
static Sint16 kart_sonic_right_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_sonic_right_wheel_polygons.inc"
};
static Sint32 kart_sonic_right_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_sonic_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_sonic_right_wheel_model = {
#include "shared/kart_rouge/kart_sonic_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_sonic_right_wheel = {
#include "shared/kart_rouge/kart_sonic_right_wheel.inc"
};
static Sint16 kart_sonic_left_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_sonic_left_wheel_polygons.inc"
};
static Sint32 kart_sonic_left_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_sonic_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_sonic_left_wheel_model = {
#include "shared/kart_rouge/kart_sonic_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_sonic_left_wheel = {
#include "shared/kart_rouge/kart_sonic_left_wheel.inc"
};
NJS_CNK_OBJECT cart_sonic_model = {
#include "shared/kart_rouge/cart_sonic_model.inc"
};
static Sint16 kart_tails_driver_detail_polygons[1797] = {
#include "shared/kart_rouge/kart_tails_driver_detail_polygons.inc"
};
static Sint32 kart_tails_driver_detail_vertices[1401] = {
#include "shared/kart_rouge/kart_tails_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_tails_driver_detail_model = {
#include "shared/kart_rouge/kart_tails_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_tails_driver_detail = {
#include "shared/kart_rouge/kart_tails_driver_detail.inc"
};
static Sint16 kart_tails_driver_polygons[1479] = {
#include "shared/kart_rouge/kart_tails_driver_polygons.inc"
};
static Sint32 kart_tails_driver_vertices[1221] = {
#include "shared/kart_rouge/kart_tails_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_tails_driver_model = {
#include "shared/kart_rouge/kart_tails_driver_model.inc"
};
static NJS_CNK_OBJECT kart_tails_driver = {
#include "shared/kart_rouge/kart_tails_driver.inc"
};
static Sint16 kart_tails_chassis_polygons[2457] = {
#include "shared/kart_rouge/kart_tails_chassis_polygons.inc"
};
static Sint32 kart_tails_chassis_vertices[2223] = {
#include "shared/kart_rouge/kart_tails_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_tails_chassis_model = {
#include "shared/kart_rouge/kart_tails_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_tails_chassis = {
#include "shared/kart_rouge/kart_tails_chassis.inc"
};
static Sint16 kart_tails_paired_wheels_polygons[289] = {
#include "shared/kart_rouge/kart_tails_paired_wheels_polygons.inc"
};
static Sint32 kart_tails_paired_wheels_vertices[219] = {
#include "shared/kart_rouge/kart_tails_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_tails_paired_wheels_model = {
#include "shared/kart_rouge/kart_tails_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_tails_paired_wheels = {
#include "shared/kart_rouge/kart_tails_paired_wheels.inc"
};
static Sint16 kart_tails_right_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_tails_right_wheel_polygons.inc"
};
static Sint32 kart_tails_right_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_tails_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_tails_right_wheel_model = {
#include "shared/kart_rouge/kart_tails_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_tails_right_wheel = {
#include "shared/kart_rouge/kart_tails_right_wheel.inc"
};
static Sint16 kart_tails_left_wheel_polygons[151] = {
#include "shared/kart_rouge/kart_tails_left_wheel_polygons.inc"
};
static Sint32 kart_tails_left_wheel_vertices[111] = {
#include "shared/kart_rouge/kart_tails_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_tails_left_wheel_model = {
#include "shared/kart_rouge/kart_tails_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_tails_left_wheel = {
#include "shared/kart_rouge/kart_tails_left_wheel.inc"
};
NJS_CNK_OBJECT cart_tails_model = {
#include "shared/kart_rouge/cart_tails_model.inc"
};
