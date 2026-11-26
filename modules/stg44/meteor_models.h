#ifndef STG44_METEOR_MODELS_H
#define STG44_METEOR_MODELS_H

/* The stage meteor models and their individual texture bindings. */
#include <ninja.h>

static NJS_TEXNAME fireball_surface_textures[1] = {
#include "stg44/meteor_models/fireball_surface_textures.inc"
};

static NJS_TEXLIST mad_space_fireball_texlist = {
#include "stg44/meteor_models/mad_space_fireball_texlist.inc"
};

static Sint16 mad_space_fireball_polygons[169] = {
#include "stg44/meteor_models/mad_space_fireball_polygons.inc"
};

static Sint32 mad_space_fireball_vertices[103] = {
#include "stg44/meteor_models/mad_space_fireball_vertices.inc"
};

static NJS_CNK_MODEL mad_space_fireball_mesh = {
#include "stg44/meteor_models/mad_space_fireball_mesh.inc"
};

static NJS_CNK_OBJECT mad_space_fireball_object[1] = {
#include "stg44/meteor_models/mad_space_fireball_object.inc"
};

static NJS_TEXNAME mad_space_orbit_textures[1] = {
#include "stg44/meteor_models/mad_space_orbit_textures.inc"
};

static NJS_TEXLIST mad_space_orbit_texlist = {
#include "stg44/meteor_models/mad_space_orbit_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fireball.h"

static NJS_CNK_OBJECT mad_space_orbit_object[1] = {
#include "stg44/meteor_models/mad_space_orbit_object.inc"
};

static NJS_TEXNAME mad_space_fireball_piece_textures[2] = {
#include "stg44/meteor_models/mad_space_fireball_piece_textures.inc"
};

static NJS_TEXLIST mad_space_fireball_piece_texlist = {
#include "stg44/meteor_models/mad_space_fireball_piece_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fragment.h"

static NJS_CNK_OBJECT mad_space_fireball_piece[1] = {
#include "stg44/meteor_models/mad_space_fireball_piece.inc"
};

static NJS_TEXNAME mad_space_fireball_shard_textures[2] = {
#include "stg44/meteor_models/mad_space_fireball_shard_textures.inc"
};

static NJS_TEXLIST mad_space_fireball_shard_texlist = {
#include "stg44/meteor_models/mad_space_fireball_shard_texlist.inc"
};

#include "shared/meteor_shapes/meteor_shard.h"

static NJS_CNK_OBJECT mad_space_fireball_shard[1] = {
#include "stg44/meteor_models/mad_space_fireball_shard.inc"
};

#endif
