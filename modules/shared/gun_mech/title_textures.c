/* the texture lists that mech_boss_textures_load fills by index: each boss's
 * title, the titles' letters, and grobo_eff2_texlist */
#include <ninja.h>

#include "shared/gun_mech/title_textures/text.inc"

#include "shared/mech_boss.h"

static NJS_TEXNAME bigfoot_title_textures[9] = {
#include "shared/gun_mech/title_textures/bigfoot_title_textures.inc"
};

NJS_TEXLIST bigfoot_title_texlist = {
#include "shared/gun_mech/title_textures/bigfoot_title_texlist.inc"
};

static NJS_TEXNAME hotshot_title_textures[9] = {
#include "shared/gun_mech/title_textures/hotshot_title_textures.inc"
};

NJS_TEXLIST hotshot_title_texlist = {
#include "shared/gun_mech/title_textures/hotshot_title_texlist.inc"
};

static NJS_TEXNAME fdog_title_textures[9] = {
#include "shared/gun_mech/title_textures/fdog_title_textures.inc"
};

NJS_TEXLIST fdog_title_texlist = {
#include "shared/gun_mech/title_textures/fdog_title_texlist.inc"
};

static NJS_TEXNAME grobo_title_moji_textures[19] = {
#include "shared/gun_mech/title_textures/grobo_title_moji_textures.inc"
};

NJS_TEXLIST grobo_title_moji_texlist = {
#include "shared/gun_mech/title_textures/grobo_title_moji_texlist.inc"
};

static NJS_TEXNAME grobo_eff2_textures[6] = {
#include "shared/gun_mech/title_textures/grobo_eff2_textures.inc"
};

NJS_TEXLIST grobo_eff2_texlist = {
#include "shared/gun_mech/title_textures/grobo_eff2_texlist.inc"
};
