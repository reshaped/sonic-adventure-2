/* the background's PVM table: bgtex38, then the land's texture list, which no
 * PVM fills. texture_load_shared gives the background's lists their textures
 * from both */
#include <ninja.h>

#include "stg38/background_archive/text.inc"

#include "stg38/background.h"
#include "stg38/core_knuckles.h"
#include "system/texture.h"

NJS_TEXLIST **core_knuckles_bg_groups[2] = {
#include "stg38/background_archive/core_knuckles_bg_groups.inc"
};

struct texture_pvm core_knuckles_bg_pvms[3] = {
#include "stg38/background_archive/core_knuckles_bg_pvms.inc"
};
