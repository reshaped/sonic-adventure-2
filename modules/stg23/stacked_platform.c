#include <ninja.h>

#include "stg21/stacked_platform.h"
#include "stg23/tier_models.h" /* IWYU pragma: keep -- names shared/stacked_platform_task.h takes */
#include "stg23/tier_textures.h"

#include "stg23/stacked_platform/text.inc"
#include "shared/stacked_platform_art.h" /* IWYU pragma: keep -- the data it defines here */

NJS_TEXLIST *stacked_platform_texlists[6] = {
#include "stg23/stacked_platform/stacked_platform_texlists.inc"
};

#define HIDDEN_BASE_STACK
#include "shared/stacked_platform_task.h" /* IWYU pragma: keep -- the code it compiles here */
