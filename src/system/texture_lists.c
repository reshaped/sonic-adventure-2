/* Loading a texture fills in its slot's attributes and address. The HUD and
 * effect code also index these writable arrays directly. */
#include <ninja.h>

#include "effect/texlist.h"
#include "hud/texture_lists.h"
#include "system/texture.h"

#include "system/texture_lists/text.inc"

/* The loader needs the number of writable slots even without the original
 * names. */
#define TEXTURE_LIST(names) {names, sizeof(names) / sizeof((names)[0])}

static NJS_TEXNAME console_textures[25] = {
#include "system/texture_lists/console_textures.inc"
};
NJS_TEXLIST console_texlist = TEXTURE_LIST(console_textures);

NJS_TEXNAME effect_texnames[EFFECT_TEXTURE_COUNT] = {
#include "system/texture_lists/effect_texnames.inc"
};
NJS_TEXLIST effect_texlist = TEXTURE_LIST(effect_texnames);

static NJS_TEXNAME zanki_sonic_textures[1] = {
#include "system/texture_lists/zanki_sonic_textures.inc"
};
NJS_TEXLIST zanki_sonic_texlist = TEXTURE_LIST(zanki_sonic_textures);

static NJS_TEXNAME zanki_knuckle_textures[1] = {
#include "system/texture_lists/zanki_knuckle_textures.inc"
};
NJS_TEXLIST zanki_knuckle_texlist = TEXTURE_LIST(zanki_knuckle_textures);

static NJS_TEXNAME zanki_tails_textures[1] = {
#include "system/texture_lists/zanki_tails_textures.inc"
};
NJS_TEXLIST zanki_tails_texlist = TEXTURE_LIST(zanki_tails_textures);

static NJS_TEXNAME zanki_egg_textures[2] = {
#include "system/texture_lists/zanki_egg_textures.inc"
};
NJS_TEXLIST zanki_egg_texlist = TEXTURE_LIST(zanki_egg_textures);

static NJS_TEXNAME zanki_shadow_textures[1] = {
#include "system/texture_lists/zanki_shadow_textures.inc"
};
NJS_TEXLIST zanki_shadow_texlist = TEXTURE_LIST(zanki_shadow_textures);

static NJS_TEXNAME zanki_rouge_textures[1] = {
#include "system/texture_lists/zanki_rouge_textures.inc"
};
NJS_TEXLIST zanki_rouge_texlist = TEXTURE_LIST(zanki_rouge_textures);

static NJS_TEXNAME zanki_amy_textures[1] = {
#include "system/texture_lists/zanki_amy_textures.inc"
};
NJS_TEXLIST zanki_amy_texlist = TEXTURE_LIST(zanki_amy_textures);

static NJS_TEXNAME zanki_metal_textures[1] = {
#include "system/texture_lists/zanki_metal_textures.inc"
};
NJS_TEXLIST zanki_metal_texlist = TEXTURE_LIST(zanki_metal_textures);

static NJS_TEXNAME zanki_tical_textures[1] = {
#include "system/texture_lists/zanki_tical_textures.inc"
};
NJS_TEXLIST zanki_tical_texlist = TEXTURE_LIST(zanki_tical_textures);

static NJS_TEXNAME zanki_chaos0_textures[1] = {
#include "system/texture_lists/zanki_chaos0_textures.inc"
};
NJS_TEXLIST zanki_chaos0_texlist = TEXTURE_LIST(zanki_chaos0_textures);

static NJS_TEXNAME zanki_chao_textures[1] = {
#include "system/texture_lists/zanki_chao_textures.inc"
};
NJS_TEXLIST zanki_chao_texlist = TEXTURE_LIST(zanki_chao_textures);

static NJS_TEXNAME zanki_big_textures[2] = {
#include "system/texture_lists/zanki_big_textures.inc"
};
NJS_TEXLIST zanki_big_texlist = TEXTURE_LIST(zanki_big_textures);

static NJS_TEXNAME result_textures[4] = {
#include "system/texture_lists/result_textures.inc"
};
NJS_TEXLIST result_texlist = TEXTURE_LIST(result_textures);

static NJS_TEXNAME paldam_textures[2] = {
#include "system/texture_lists/paldam_textures.inc"
};
static NJS_TEXLIST paldam_texlist = TEXTURE_LIST(paldam_textures);

static NJS_TEXNAME game2p_textures[1] = {
#include "system/texture_lists/game2p_textures.inc"
};
NJS_TEXLIST game2p_texlist = TEXTURE_LIST(game2p_textures);
