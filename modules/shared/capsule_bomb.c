/* the capsule bomb, CAPSULEBOMB in a stage's object list. the bomb rides
 * in a capsule along a track. a hit bursts the capsule, and the bomb then
 * blows up */
#include <ninja.h>

#include "collision/collision.h"
#include "shared/capsule_bomb.h"
#include "shared/capsule_bomb_parts.h"

#define ACTIVE_SHAPES BOMB_SHAPES

#include "shared/capsule_bomb_art/editor.h" /* IWYU pragma: keep -- the data it defines here */

#include "shared/capsule_bomb/text.inc"

static NJS_TEXNAME capsule_bomb_names[22] = {
#include "shared/capsule_bomb/capsule_bomb_names.inc"
};

NJS_TEXLIST capsule_bomb_texlist = {
#include "shared/capsule_bomb/capsule_bomb_texlist.inc"
};

#include "shared/capsule_bomb_art/top_cap.h"

NJS_CNK_OBJECT capsule_bomb_top_cap = {
#include "shared/capsule_bomb/capsule_bomb_top_cap.inc"
};

#include "shared/capsule_bomb_art/bottom_cap.h"

NJS_CNK_OBJECT capsule_bomb_bottom_cap = {
#include "shared/capsule_bomb/capsule_bomb_bottom_cap.inc"
};

#include "shared/capsule_bomb_art/body.h"

NJS_CNK_OBJECT capsule_bomb_object[1] = {
#include "shared/capsule_bomb/capsule_bomb_object.inc"
};

static NJS_TEXNAME capsule_bomb_half_names[4] = {
#include "shared/capsule_bomb/capsule_bomb_half_names.inc"
};

NJS_TEXLIST capsule_bomb_half_texlist = {
#include "shared/capsule_bomb/capsule_bomb_half_texlist.inc"
};

#include "shared/capsule_bomb_art/lower_half.h"

NJS_CNK_OBJECT capsule_bomb_lower_half[1] = {
#include "shared/capsule_bomb/capsule_bomb_lower_half.inc"
};

#include "shared/capsule_bomb_art/upper_half.h"

NJS_CNK_OBJECT capsule_bomb_upper_half[1] = {
#include "shared/capsule_bomb/capsule_bomb_upper_half.inc"
};

static NJS_CNK_OBJECT capsule_bomb_halves = {
#include "shared/capsule_bomb/capsule_bomb_halves.inc"
};

static NJS_TEXNAME capsule_bomb_shard_a_names[1] = {
#include "shared/capsule_bomb/capsule_bomb_shard_a_names.inc"
};

NJS_TEXLIST capsule_bomb_shard_a_texlist = {
#include "shared/capsule_bomb/capsule_bomb_shard_a_texlist.inc"
};

#include "shared/capsule_bomb_art/shard_a.h"

NJS_CNK_MODEL capsule_bomb_shard_a_model[1] = {
#include "shared/capsule_bomb/capsule_bomb_shard_a_model.inc"
};

NJS_CNK_OBJECT capsule_bomb_shard_a = {
#include "shared/capsule_bomb/capsule_bomb_shard_a.inc"
};

static NJS_TEXNAME capsule_bomb_shard_b_names[1] = {
#include "shared/capsule_bomb/capsule_bomb_shard_b_names.inc"
};

NJS_TEXLIST capsule_bomb_shard_b_texlist = {
#include "shared/capsule_bomb/capsule_bomb_shard_b_texlist.inc"
};

#include "shared/capsule_bomb_art/shard_b.h"

NJS_CNK_MODEL capsule_bomb_shard_b_model[1] = {
#include "shared/capsule_bomb/capsule_bomb_shard_b_model.inc"
};

NJS_CNK_OBJECT capsule_bomb_shard_b = {
#include "shared/capsule_bomb/capsule_bomb_shard_b.inc"
};

NJS_TEXLIST *capsule_bomb_texlists[5] = {
#include "shared/capsule_bomb/capsule_bomb_texlists.inc"
};

static struct collision_shape capsule_bomb_shapes[ACTIVE_SHAPES] = {
    BOMB_SHAPE(SHAPE_CAPSULE, BOMB_PUSH, 0, 0, 5.0F, 15.0F),
    BOMB_SHAPE(SHAPE_SPHERE, BOMB_PUSH, 0, SHAPE_IN_WORLD, 5.0F, 0.0F),
    BOMB_SHAPE(SHAPE_SPHERE, BOMB_PUSH, 0, SHAPE_IN_WORLD, 15.0F, 0.0F),
};
static struct collision_shape capsule_bomb_damage_shape[DAMAGE_SHAPES] = {
    BOMB_SHAPE(SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), BLAST_HIT,
               SHAPE_HURTABLE, 2.0F, 0.0F),
};
static struct collision_shape dat_capsule_bomb_0[1] = {
    BOMB_SHAPE(SHAPE_SPHERE, BOMB_PUSH, 0, 0, 5.0F, 0.0F),
};

#include "shared/capsule_bomb_art/build_state.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/capsule_bomb_actor.h" /* IWYU pragma: keep -- the code it compiles here */
