#include <ninja.h>

#include "shared/cannon_parts.h"
#include "stg24/fixture_textures.h"
#include "stg24/reactor_art.h"

NJS_TEXLIST *engine_reactor_texlists[18] = {
#include "stg24/reactor_settings/engine_reactor_texlists.inc"
};

NJS_TEXLIST *reactor_fragment_textures[8] = {
#include "stg24/reactor_settings/reactor_fragment_textures.inc"
};

NJS_CNK_OBJECT *reactor_fragments[8] = {
#include "stg24/reactor_settings/reactor_fragments.inc"
};

NJS_POINT3 reactor_panel_positions[9] = {
#include "stg24/reactor_settings/reactor_panel_positions.inc"
};
