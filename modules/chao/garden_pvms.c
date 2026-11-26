/* the PVMs that texture_load_shared loads for a garden area, and its
 * effects' and Omochao's texture lists. each list takes its textures from
 * the PVMs by name */
#include "chao/garden_pvms/text.inc"
#include <ninja.h>
#include "chao/garden_scene.h"
#include "effect/texlist.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/flame.h"
#include "shared/garden_setup.h"
#include "shared/sparkle.h"
#include "shared/splash.h"
#include "shared/stage_texture_groups.h"
#include "shared/water_ripple.h"
#include "system/texture.h"

/* efftex_common's names. the Chao race loads efftex_common into
 * garden_effect_texlist too */
static NJS_TEXNAME effect_names[121] = {
#include "chao/garden_pvms/effect_names.inc"
};
NJS_TEXLIST garden_effect_texlist = {
#include "chao/garden_pvms/garden_effect_texlist.inc"
};
#include "shared/objtex_common.h"

static NJS_TEXLIST *garden_effects[8] = {
#include "chao/garden_pvms/garden_effects.inc"
};

NJS_TEXLIST **garden_texture_groups[3] = {
#include "chao/garden_pvms/garden_texture_groups.inc"
};
struct texture_pvm garden_pvms[3] = {
#include "chao/garden_pvms/garden_pvms.inc"
};
