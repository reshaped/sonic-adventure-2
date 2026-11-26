#include <ninja.h>

#include "stg31/spikes.h"

#include "stg31/spike_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg31/spike_textures/textures.inc"
};

NJS_TEXLIST green_hill_spikes_texlist = {textures, sizeof(textures) /
                                                       sizeof(textures[0])};
