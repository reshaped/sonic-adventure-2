/* Harbor missile flying geometry. */
#ifndef STG10_STRUCTURES_HARBOR_MISSILE_FLYING_H
#define STG10_STRUCTURES_HARBOR_MISSILE_FLYING_H
#include <ninja.h>
#include <sg_xpt.h>

enum harbor_missile_flying_texture_slot {
    HARBOR_MISSILE_FLYING_TEX_MIU32_MH001,
    HARBOR_MISSILE_FLYING_TEX_MIU256_MH001,
    HARBOR_MISSILE_FLYING_TEX_MIU64_MH001,
    HARBOR_MISSILE_FLYING_TEX_MIU64_MH002,
    HARBOR_MISSILE_FLYING_TEX_MIU256_MH002,
    HARBOR_MISSILE_FLYING_TEX_MIU64_MH003,
    HARBOR_MISSILE_FLYING_TEX_MIU64_MH004,
    HARBOR_MISSILE_FLYING_TEX_MIU128_MH001,
    HARBOR_MISSILE_FLYING_TEX_MIU64_IG019,
    HARBOR_MISSILE_FLYING_TEX_MIU128_MH002,
    HARBOR_MISSILE_FLYING_TEX_MIU64_MH007,
    HARBOR_MISSILE_FLYING_TEXTURE_COUNT
};

#include "draw/texture_anim.h"

static NJS_TEXNAME
    harbor_missile_flying_names[HARBOR_MISSILE_FLYING_TEXTURE_COUNT] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_names.inc"
};

static NJS_TEXLIST harbor_missile_flying_texlist = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_texlist.inc"
};

static Sint16 harbor_missile_flying_part_2_polygons[309] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_2_polygons.inc"
};

static Sint32 harbor_missile_flying_part_2_vertices[279] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_2_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_flying_part_2_model = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_2_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_flying_part_2_object = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_2_object.inc"
};

static Sint16 harbor_missile_flying_part_1_polygons[315] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_1_polygons.inc"
};

static Sint32 harbor_missile_flying_part_1_vertices[279] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_1_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_flying_part_1_model = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_1_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_flying_part_1_object = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_part_1_object.inc"
};

static Sint16 harbor_missile_flying_polygons[1657] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_polygons.inc"
};

static Sint32 harbor_missile_flying_vertices[879] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_vertices.inc"
};

static NJS_CNK_MODEL harbor_missile_flying_model = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_model.inc"
};

static NJS_CNK_OBJECT harbor_missile_flying_object[1] = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flying_object.inc"
};

/* One key per strip. Each entry starts with the polygon-list offset of
 * its strip length, then the vertex count and the original UV pairs. */
static Sint16 flame_scroll_coordinates[432] = {
#include "stg10/structures/harbor_missile_flying/flame_scroll_coordinates.inc"
};

/* The exhaust scrolls twelve texture-coordinate units down per frame. */
static struct texture_anim harbor_missile_flame_anim = {
#include "stg10/structures/harbor_missile_flying/harbor_missile_flame_anim.inc"
};

#endif
