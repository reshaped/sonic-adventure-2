#include <ninja.h>

#include "stg36/background_archives/text.inc"

#include "shared/core_energy_art.h"
#include "stg36/machinery_textures.h"
#include "stg36/entrance_lamps.h"
#include "stg36/route.h"
#include "system/texture.h"

static NJS_TEXLIST *machinery_lists[8] = {
#include "stg36/background_archives/machinery_lists.inc"
};

NJS_TEXLIST **core_tails_bg_groups[2] = {
#include "stg36/background_archives/core_tails_bg_groups.inc"
};

struct texture_pvm core_tails_bg_pvms[2] = {
#include "stg36/background_archives/core_tails_bg_pvms.inc"
};
