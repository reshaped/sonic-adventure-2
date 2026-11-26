#ifndef SHARED_CARRIER_BACKGROUND_GROUPS_H
#define SHARED_CARRIER_BACKGROUND_GROUPS_H

#include <ninja.h>

#include "shared/carrier_textures.h"
#include "shared/sea.h"
#include "system/texture.h"

static NJS_TEXLIST *background_group[3] = {
#include "shared/carrier_background_groups/background_group.inc"
};
static NJS_TEXLIST **background_groups[2] = {
#include "shared/carrier_background_groups/background_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "shared/carrier_background_groups/background_pvms.inc"
};

#endif
