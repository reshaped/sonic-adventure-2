/* Harbor missile piece4 geometry. */
#ifndef STG10_STRUCTURES_HARBOR_MISSILE_PIECE4_H
#define STG10_STRUCTURES_HARBOR_MISSILE_PIECE4_H
#include <ninja.h>
#include <sg_xpt.h>

enum harbor_missile_piece4_texture_slot {
    HARBOR_MISSILE_PIECE4_TEX_PR64_12,
    HARBOR_MISSILE_PIECE4_TEX_PR64_19,
    HARBOR_MISSILE_PIECE4_TEXTURE_COUNT
};

static NJS_TEXNAME
    harbor_missile_piece4_names[HARBOR_MISSILE_PIECE4_TEXTURE_COUNT] = {
#include "stg10/structures/harbor_missile_piece4/harbor_missile_piece4_names.inc"
};

static NJS_TEXLIST harbor_missile_piece4_texlist = {
#include "stg10/structures/harbor_missile_piece4/harbor_missile_piece4_texlist.inc"
};

static Sint16 harbor_missile_piece4_polygons[217] = {
#include "stg10/structures/harbor_missile_piece4/harbor_missile_piece4_polygons.inc"
};

static Sint32 harbor_missile_piece4_vertices[235] = {
#include "stg10/structures/harbor_missile_piece4/harbor_missile_piece4_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_piece4_model = {
#include "stg10/structures/harbor_missile_piece4/harbor_missile_piece4_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_piece4_object = {
#include "stg10/structures/harbor_missile_piece4/harbor_missile_piece4_object.inc"
};

#endif
