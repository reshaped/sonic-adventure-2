/* Harbor missile piece2 geometry. */
#ifndef STG10_STRUCTURES_HARBOR_MISSILE_PIECE2_H
#define STG10_STRUCTURES_HARBOR_MISSILE_PIECE2_H
#include <ninja.h>
#include <sg_xpt.h>

enum harbor_missile_piece2_texture_slot {
    HARBOR_MISSILE_PIECE2_TEX_MIU128_MH001,
    HARBOR_MISSILE_PIECE2_TEX_MIU64_MH003,
    HARBOR_MISSILE_PIECE2_TEX_MIU64_MH006,
    HARBOR_MISSILE_PIECE2_TEXTURE_COUNT
};

static NJS_TEXNAME
    harbor_missile_piece2_names[HARBOR_MISSILE_PIECE2_TEXTURE_COUNT] = {
#include "stg10/structures/harbor_missile_piece2/harbor_missile_piece2_names.inc"
};

static NJS_TEXLIST harbor_missile_piece2_texlist = {
#include "stg10/structures/harbor_missile_piece2/harbor_missile_piece2_texlist.inc"
};

static Sint16 harbor_missile_piece2_polygons[397] = {
#include "stg10/structures/harbor_missile_piece2/harbor_missile_piece2_polygons.inc"
};

static Sint32 harbor_missile_piece2_vertices[419] = {
#include "stg10/structures/harbor_missile_piece2/harbor_missile_piece2_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_piece2_model = {
#include "stg10/structures/harbor_missile_piece2/harbor_missile_piece2_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_piece2_object = {
#include "stg10/structures/harbor_missile_piece2/harbor_missile_piece2_object.inc"
};

#endif
