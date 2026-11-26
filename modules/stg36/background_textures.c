#include <ninja.h>

#include "shared/core_conduit_textures.h"
#include "shared/core_lamp_art.h"
#include "stg36/machinery_textures.h"

static NJS_TEXNAME names[7] = {
#include "stg36/background_textures/names.inc"
};

NJS_TEXLIST core_tails_background_textures = {
#include "stg36/background_textures/core_tails_background_textures.inc"
};
