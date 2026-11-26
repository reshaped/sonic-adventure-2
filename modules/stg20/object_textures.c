#define CARRIER_DUEL
#include "stg20/object_textures/text.inc"
#include <ninja.h>

#include "shared/carrier_textures.h"
#include "shared/carrier_texture_sources.h" /* IWYU pragma: keep -- names the bodies take */
#include "system/texture.h"

static NJS_TEXNAME deck_names[44] = {
#include "stg20/object_textures/deck_names.inc"
};

static NJS_TEXLIST deck_texlist = {
#include "stg20/object_textures/deck_texlist.inc"
};

static NJS_TEXLIST *common_group[14] = {
#include "stg20/object_textures/common_group.inc"
};

NJS_TEXLIST **carrier_texture_groups[34] = {
#include "stg20/object_textures/carrier_texture_groups.inc"
};

struct texture_pvm carrier_pvms[6] = {
#include "stg20/object_textures/carrier_pvms.inc"
};
