/* the texture list of bgtex38, the background's PVM, and the background's
 * texture lists. texture_load_shared gives each list its textures by name. it
 * also holds the points the fence cameras look from and at */
#include <ninja.h>

#include "shared/core_scenery_art.h"
#include "shared/shaft_wall_art.h"
#include "shared/water_shaft/textures.h"
#include "stg38/background.h"
#include "stg38/water_scenery.h"

static NJS_TEXNAME background_slots[7] = {
#include "stg38/background_textures/background_slots.inc"
};

NJS_TEXLIST core_knuckles_background_textures = {
#include "stg38/background_textures/core_knuckles_background_textures.inc"
};

NJS_POINT3 first_fence_view = {
#include "stg38/background_textures/first_fence_view.inc"
};
NJS_POINT3 second_fence_view = {
#include "stg38/background_textures/second_fence_view.inc"
};
NJS_POINT3 first_fence_target = {
#include "stg38/background_textures/first_fence_target.inc"
};
NJS_POINT3 second_fence_target = {
#include "stg38/background_textures/second_fence_target.inc"
};

NJS_TEXLIST *core_knuckles_background_lists[12] = {
#include "stg38/background_textures/core_knuckles_background_lists.inc"
};
