/* CART.PRS's Basic models, from the disc. its D lies between dat_8c7a1df4.c
 * and jump_ramp.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/ramp_art.h"

static NJS_MATERIAL ramp_material[1] = {
#include "cart/ramp_art/ramp_material.inc"
};
static Sint16 ramp_indices[9] = {
#include "cart/ramp_art/ramp_indices.inc"
};
static NJS_MESHSET ramp_strips[1] = {
#include "cart/ramp_art/ramp_strips.inc"
};
static NJS_POINT3 ramp_points[6] = {
#include "cart/ramp_art/ramp_points.inc"
};
static NJS_POINT3 ramp_normals[6] = {
#include "cart/ramp_art/ramp_normals.inc"
};
static NJS_MODEL ramp_model = {
#include "cart/ramp_art/ramp_model.inc"
};
NJS_OBJECT cart_jump_land = {
#include "cart/ramp_art/cart_jump_land.inc"
};
