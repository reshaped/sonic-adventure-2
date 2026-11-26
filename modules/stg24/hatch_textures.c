#include <ninja.h>

#include "stg24/fixture_textures.h"
#include "stg24/hatch_air.h"
#include "stg24/hatch_textures.h"

NJS_TEXLIST *engine_hatch_texlists[4] = {
#include "stg24/hatch_textures/engine_hatch_texlists.inc"
};
