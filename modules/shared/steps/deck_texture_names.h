/* Material slots in the order used by the deck step. */
#ifndef SHARED_STEPS_DECK_TEXTURE_NAMES_H
#define SHARED_STEPS_DECK_TEXTURE_NAMES_H
#include <ninja.h>
#include "shared/step_textures.h"

static NJS_TEXNAME textures[9] = {
#include "shared/steps/deck_texture_names/textures.inc"
};

#endif
