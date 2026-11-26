/* Material slots in the order used by the wide deck step. */
#ifndef SHARED_STEPS_WIDE_DECK_TEXTURE_NAMES_H
#define SHARED_STEPS_WIDE_DECK_TEXTURE_NAMES_H
#include <ninja.h>
#include "shared/step_textures.h"

static const char miu128_cg007[16] = {
#include "shared/steps/wide_deck_texture_names/miu128_cg007.inc"
};

static NJS_TEXNAME textures[10] = {
#include "shared/steps/wide_deck_texture_names/textures.inc"
};

#endif
