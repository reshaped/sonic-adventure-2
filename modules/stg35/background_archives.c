#include <ninja.h>

#include "stg35/background_archives/text.inc"

#include "shared/core_energy_art.h"
#include "stg35/machinery_textures.h"
#include "stg35/machinery.h"
#include "system/texture.h"

static NJS_TEXLIST *machinery_lists[10] = {
#include "stg35/background_archives/machinery_lists.inc"
};

NJS_TEXLIST **core_eggman_bg_groups[2] = {
#include "stg35/background_archives/core_eggman_bg_groups.inc"
};

struct texture_pvm core_eggman_bg_pvms[3] = {
#include "stg35/background_archives/core_eggman_bg_pvms.inc"
};
