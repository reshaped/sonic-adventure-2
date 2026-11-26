/* the texture lists of the mech bosses' models and effects. grobo_texlist and
 * grobo_eff_texlist take their PVMs' textures by index, so their names keep
 * each PVM's order. no code reads single_beam_texlist */
#include <ninja.h>

#include "shared/mech_boss.h"
#include "shared/mech_boss_beam.h"

#include "shared/gun_mech/model_textures/text.inc"

static NJS_TEXNAME grobo_textures[26] = {
#include "shared/gun_mech/model_textures/grobo_textures.inc"
};

NJS_TEXLIST grobo_texlist = {
#include "shared/gun_mech/model_textures/grobo_texlist.inc"
};

static NJS_TEXNAME grobo_eff_textures[3] = {
#include "shared/gun_mech/model_textures/grobo_eff_textures.inc"
};

NJS_TEXLIST grobo_eff_texlist = {
#include "shared/gun_mech/model_textures/grobo_eff_texlist.inc"
};

static NJS_TEXNAME jet1_textures[1] = {
#include "shared/gun_mech/model_textures/jet1_textures.inc"
};

NJS_TEXLIST jet1_texlist = {
#include "shared/gun_mech/model_textures/jet1_texlist.inc"
};

static NJS_TEXNAME single_beam_textures[1] = {
#include "shared/gun_mech/model_textures/single_beam_textures.inc"
};

static NJS_TEXLIST single_beam_texlist = {
#include "shared/gun_mech/model_textures/single_beam_texlist.inc"
};
