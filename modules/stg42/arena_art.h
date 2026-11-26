/* The two rotating star fields, Earth, the tiled course below and the far
 * horizon. */
#ifndef STG42_ARENA_ART_H
#define STG42_ARENA_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME versus_sonic_2_background_pvm_textures[21] = {
#include "stg42/arena_art/versus_sonic_2_background_pvm_textures.inc"
};

static NJS_TEXLIST versus_sonic_2_background_pvm_texlist = {
#include "stg42/arena_art/versus_sonic_2_background_pvm_texlist.inc"
};

static NJS_TEXNAME turning_b_textures[1] = {
#include "stg42/arena_art/turning_b_textures.inc"
};

static NJS_TEXLIST turning_b_texlist = {
#include "stg42/arena_art/turning_b_texlist.inc"
};

#include "shared/celestial/orbit_64_chase.h"

static NJS_CNK_OBJECT turning_b_object = {
#include "stg42/arena_art/turning_b_object.inc"
};

static NJS_TEXNAME turning_a_textures[2] = {
#include "stg42/arena_art/turning_a_textures.inc"
};

static NJS_TEXLIST turning_a_texlist = {
#include "stg42/arena_art/turning_a_texlist.inc"
};

#include "shared/celestial/orbit_128.h"

static NJS_CNK_OBJECT turning_a_object = {
#include "stg42/arena_art/turning_a_object.inc"
};

static NJS_TEXNAME upper_textures[2] = {
#include "stg42/arena_art/upper_textures.inc"
};

static NJS_TEXLIST upper_texlist = {
#include "stg42/arena_art/upper_texlist.inc"
};

#include "shared/celestial/earth.h"

static NJS_CNK_OBJECT upper_object = {
#include "stg42/arena_art/upper_object.inc"
};

static NJS_TEXNAME floor_textures[15] = {
#include "stg42/arena_art/floor_textures.inc"
};

static NJS_TEXLIST floor_texlist = {
#include "stg42/arena_art/floor_texlist.inc"
};

static Sint16 floor_polygons[4245] = {
#include "stg42/arena_art/floor_polygons.inc"
};

static Sint32 floor_vertices[3923] = {
#include "stg42/arena_art/floor_vertices.inc"
};

static NJS_CNK_MODEL floor_model = {
#include "stg42/arena_art/floor_model.inc"
};

static NJS_CNK_OBJECT floor_object = {
#include "stg42/arena_art/floor_object.inc"
};

static NJS_TEXNAME around_textures[2] = {
#include "stg42/arena_art/around_textures.inc"
};

static NJS_TEXLIST around_texlist = {
#include "stg42/arena_art/around_texlist.inc"
};

static Sint16 around_polygons[175] = {
#include "stg42/arena_art/around_polygons.inc"
};

static Sint32 around_vertices[103] = {
#include "stg42/arena_art/around_vertices.inc"
};

static NJS_CNK_MODEL around_model = {
#include "stg42/arena_art/around_model.inc"
};

static NJS_CNK_OBJECT around_object = {
#include "stg42/arena_art/around_object.inc"
};

#endif
