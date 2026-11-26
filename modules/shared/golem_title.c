/* the title card's twelve textures: seven bars, and the letters of GOLEM */
#include <ninja.h>

#include "shared/golem_title.h"

#include "shared/golem_title/text.inc"

static NJS_TEXNAME title_textures[12] = {
#include "shared/golem_title/title_textures.inc"
};

NJS_TEXLIST golem_title_texlist = {
#include "shared/golem_title/golem_title_texlist.inc"
};
