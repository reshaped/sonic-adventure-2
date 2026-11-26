/* the capsule bomb's texture list and models in Cannon's Core. its list holds
 * one poison texture, where the other stages' list holds thirteen */
#include <ninja.h>

#include "shared/capsule_bomb_parts.h"
#include "shared/relay_bomb_textures/text.inc"

static NJS_TEXNAME capsule_bomb_names[10] = {
#include "shared/relay_bomb_textures/capsule_bomb_names.inc"
};

NJS_TEXLIST capsule_bomb_texlist = {
#include "shared/relay_bomb_textures/capsule_bomb_texlist.inc"
};

#include "shared/capsule_bomb_art/top_cap.h"

NJS_CNK_OBJECT capsule_bomb_top_cap = {
#include "shared/relay_bomb_textures/capsule_bomb_top_cap.inc"
};

#include "shared/capsule_bomb_art/bottom_cap.h"

NJS_CNK_OBJECT capsule_bomb_bottom_cap = {
#include "shared/relay_bomb_textures/capsule_bomb_bottom_cap.inc"
};

#include "shared/capsule_bomb_art/body.h"

NJS_CNK_OBJECT capsule_bomb_object[1] = {
#include "shared/relay_bomb_textures/capsule_bomb_object.inc"
};

static NJS_TEXNAME capsule_bomb_half_names[4] = {
#include "shared/relay_bomb_textures/capsule_bomb_half_names.inc"
};

NJS_TEXLIST capsule_bomb_half_texlist = {
#include "shared/relay_bomb_textures/capsule_bomb_half_texlist.inc"
};

#include "shared/capsule_bomb_art/lower_half.h"

NJS_CNK_OBJECT capsule_bomb_lower_half[1] = {
#include "shared/relay_bomb_textures/capsule_bomb_lower_half.inc"
};

#include "shared/capsule_bomb_art/upper_half.h"

NJS_CNK_OBJECT capsule_bomb_upper_half[1] = {
#include "shared/relay_bomb_textures/capsule_bomb_upper_half.inc"
};

static NJS_CNK_OBJECT capsule_bomb_halves = {
#include "shared/relay_bomb_textures/capsule_bomb_halves.inc"
};

static NJS_TEXNAME capsule_bomb_shard_a_names[1] = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_a_names.inc"
};

NJS_TEXLIST capsule_bomb_shard_a_texlist = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_a_texlist.inc"
};

#include "shared/capsule_bomb_art/shard_a.h"

NJS_CNK_MODEL capsule_bomb_shard_a_model[1] = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_a_model.inc"
};

NJS_CNK_OBJECT capsule_bomb_shard_a = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_a.inc"
};

static NJS_TEXNAME capsule_bomb_shard_b_names[1] = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_b_names.inc"
};

NJS_TEXLIST capsule_bomb_shard_b_texlist = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_b_texlist.inc"
};

#include "shared/capsule_bomb_art/shard_b.h"

NJS_CNK_MODEL capsule_bomb_shard_b_model[1] = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_b_model.inc"
};

NJS_CNK_OBJECT capsule_bomb_shard_b = {
#include "shared/relay_bomb_textures/capsule_bomb_shard_b.inc"
};
