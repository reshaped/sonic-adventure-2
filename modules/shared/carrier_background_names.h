#ifndef SHARED_CARRIER_BACKGROUND_NAMES_H
#define SHARED_CARRIER_BACKGROUND_NAMES_H

/* The sky occupies the first two PVM slots, followed by the twenty sea frames.
 */
#include <ninja.h>

#include "shared/carrier_textures.h"

static NJS_TEXNAME background_names[22] = {
#include "shared/carrier_background_names/background_names.inc"
};
static NJS_TEXLIST background_texlist = {
#include "shared/carrier_background_names/background_texlist.inc"
};

#endif
