/* the PVMs that texture_load_shared loads for the carrier stages, and its
 * objects' texture lists. deck_texlist takes the stage PVM's textures by index,
 * so its names keep the PVM's order. every other list takes its textures from
 * the PVMs by name. the land's entry names no PVM: world_land_start has loaded
 * the land's list already */
#include "shared/carrier_pvms/text.inc"
#include <ninja.h>

#include "shared/carrier_textures.h"
#include "shared/carrier_texture_sources.h" /* IWYU pragma: keep -- names the bodies take */
#include "system/texture.h"

static NJS_TEXNAME deck_names[39] = {
#include "shared/carrier_pvms/deck_names.inc"
};

static NJS_TEXLIST deck_texlist = {
#include "shared/carrier_pvms/deck_texlist.inc"
};

static NJS_TEXLIST *common_group[16] = {
#include "shared/carrier_pvms/common_group.inc"
};

NJS_TEXLIST **carrier_texture_groups[46] = {
#include "shared/carrier_pvms/carrier_texture_groups.inc"
};

struct texture_pvm carrier_pvms[5] = {
#include "shared/carrier_pvms/carrier_pvms.inc"
};
