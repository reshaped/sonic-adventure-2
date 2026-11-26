#include <ninja.h>

#include "stg21/grave.h"
#include "stg21/grave_textures/text.inc"

static NJS_TEXNAME textures[5] = {
#include "stg21/grave_textures/textures.inc"
};
NJS_TEXLIST grave_texlist = {
#include "stg21/grave_textures/grave_texlist.inc"
};
