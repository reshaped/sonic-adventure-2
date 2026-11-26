/* the background's PVM table: bgtex37, then the land's texture list, which no
 * PVM fills. texture_load_shared gives the background's lists their textures
 * from both */
#include <ninja.h>

#include "stg37/background_archive/text.inc"

#include "stg37/background.h"
#include "stg37/core_rouge.h"
#include "system/texture.h"

NJS_TEXLIST **core_rouge_bg_groups[2] = {
#include "stg37/background_archive/core_rouge_bg_groups.inc"
};

struct texture_pvm core_rouge_bg_pvms[3] = {
#include "stg37/background_archive/core_rouge_bg_pvms.inc"
};
