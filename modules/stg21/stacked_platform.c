#include <ninja.h>

#include "stg21/stacked_platform.h"

#include "stg21/stacked_platform/text.inc"
#include "shared/stacked_platform_art.h" /* IWYU pragma: keep -- the data it defines here */

NJS_TEXLIST *stacked_platform_texlists[13] = {
#include "stg21/stacked_platform/stacked_platform_texlists.inc"
};

#include "shared/stacked_platform_task.h" /* IWYU pragma: keep -- the code it compiles here */
