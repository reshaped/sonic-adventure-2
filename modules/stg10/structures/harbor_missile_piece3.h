/* Harbor missile piece3 geometry. */
#ifndef STG10_STRUCTURES_HARBOR_MISSILE_PIECE3_H
#define STG10_STRUCTURES_HARBOR_MISSILE_PIECE3_H
#include <ninja.h>
#include <sg_xpt.h>

enum harbor_missile_piece3_texture_slot {
    HARBOR_MISSILE_PIECE3_TEX_PR128_22,
    HARBOR_MISSILE_PIECE3_TEX_PR128_5,
    HARBOR_MISSILE_PIECE3_TEX_PR64_15,
    HARBOR_MISSILE_PIECE3_TEX_PR64_20,
    HARBOR_MISSILE_PIECE3_TEXTURE_COUNT
};

static NJS_TEXNAME
    harbor_missile_piece3_names[HARBOR_MISSILE_PIECE3_TEXTURE_COUNT] = {
#include "stg10/structures/harbor_missile_piece3/harbor_missile_piece3_names.inc"
};

static NJS_TEXLIST harbor_missile_piece3_texlist = {
#include "stg10/structures/harbor_missile_piece3/harbor_missile_piece3_texlist.inc"
};

static Sint16 harbor_missile_piece3_polygons[361] = {
#include "stg10/structures/harbor_missile_piece3/harbor_missile_piece3_polygons.inc"
};

static Sint32 harbor_missile_piece3_vertices[379] = {
#include "stg10/structures/harbor_missile_piece3/harbor_missile_piece3_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_piece3_model = {
#include "stg10/structures/harbor_missile_piece3/harbor_missile_piece3_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_piece3_object = {
#include "stg10/structures/harbor_missile_piece3/harbor_missile_piece3_object.inc"
};

#endif
