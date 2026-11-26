/* Harbor missile piece1 geometry. */
#ifndef STG10_STRUCTURES_HARBOR_MISSILE_PIECE1_H
#define STG10_STRUCTURES_HARBOR_MISSILE_PIECE1_H
#include <ninja.h>
#include <sg_xpt.h>

enum harbor_missile_piece1_texture_slot {
    HARBOR_MISSILE_PIECE1_TEX_MIU128_MH001,
    HARBOR_MISSILE_PIECE1_TEX_MIU64_MH002,
    HARBOR_MISSILE_PIECE1_TEX_MIU64_MH003,
    HARBOR_MISSILE_PIECE1_TEX_MIU64_MH006,
    HARBOR_MISSILE_PIECE1_TEX_PR128_8,
    HARBOR_MISSILE_PIECE1_TEX_PR32_1,
    HARBOR_MISSILE_PIECE1_TEX_PR32_3,
    HARBOR_MISSILE_PIECE1_TEX_PR64_14,
    HARBOR_MISSILE_PIECE1_TEXTURE_COUNT
};

static NJS_TEXNAME
    harbor_missile_piece1_names[HARBOR_MISSILE_PIECE1_TEXTURE_COUNT] = {
#include "stg10/structures/harbor_missile_piece1/harbor_missile_piece1_names.inc"
};

static NJS_TEXLIST harbor_missile_piece1_texlist = {
#include "stg10/structures/harbor_missile_piece1/harbor_missile_piece1_texlist.inc"
};

static Sint16 harbor_missile_piece1_polygons[495] = {
#include "stg10/structures/harbor_missile_piece1/harbor_missile_piece1_polygons.inc"
};

static Sint32 harbor_missile_piece1_vertices[499] = {
#include "stg10/structures/harbor_missile_piece1/harbor_missile_piece1_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_piece1_model = {
#include "stg10/structures/harbor_missile_piece1/harbor_missile_piece1_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_piece1_object = {
#include "stg10/structures/harbor_missile_piece1/harbor_missile_piece1_object.inc"
};

#endif
