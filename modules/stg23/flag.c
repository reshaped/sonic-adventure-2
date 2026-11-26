/* Hidden Base's flag. one texture list serves every frame of its cloth. the
 * frames' models stand in the image in another order than they play.
 * ocean_flag_models lists them in play order */
#include <ninja.h>

#define HIDDEN_BASE_FLAG
#include "stg21/flag.h"

#include "shared/ocean_flag_frames.h"

NJS_CNK_MODEL *ocean_flag_models[FLAG_FRAMES] = {
#include "stg23/flag/ocean_flag_models.inc"
};
NJS_TEXLIST *ocean_flag_texlists[] = {
    &hidden_base_flag_cloth,
    &flag_support_texlist,
    NULL,
};

#include "stg21/flag.c" /* NOLINT(bugprone-suspicious-include) */
