#ifndef STG32_METEOR_MODELS_H
#define STG32_METEOR_MODELS_H

/* The stage meteor models and their individual texture bindings. */
#include <ninja.h>

static NJS_TEXNAME meteor_rock_a_textures[2] = {
#include "stg32/meteor_models/meteor_rock_a_textures.inc"
};

static NJS_TEXLIST meteor_rock_a_texlist = {
#include "stg32/meteor_models/meteor_rock_a_texlist.inc"
};

static Sint16 meteor_rock_a_polygons[339] = {
#include "stg32/meteor_models/meteor_rock_a_polygons.inc"
};

static Sint32 meteor_rock_a_vertices[177] = {
#include "stg32/meteor_models/meteor_rock_a_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_a_mesh = {
#include "stg32/meteor_models/meteor_rock_a_mesh.inc"
};

static NJS_CNK_OBJECT meteor_rock_a[1] = {
#include "stg32/meteor_models/meteor_rock_a.inc"
};

static NJS_TEXNAME meteor_rock_b_textures[2] = {
#include "stg32/meteor_models/meteor_rock_b_textures.inc"
};

static NJS_TEXLIST meteor_rock_b_texlist = {
#include "stg32/meteor_models/meteor_rock_b_texlist.inc"
};

static Sint16 meteor_rock_b_polygons[349] = {
#include "stg32/meteor_models/meteor_rock_b_polygons.inc"
};

static Sint32 meteor_rock_b_vertices[195] = {
#include "stg32/meteor_models/meteor_rock_b_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_b_mesh = {
#include "stg32/meteor_models/meteor_rock_b_mesh.inc"
};

static NJS_CNK_OBJECT meteor_rock_b[1] = {
#include "stg32/meteor_models/meteor_rock_b.inc"
};

static NJS_TEXNAME meteor_rock_a_chip_textures[2] = {
#include "stg32/meteor_models/meteor_rock_a_chip_textures.inc"
};

static NJS_TEXLIST meteor_rock_a_chip_texlist = {
#include "stg32/meteor_models/meteor_rock_a_chip_texlist.inc"
};

static Sint16 meteor_rock_a_chip_polygons[127] = {
#include "stg32/meteor_models/meteor_rock_a_chip_polygons.inc"
};

static Sint32 meteor_rock_a_chip_vertices[75] = {
#include "stg32/meteor_models/meteor_rock_a_chip_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_a_chip_mesh = {
#include "stg32/meteor_models/meteor_rock_a_chip_mesh.inc"
};

static NJS_CNK_OBJECT meteor_rock_a_chip[1] = {
#include "stg32/meteor_models/meteor_rock_a_chip.inc"
};

static NJS_TEXNAME meteor_rock_b_chip_textures[2] = {
#include "stg32/meteor_models/meteor_rock_b_chip_textures.inc"
};

static NJS_TEXLIST meteor_rock_b_chip_texlist = {
#include "stg32/meteor_models/meteor_rock_b_chip_texlist.inc"
};

static Sint16 meteor_rock_b_chip_polygons[137] = {
#include "stg32/meteor_models/meteor_rock_b_chip_polygons.inc"
};

static Sint32 meteor_rock_b_chip_vertices[81] = {
#include "stg32/meteor_models/meteor_rock_b_chip_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_b_chip_mesh = {
#include "stg32/meteor_models/meteor_rock_b_chip_mesh.inc"
};

static NJS_CNK_OBJECT meteor_rock_b_chip[1] = {
#include "stg32/meteor_models/meteor_rock_b_chip.inc"
};

static NJS_TEXNAME meteor_rock_chip_textures[1] = {
#include "stg32/meteor_models/meteor_rock_chip_textures.inc"
};

static NJS_TEXLIST meteor_rock_chip_texlist = {
#include "stg32/meteor_models/meteor_rock_chip_texlist.inc"
};

static Sint16 meteor_rock_chip_polygons[91] = {
#include "stg32/meteor_models/meteor_rock_chip_polygons.inc"
};

static Sint32 meteor_rock_chip_vertices[63] = {
#include "stg32/meteor_models/meteor_rock_chip_vertices.inc"
};

static NJS_CNK_MODEL meteor_rock_chip_mesh = {
#include "stg32/meteor_models/meteor_rock_chip_mesh.inc"
};

static NJS_CNK_OBJECT meteor_rock_chip[1] = {
#include "stg32/meteor_models/meteor_rock_chip.inc"
};

static NJS_TEXNAME herd_falling_meteor_textures[1] = {
#include "stg32/meteor_models/herd_falling_meteor_textures.inc"
};

static NJS_TEXLIST herd_falling_meteor_texlist = {
#include "stg32/meteor_models/herd_falling_meteor_texlist.inc"
};

#include "shared/meteor_shapes/meteor_falling.h"

static NJS_CNK_OBJECT herd_falling_meteor_object[1] = {
#include "stg32/meteor_models/herd_falling_meteor_object.inc"
};

static NJS_TEXNAME meteor_fireball_textures[1] = {
#include "stg32/meteor_models/meteor_fireball_textures.inc"
};

static NJS_TEXLIST meteor_fireball_texlist = {
#include "stg32/meteor_models/meteor_fireball_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fireball.h"

static NJS_CNK_OBJECT meteor_fireball_object[1] = {
#include "stg32/meteor_models/meteor_fireball_object.inc"
};

static NJS_TEXNAME meteor_fireball_shard_textures[2] = {
#include "stg32/meteor_models/meteor_fireball_shard_textures.inc"
};

static NJS_TEXLIST meteor_fireball_shard_texlist = {
#include "stg32/meteor_models/meteor_fireball_shard_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fragment.h"

static NJS_CNK_OBJECT meteor_fireball_shard[1] = {
#include "stg32/meteor_models/meteor_fireball_shard.inc"
};

static NJS_TEXNAME meteor_fireball_chip_textures[2] = {
#include "stg32/meteor_models/meteor_fireball_chip_textures.inc"
};

static NJS_TEXLIST meteor_fireball_chip_texlist = {
#include "stg32/meteor_models/meteor_fireball_chip_texlist.inc"
};

#include "shared/meteor_shapes/meteor_shard.h"

static NJS_CNK_OBJECT meteor_fireball_chip[1] = {
#include "stg32/meteor_models/meteor_fireball_chip.inc"
};

static Sint16 meteor_fireball_shadow_polygons[59] = {
#include "stg32/meteor_models/meteor_fireball_shadow_polygons.inc"
};

struct herd_fireball_shadow_vertices_layout {
    Sint32 head[2];
    NJS_POINT3 vertices[11];
    Sint32 end;
};

static struct herd_fireball_shadow_vertices_layout
    meteor_fireball_shadow_vertices = {
#include "stg32/meteor_models/meteor_fireball_shadow_vertices.inc"
};

static NJS_CNK_MODEL meteor_fireball_shadow_mesh = {
#include "stg32/meteor_models/meteor_fireball_shadow_mesh.inc"
};

#endif
