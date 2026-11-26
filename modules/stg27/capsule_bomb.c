/* Lost Colony's capsule bomb. it has one collision shape, and its blast adds
 * to colony_flash rather than vibrating a pad. it drops its weak point as it
 * stops past the far end */
#include <ninja.h>

#include "collision/collision.h"
#include "shared/capsule_bomb.h"
#include "shared/capsule_bomb_parts.h"
#include "stg27/light.h" /* IWYU pragma: keep -- names shared/capsule_bomb_actor.h takes */

#define COLONY_BOMB
#define ACTIVE_SHAPES 1

#include "shared/capsule_bomb_art/editor.h" /* IWYU pragma: keep -- the data it defines here */

#include "stg27/capsule_bomb/text.inc"

static NJS_TEXNAME capsule_bomb_names[22] = {
#include "stg27/capsule_bomb/capsule_bomb_names.inc"
};

NJS_TEXLIST capsule_bomb_texlist = {
#include "stg27/capsule_bomb/capsule_bomb_texlist.inc"
};

#include "shared/capsule_bomb_art/top_cap.h"

NJS_CNK_OBJECT capsule_bomb_top_cap = {
#include "stg27/capsule_bomb/capsule_bomb_top_cap.inc"
};

#include "shared/capsule_bomb_art/bottom_cap.h"

NJS_CNK_OBJECT capsule_bomb_bottom_cap = {
#include "stg27/capsule_bomb/capsule_bomb_bottom_cap.inc"
};

#include "shared/capsule_bomb_art/body.h"

NJS_CNK_OBJECT capsule_bomb_object[1] = {
#include "stg27/capsule_bomb/capsule_bomb_object.inc"
};

static NJS_TEXNAME capsule_bomb_half_names[4] = {
#include "stg27/capsule_bomb/capsule_bomb_half_names.inc"
};

NJS_TEXLIST capsule_bomb_half_texlist = {
#include "stg27/capsule_bomb/capsule_bomb_half_texlist.inc"
};

#include "shared/capsule_bomb_art/lower_half.h"

NJS_CNK_OBJECT capsule_bomb_lower_half[1] = {
#include "stg27/capsule_bomb/capsule_bomb_lower_half.inc"
};

#include "shared/capsule_bomb_art/upper_half.h"

NJS_CNK_OBJECT capsule_bomb_upper_half[1] = {
#include "stg27/capsule_bomb/capsule_bomb_upper_half.inc"
};

static NJS_CNK_OBJECT capsule_bomb_halves = {
#include "stg27/capsule_bomb/capsule_bomb_halves.inc"
};

static NJS_TEXNAME capsule_bomb_shard_a_names[1] = {
#include "stg27/capsule_bomb/capsule_bomb_shard_a_names.inc"
};

NJS_TEXLIST capsule_bomb_shard_a_texlist = {
#include "stg27/capsule_bomb/capsule_bomb_shard_a_texlist.inc"
};

#include "shared/capsule_bomb_art/shard_a.h"

NJS_CNK_MODEL capsule_bomb_shard_a_model[1] = {
#include "stg27/capsule_bomb/capsule_bomb_shard_a_model.inc"
};

NJS_CNK_OBJECT capsule_bomb_shard_a = {
#include "stg27/capsule_bomb/capsule_bomb_shard_a.inc"
};

static NJS_TEXNAME capsule_bomb_shard_b_names[1] = {
#include "stg27/capsule_bomb/capsule_bomb_shard_b_names.inc"
};

NJS_TEXLIST capsule_bomb_shard_b_texlist = {
#include "stg27/capsule_bomb/capsule_bomb_shard_b_texlist.inc"
};

#include "shared/capsule_bomb_art/shard_b.h"

NJS_CNK_MODEL capsule_bomb_shard_b_model[1] = {
#include "stg27/capsule_bomb/capsule_bomb_shard_b_model.inc"
};

NJS_CNK_OBJECT capsule_bomb_shard_b = {
#include "stg27/capsule_bomb/capsule_bomb_shard_b.inc"
};

NJS_TEXLIST *capsule_bomb_texlists[5] = {
#include "stg27/capsule_bomb/capsule_bomb_texlists.inc"
};

static struct collision_shape capsule_bomb_shapes[ACTIVE_SHAPES] = {
    BOMB_SHAPE(SHAPE_CAPSULE, BOMB_PUSH, 0, 0, 5.0F, 15.0F),
};
static struct collision_shape capsule_bomb_damage_shape[DAMAGE_SHAPES] = {
    BOMB_SHAPE(SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), BLAST_HIT,
               SHAPE_HURTABLE, 2.0F, 0.0F),
};

#include "shared/capsule_bomb_art/build_state.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/capsule_bomb_actor.h" /* IWYU pragma: keep -- the code it compiles here */
