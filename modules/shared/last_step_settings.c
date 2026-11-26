#include <ninja.h>
#include <sg_xpt.h>

#include "shared/last_step.h"

NJS_TEXLIST *last_step_texlists[] = {&last_step_texlist, NULL};

Sint32 last_step_panel_volume = 125;

const char *last_step_path_names[6] = {
#include "shared/core_path_titles.inc"
};
