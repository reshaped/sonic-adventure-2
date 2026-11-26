#include <ninja.h>

#include "stg43/platform_textures/text.inc"

#include "shared/frstage.h"

static NJS_TEXNAME textures[5] = {
#include "stg43/platform_textures/textures.inc"
};
NJS_TEXLIST frstage_texlist = {
#include "stg43/platform_textures/frstage_texlist.inc"
};
