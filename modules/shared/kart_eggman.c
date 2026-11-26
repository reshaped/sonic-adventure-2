/* the chunk models of ADVERTISE.PRS and CART.PRS, from the disc. its D's
 * neighbours differ by image */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/kart_models.h"

static Sint16 kart_eggman_driver_detail_polygons[1275] = {
#include "shared/kart_eggman/kart_eggman_driver_detail_polygons.inc"
};
static Sint32 kart_eggman_driver_detail_vertices[1209] = {
#include "shared/kart_eggman/kart_eggman_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_eggman_driver_detail_model = {
#include "shared/kart_eggman/kart_eggman_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_eggman_driver_detail = {
#include "shared/kart_eggman/kart_eggman_driver_detail.inc"
};
static Sint16 kart_eggman_driver_polygons[2317] = {
#include "shared/kart_eggman/kart_eggman_driver_polygons.inc"
};
static Sint32 kart_eggman_driver_vertices[2229] = {
#include "shared/kart_eggman/kart_eggman_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_eggman_driver_model = {
#include "shared/kart_eggman/kart_eggman_driver_model.inc"
};
static NJS_CNK_OBJECT kart_eggman_driver = {
#include "shared/kart_eggman/kart_eggman_driver.inc"
};
static Sint16 kart_eggman_chassis_polygons[2465] = {
#include "shared/kart_eggman/kart_eggman_chassis_polygons.inc"
};
static Sint32 kart_eggman_chassis_vertices[2223] = {
#include "shared/kart_eggman/kart_eggman_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_eggman_chassis_model = {
#include "shared/kart_eggman/kart_eggman_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_eggman_chassis = {
#include "shared/kart_eggman/kart_eggman_chassis.inc"
};
static Sint16 kart_eggman_right_wheel_polygons[151] = {
#include "shared/kart_eggman/kart_eggman_right_wheel_polygons.inc"
};
static Sint32 kart_eggman_right_wheel_vertices[111] = {
#include "shared/kart_eggman/kart_eggman_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_eggman_right_wheel_model = {
#include "shared/kart_eggman/kart_eggman_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_eggman_right_wheel = {
#include "shared/kart_eggman/kart_eggman_right_wheel.inc"
};
static Sint16 kart_eggman_paired_wheels_polygons[289] = {
#include "shared/kart_eggman/kart_eggman_paired_wheels_polygons.inc"
};
static Sint32 kart_eggman_paired_wheels_vertices[219] = {
#include "shared/kart_eggman/kart_eggman_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_eggman_paired_wheels_model = {
#include "shared/kart_eggman/kart_eggman_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_eggman_paired_wheels = {
#include "shared/kart_eggman/kart_eggman_paired_wheels.inc"
};
static Sint16 kart_eggman_left_wheel_polygons[151] = {
#include "shared/kart_eggman/kart_eggman_left_wheel_polygons.inc"
};
static Sint32 kart_eggman_left_wheel_vertices[111] = {
#include "shared/kart_eggman/kart_eggman_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_eggman_left_wheel_model = {
#include "shared/kart_eggman/kart_eggman_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_eggman_left_wheel = {
#include "shared/kart_eggman/kart_eggman_left_wheel.inc"
};
NJS_CNK_OBJECT cart_eggman_model = {
#include "shared/kart_eggman/cart_eggman_model.inc"
};
static Sint16 kart_knuckles_driver_detail_polygons[2325] = {
#include "shared/kart_eggman/kart_knuckles_driver_detail_polygons.inc"
};
static Sint32 kart_knuckles_driver_detail_vertices[1953] = {
#include "shared/kart_eggman/kart_knuckles_driver_detail_vertices.inc"
};
static NJS_CNK_MODEL kart_knuckles_driver_detail_model = {
#include "shared/kart_eggman/kart_knuckles_driver_detail_model.inc"
};
static NJS_CNK_OBJECT kart_knuckles_driver_detail = {
#include "shared/kart_eggman/kart_knuckles_driver_detail.inc"
};
static Sint16 kart_knuckles_driver_polygons[1771] = {
#include "shared/kart_eggman/kart_knuckles_driver_polygons.inc"
};
static Sint32 kart_knuckles_driver_vertices[1335] = {
#include "shared/kart_eggman/kart_knuckles_driver_vertices.inc"
};
static NJS_CNK_MODEL kart_knuckles_driver_model = {
#include "shared/kart_eggman/kart_knuckles_driver_model.inc"
};
static NJS_CNK_OBJECT kart_knuckles_driver = {
#include "shared/kart_eggman/kart_knuckles_driver.inc"
};
static Sint16 kart_knuckles_chassis_polygons[2451] = {
#include "shared/kart_eggman/kart_knuckles_chassis_polygons.inc"
};
static Sint32 kart_knuckles_chassis_vertices[2223] = {
#include "shared/kart_eggman/kart_knuckles_chassis_vertices.inc"
};
static NJS_CNK_MODEL kart_knuckles_chassis_model = {
#include "shared/kart_eggman/kart_knuckles_chassis_model.inc"
};
static NJS_CNK_OBJECT kart_knuckles_chassis = {
#include "shared/kart_eggman/kart_knuckles_chassis.inc"
};
static Sint16 kart_knuckles_paired_wheels_polygons[289] = {
#include "shared/kart_eggman/kart_knuckles_paired_wheels_polygons.inc"
};
static Sint32 kart_knuckles_paired_wheels_vertices[219] = {
#include "shared/kart_eggman/kart_knuckles_paired_wheels_vertices.inc"
};
static NJS_CNK_MODEL kart_knuckles_paired_wheels_model = {
#include "shared/kart_eggman/kart_knuckles_paired_wheels_model.inc"
};
static NJS_CNK_OBJECT kart_knuckles_paired_wheels = {
#include "shared/kart_eggman/kart_knuckles_paired_wheels.inc"
};
static Sint16 kart_knuckles_right_wheel_polygons[151] = {
#include "shared/kart_eggman/kart_knuckles_right_wheel_polygons.inc"
};
static Sint32 kart_knuckles_right_wheel_vertices[111] = {
#include "shared/kart_eggman/kart_knuckles_right_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_knuckles_right_wheel_model = {
#include "shared/kart_eggman/kart_knuckles_right_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_knuckles_right_wheel = {
#include "shared/kart_eggman/kart_knuckles_right_wheel.inc"
};
static Sint16 kart_knuckles_left_wheel_polygons[151] = {
#include "shared/kart_eggman/kart_knuckles_left_wheel_polygons.inc"
};
static Sint32 kart_knuckles_left_wheel_vertices[111] = {
#include "shared/kart_eggman/kart_knuckles_left_wheel_vertices.inc"
};
static NJS_CNK_MODEL kart_knuckles_left_wheel_model = {
#include "shared/kart_eggman/kart_knuckles_left_wheel_model.inc"
};
static NJS_CNK_OBJECT kart_knuckles_left_wheel = {
#include "shared/kart_eggman/kart_knuckles_left_wheel.inc"
};
NJS_CNK_OBJECT cart_knuckles_model = {
#include "shared/kart_eggman/cart_knuckles_model.inc"
};
