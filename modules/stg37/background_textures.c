/* the texture list of bgtex37, the background's PVM, and the background's
 * texture lists. texture_load_shared gives each list its textures by name */
#include <ninja.h>

#include "shared/core_scenery_art.h"
#include "shared/shaft_wall_art.h"
#include "shared/water_shaft/textures.h"
#include "stg37/background.h"
#include "stg37/water_cover.h"
#include "stg37/water_materials.h"
#include "stg37/water_scenery.h"

static NJS_TEXNAME background_slots[9] = {
#include "stg37/background_textures/background_slots.inc"
};

NJS_TEXLIST core_rouge_background_textures = {
#include "stg37/background_textures/core_rouge_background_textures.inc"
};

NJS_TEXLIST *core_rouge_background_lists[22] = {
#include "stg37/background_textures/core_rouge_background_lists.inc"
};
