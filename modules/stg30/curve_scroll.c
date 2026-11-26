#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "stg30/curved_rail.h"
#include "stg30/curved_rail_art.h"

/* one key of uvs, as texture_anim.h lays them out: an entry for each strip */
/* clang-format off */
static Sint16 uvs[148] = {
#include "stg30/curve_scroll/uvs.inc"
};
struct texture_anim curved_rail_anim = {TEXTURE_ANIM_SCROLL, 1, 1, NULL, uvs, NULL, NULL, NULL, 0, 0};
NJS_TEXLIST *curved_rail_texlists[] = {&curved_rail_texlist, &curved_rail_b_texlist, NULL};
/* a collision shape that no code reads */
static struct collision_shape box = {
    0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 9.0F, 0.0F}, 10.0F, 9.0F, 46.0F, 0, {0, 0, 0},
};
/* clang-format on */
