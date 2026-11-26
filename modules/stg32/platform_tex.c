#include <ninja.h>

#include "stg32/platform.h"
#include "stg32/platform_names.h"

const struct platform_filenames meteor_platform_filenames = {
#include "stg32/platform_tex/meteor_platform_filenames.inc"
};

NJS_TEXLIST meteor_platform_texlist = {
#include "stg32/platform_tex/meteor_platform_texlist.inc"
};
