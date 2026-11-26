/* the texture lists of Lost Colony's STEP, and the names of its six paths,
 * which no code reads */
#include <ninja.h>

#include "shared/engine_step.h"
#include "stg27/object_archives.h"

NJS_TEXLIST *colony_step_texlists[2] = {
#include "stg27/step_settings/colony_step_texlists.inc"
};

static const char *path_names[6] = {
#include "shared/core_path_titles.inc"
};
