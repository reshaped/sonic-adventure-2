/* the speed characters' effect texture lists, and the flame ring's fire */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "effect/sonic_effect_assets.h"

#include "effect/sonic_effect_assets/text.inc"

NJS_TEXNAME s_efftex_texname[SPEED_EFFECT_TEXTURES] = {
#include "effect/sonic_effect_assets/s_efftex_texname.inc"
};

NJS_TEXLIST s_efftex_texlist = {
#include "effect/sonic_effect_assets/s_efftex_texlist.inc"
};

NJS_TEXNAME sh_efftex_texname[SPEED_EFFECT_TEXTURES] = {
#include "effect/sonic_effect_assets/sh_efftex_texname.inc"
};

NJS_TEXLIST sh_efftex_texlist = {
#include "effect/sonic_effect_assets/sh_efftex_texlist.inc"
};

/* the ring is RING_BANDS strips around its eight sides. each strip has
 * BAND_CORNERS corners, nine pairs with the first pair again at the seam. a
 * corner is a vertex index, a u and a v. flame_ring_uvs holds one key, the
 * unscrolled u and v of every corner */
#define RING_BANDS 5
#define BAND_CORNERS 18
#define BAND_SHORTS (1 + (3 * BAND_CORNERS))
#define FIRST_BAND 11
#define RING_UV_SHORTS (2 + (RING_BANDS * (2 + (2 * BAND_CORNERS))))
#define RING_POLYGON_SHORTS (FIRST_BAND + (RING_BANDS * BAND_SHORTS) + 1)

static Sint16 flame_ring_uvs[RING_UV_SHORTS] = {
#include "effect/sonic_effect_assets/flame_ring_uvs.inc"
};

struct texture_anim flame_ring_anim = {
#include "effect/sonic_effect_assets/flame_ring_anim.inc"
};

static Sint16 flame_ring_polygons[RING_POLYGON_SHORTS] = {
#include "effect/sonic_effect_assets/flame_ring_polygons.inc"
};

#define RING_VERTICES 48
static Sint32 flame_ring_vertices[195] = {
#include "effect/sonic_effect_assets/flame_ring_vertices.inc"
};

NJS_CNK_MODEL flame_ring_model = {
#include "effect/sonic_effect_assets/flame_ring_model.inc"
};

NJS_CNK_OBJECT flame_ring_object = {
#include "effect/sonic_effect_assets/flame_ring_object.inc"
};
