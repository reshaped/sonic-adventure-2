/* These stages store the 17-slot star bank with a private space64_20
 * filename. Final Chase borrows that name from its background archive. */
#include <ninja.h>

#include "shared/space_sky_textures.h"

#include "shared/sky_stars_lower/text.inc"

static NJS_TEXNAME textures[17] = {
#include "shared/sky_stars_lower/textures.inc"
};
NJS_TEXLIST background_1_texlist = {textures,
                                    sizeof(textures) / sizeof(textures[0])};
