/* Harbor missile geometry. */
#ifndef STG10_STRUCTURES_HARBOR_MISSILE_H
#define STG10_STRUCTURES_HARBOR_MISSILE_H
#include <ninja.h>
#include <sg_xpt.h>

enum harbor_missile_texture_slot {
    HARBOR_MISSILE_TEX_MIU128_MH001,
    HARBOR_MISSILE_TEX_MIU256_MH001,
    HARBOR_MISSILE_TEX_MIU256_MH002,
    HARBOR_MISSILE_TEX_MIU32_MH001,
    HARBOR_MISSILE_TEX_MIU64_IG019,
    HARBOR_MISSILE_TEX_MIU64_MH001,
    HARBOR_MISSILE_TEX_MIU64_MH002,
    HARBOR_MISSILE_TEX_MIU64_MH003,
    HARBOR_MISSILE_TEX_MIU64_MH004,
    HARBOR_MISSILE_TEX_MIU64_MH007,
    HARBOR_MISSILE_TEXTURE_COUNT
};

static NJS_TEXNAME harbor_missile_names[HARBOR_MISSILE_TEXTURE_COUNT] = {
#include "stg10/structures/harbor_missile/harbor_missile_names.inc"
};

static NJS_TEXLIST harbor_missile_texlist = {
#include "stg10/structures/harbor_missile/harbor_missile_texlist.inc"
};

static Sint16 harbor_missile_part_2_polygons[315] = {
#include "stg10/structures/harbor_missile/harbor_missile_part_2_polygons.inc"
};

static Sint32 harbor_missile_part_2_vertices[279] = {
#include "stg10/structures/harbor_missile/harbor_missile_part_2_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_part_2_model = {
#include "stg10/structures/harbor_missile/harbor_missile_part_2_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_part_2_object = {
#include "stg10/structures/harbor_missile/harbor_missile_part_2_object.inc"
};

static Sint16 harbor_missile_part_1_polygons[315] = {
#include "stg10/structures/harbor_missile/harbor_missile_part_1_polygons.inc"
};

static Sint32 harbor_missile_part_1_vertices[279] = {
#include "stg10/structures/harbor_missile/harbor_missile_part_1_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_part_1_model = {
#include "stg10/structures/harbor_missile/harbor_missile_part_1_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_part_1_object = {
#include "stg10/structures/harbor_missile/harbor_missile_part_1_object.inc"
};

static Sint16 harbor_missile_polygons[987] = {
#include "stg10/structures/harbor_missile/harbor_missile_polygons.inc"
};

static Sint32 harbor_missile_vertices[879] = {
#include "stg10/structures/harbor_missile/harbor_missile_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_model = {
#include "stg10/structures/harbor_missile/harbor_missile_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_object = {
#include "stg10/structures/harbor_missile/harbor_missile_object.inc"
};

#endif
