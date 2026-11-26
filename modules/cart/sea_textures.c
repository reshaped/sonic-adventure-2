#include <ninja.h>
#include <sg_xpt.h>

#include "cart/background_art.h"
#include "cart/shared_texture_names.h"

/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
const char cart_floor_texture_filenames[640] = {
#include "cart/sea_textures/cart_floor_texture_filenames.inc"
};

/* CART.PRS's chunk models and texture lists, from the disc. its D lies
 * between pads.c and background.c */
static Sint16 floor_polygons[677] = {
#include "cart/sea_textures/floor_polygons.inc"
};
static Sint32 floor_vertices[487] = {
#include "cart/sea_textures/floor_vertices.inc"
};
static NJS_CNK_MODEL floor_model = {
#include "cart/sea_textures/floor_model.inc"
};
NJS_CNK_OBJECT cart_floor[1] = {
#include "cart/sea_textures/cart_floor.inc"
};
static NJS_TEXNAME floor_slots[20] = {
#include "cart/sea_textures/floor_slots.inc"
};
NJS_TEXLIST cart_floor_textures = {
#include "cart/sea_textures/cart_floor_textures.inc"
};
static Sint16 sky_polygons[839] = {
#include "cart/sea_textures/sky_polygons.inc"
};
static Sint32 sky_vertices[459] = {
#include "cart/sea_textures/sky_vertices.inc"
};
static NJS_CNK_MODEL sky_model = {
#include "cart/sea_textures/sky_model.inc"
};
NJS_CNK_OBJECT cart_sky = {
#include "cart/sea_textures/cart_sky.inc"
};
static NJS_TEXNAME sky_slots[2] = {
#include "cart/sea_textures/sky_slots.inc"
};
NJS_TEXLIST cart_sky_textures = {
#include "cart/sea_textures/cart_sky_textures.inc"
};
