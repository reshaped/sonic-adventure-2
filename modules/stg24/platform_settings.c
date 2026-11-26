#include <ninja.h>

#include "shared/engine_step.h"
#include "stg24/fixture_textures.h"

NJS_TEXLIST *engine_step_texlists[] = {
    &step_texlist,
    &step_b_texlist,
    NULL,
};

const char *engine_step_path_names[6] = {
#include "shared/core_path_titles.inc"
};
