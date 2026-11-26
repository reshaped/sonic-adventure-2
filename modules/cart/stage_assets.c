#include "cart/stage_assets/text.inc"
#include <ninja.h>

#include "cart/stage_assets.h"
#include "system/texture.h"

/* The generated groups and PVMs reference these texture lists. */
#include "effect/texlist.h"
#include "shared/common_pvm_textures.h"
#include "shared/dust.h"
#include "shared/item_balloon.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket_thrust.h"
#include "shared/splash.h"

static NJS_TEXLIST *object_effects[8] = {
#include "cart/stage_assets/object_effects.inc"
};

NJS_TEXLIST **cart_texture_groups[2] = {
#include "cart/stage_assets/cart_texture_groups.inc"
};

struct texture_pvm cart_pvms[3] = {
#include "cart/stage_assets/cart_pvms.inc"
};
