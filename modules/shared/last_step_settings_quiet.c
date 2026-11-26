#include <ninja.h>

#include "shared/last_step.h"

NJS_TEXLIST *last_step_texlists[] = {&last_step_texlist, NULL};

const char *last_step_path_names[6] = {
#include "shared/core_path_titles.inc"
};
