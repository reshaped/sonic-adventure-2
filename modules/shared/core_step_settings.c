#include <ninja.h>

#include "shared/core_step.h"

NJS_TEXLIST *core_step_texlists[] = {&core_step_texlist, &core_step_b_texlist,
                                     NULL};

const char *core_step_path_names[6] = {
#include "shared/core_path_titles.inc"
};
