#include <ninja.h>

#include "shared/core_conduit_textures.h"
#include "shared/core_lamp_art.h"
#include "stg35/machinery_textures.h"
#include "stg35/ring_lamps.h"

static NJS_TEXNAME names[7] = {
#include "stg35/background_textures/names.inc"
};

NJS_TEXLIST core_eggman_background_textures = {
#include "stg35/background_textures/core_eggman_background_textures.inc"
};
