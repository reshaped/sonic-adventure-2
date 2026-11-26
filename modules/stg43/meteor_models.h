#ifndef STG43_METEOR_MODELS_H
#define STG43_METEOR_MODELS_H

/* The stage meteor models and their individual texture bindings. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME meteor_flame_textures[1] = {
#include "stg43/meteor_models/meteor_flame_textures.inc"
};

static NJS_TEXLIST meteor_flame_texlist = {
#include "stg43/meteor_models/meteor_flame_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fireball.h"

static NJS_CNK_OBJECT meteor_flame_object[1] = {
#include "stg43/meteor_models/meteor_flame_object.inc"
};

static NJS_TEXNAME meteor_flame_chip_textures[2] = {
#include "stg43/meteor_models/meteor_flame_chip_textures.inc"
};

static NJS_TEXLIST meteor_flame_chip_texlist = {
#include "stg43/meteor_models/meteor_flame_chip_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fragment.h"

static NJS_CNK_OBJECT meteor_flame_chip[1] = {
#include "stg43/meteor_models/meteor_flame_chip.inc"
};

static NJS_TEXNAME meteor_flame_shard_textures[2] = {
#include "stg43/meteor_models/meteor_flame_shard_textures.inc"
};

static NJS_TEXLIST meteor_flame_shard_texlist = {
#include "stg43/meteor_models/meteor_flame_shard_texlist.inc"
};

#include "shared/meteor_shapes/meteor_shard.h"

static NJS_CNK_OBJECT meteor_flame_shard[1] = {
#include "stg43/meteor_models/meteor_flame_shard.inc"
};

static NJS_TEXNAME meteor_large_textures[2] = {
#include "stg43/meteor_models/meteor_large_textures.inc"
};

static NJS_TEXLIST meteor_large_texlist = {
#include "stg43/meteor_models/meteor_large_texlist.inc"
};

static Sint16 meteor_large_polygons[351] = {
#include "stg43/meteor_models/meteor_large_polygons.inc"
};

static Sint32 meteor_large_vertices[195] = {
#include "stg43/meteor_models/meteor_large_vertices.inc"
};

static NJS_CNK_MODEL meteor_large_mesh = {
#include "stg43/meteor_models/meteor_large_mesh.inc"
};

static NJS_CNK_OBJECT meteor_large_object[1] = {
#include "stg43/meteor_models/meteor_large_object.inc"
};

static NJS_TEXNAME meteor_small_textures[2] = {
#include "stg43/meteor_models/meteor_small_textures.inc"
};

static NJS_TEXLIST meteor_small_texlist = {
#include "stg43/meteor_models/meteor_small_texlist.inc"
};

static Sint16 meteor_small_polygons[249] = {
#include "stg43/meteor_models/meteor_small_polygons.inc"
};

static Sint32 meteor_small_vertices[135] = {
#include "stg43/meteor_models/meteor_small_vertices.inc"
};

static NJS_CNK_MODEL meteor_small_mesh = {
#include "stg43/meteor_models/meteor_small_mesh.inc"
};

static NJS_CNK_OBJECT meteor_small_object[1] = {
#include "stg43/meteor_models/meteor_small_object.inc"
};

static NJS_TEXNAME meteor_chip_textures[2] = {
#include "stg43/meteor_models/meteor_chip_textures.inc"
};

static NJS_TEXLIST meteor_chip_texlist = {
#include "stg43/meteor_models/meteor_chip_texlist.inc"
};

static Sint16 meteor_chip_polygons[133] = {
#include "stg43/meteor_models/meteor_chip_polygons.inc"
};

static Sint32 meteor_chip_vertices[81] = {
#include "stg43/meteor_models/meteor_chip_vertices.inc"
};

static NJS_CNK_MODEL meteor_chip_mesh = {
#include "stg43/meteor_models/meteor_chip_mesh.inc"
};

static NJS_CNK_OBJECT meteor_chip[1] = {
#include "stg43/meteor_models/meteor_chip.inc"
};

static NJS_TEXNAME meteor_shard_textures[1] = {
#include "stg43/meteor_models/meteor_shard_textures.inc"
};

static NJS_TEXLIST meteor_shard_texlist = {
#include "stg43/meteor_models/meteor_shard_texlist.inc"
};

static Sint16 cosmic_rock_shard_polygons[89] = {
#include "stg43/meteor_models/cosmic_rock_shard_polygons.inc"
};

static Sint32 cosmic_rock_shard_vertices[63] = {
#include "stg43/meteor_models/cosmic_rock_shard_vertices.inc"
};

static NJS_CNK_MODEL cosmic_rock_shard_mesh = {
#include "stg43/meteor_models/cosmic_rock_shard_mesh.inc"
};

static NJS_CNK_OBJECT meteor_shard[1] = {
#include "stg43/meteor_models/meteor_shard.inc"
};

static NJS_TEXNAME meteor_small_shard_textures[2] = {
#include "stg43/meteor_models/meteor_small_shard_textures.inc"
};

static NJS_TEXLIST meteor_small_shard_texlist = {
#include "stg43/meteor_models/meteor_small_shard_texlist.inc"
};

static Sint16 meteor_small_shard_polygons[127] = {
#include "stg43/meteor_models/meteor_small_shard_polygons.inc"
};

static Sint32 meteor_small_shard_vertices[75] = {
#include "stg43/meteor_models/meteor_small_shard_vertices.inc"
};

static NJS_CNK_MODEL meteor_small_shard_mesh = {
#include "stg43/meteor_models/meteor_small_shard_mesh.inc"
};

static NJS_CNK_OBJECT meteor_small_shard[1] = {
#include "stg43/meteor_models/meteor_small_shard.inc"
};

#endif
