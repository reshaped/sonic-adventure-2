/* the monkey's textures, model and motions. enum monkey_texture gives the
 * texture list's slots. each _bomb_motion has one node, whose position places
 * the held bomb in the model's space */
#ifndef SHARED_MONKEY_ART_MODELS_H
#define SHARED_MONKEY_ART_MODELS_H

#include <ninja.h>

enum monkey_texture {
    MONKEY_TEX_SARU_ALL,
    MONKEY_TEX_SARU_EYE,
    MONKEY_TEX_SARU_BODY,
    MONKEY_TEXTURE_COUNT
};

static NJS_TEXNAME monkey_texture_names[MONKEY_TEXTURE_COUNT] = {
#include "shared/monkey_art/models/monkey_texture_names.inc"
};
static NJS_TEXLIST monkey_texlist = {
#include "shared/monkey_art/models/monkey_texlist.inc"
};

/* clang-format off */
#include "shared/monkey_art/body.h" /* IWYU pragma: export */
#include "shared/monkey_art/hops.h" /* IWYU pragma: export */
#include "shared/monkey_art/hold.h" /* IWYU pragma: export */
#include "shared/monkey_art/fumble.h" /* IWYU pragma: export */
#include "shared/monkey_art/heavy_throw.h" /* IWYU pragma: export */
#include "shared/monkey_art/throw.h" /* IWYU pragma: export */
/* clang-format on */

#endif
