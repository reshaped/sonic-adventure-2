#include <ninja.h>

#include "stg26/guard_eye.h"

#include "stg26/eye_beam_textures/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg26/eye_beam_textures/textures.inc"
};
NJS_TEXLIST guard_eye_beam_texlist = {textures,
                                      sizeof(textures) / sizeof(textures[0])};
