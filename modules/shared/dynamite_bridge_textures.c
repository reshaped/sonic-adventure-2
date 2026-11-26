#include <ninja.h>

#include "shared/dynamite_bridge.h"
#include "shared/dynamite_bridge_textures.h"

NJS_TEXLIST *dynamite_bridge_texlists[2] = {
#include "shared/dynamite_bridge_textures/dynamite_bridge_texlists.inc"
};
