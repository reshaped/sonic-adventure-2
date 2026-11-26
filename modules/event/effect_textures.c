#include "event/effect_textures/text.inc"
#include <ninja.h>

/* The generated registry names these object texture lists. */
#include "effect/burst.h"
#include "event/loader.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/efftex_common.h"
#include "shared/flame.h"
#include "shared/flash.h"
#include "shared/lens_flare.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket_thrust.h"
#include "shared/sparkle.h"
#include "shared/splash.h"
#include "shared/warp_hole.h"
#include "shared/white_breath.h"
#include "system/texture.h"

static NJS_TEXLIST *effect_textures[13] = {
#include "event/effect_textures/effect_textures.inc"
};

NJS_TEXLIST **event_texture_groups[2] = {
#include "event/effect_textures/event_texture_groups.inc"
};
struct texture_pvm event_pvms[2] = {
#include "event/effect_textures/event_pvms.inc"
};
