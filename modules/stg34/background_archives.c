/* the background's PVM table: bgtex34, then the land's texture list, which no
 * PVM fills. texture_load_shared gives the background's lists their textures
 * from both */
#include <ninja.h>

#include "stg34/background_archives/text.inc"

#include "stg34/core_sonic.h"
#include "stg34/scenery_art.h"
#include "system/texture.h"

NJS_TEXLIST **core_sonic_bg_groups[2] = {
#include "stg34/background_archives/core_sonic_bg_groups.inc"
};

struct texture_pvm core_sonic_bg_pvms[3] = {
#include "stg34/background_archives/core_sonic_bg_pvms.inc"
};
