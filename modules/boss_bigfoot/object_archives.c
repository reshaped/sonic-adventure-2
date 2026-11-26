/* the PVMs that texture_load_shared loads for the fight, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "boss_bigfoot/object_archives/text.inc"
#include <ninja.h>

#include "boss_bigfoot/object_archives.h"
#include "shared/common_pvm_textures.h"
#include "shared/mech_boss_beam.h"
#include "system/texture.h"
#include "boss_bigfoot/prop.h"
#include "boss_bigfoot/signboard.h"
#include "shared/glow_light.h"
#include "shared/parked_car.h"
#include "shared/plain_tree.h"
#include "shared/stage_texture_groups.h"

static NJS_TEXNAME street_textures[73] = {
#include "boss_bigfoot/object_archives/street_textures.inc"
};

static NJS_TEXLIST street_texlist = {
#include "boss_bigfoot/object_archives/street_texlist.inc"
};

#include "shared/gun_mech/stage_textures.h"

NJS_TEXLIST **bigfoot_texture_groups[26] = {
#include "boss_bigfoot/object_archives/bigfoot_texture_groups.inc"
};

struct texture_pvm bigfoot_texture_pvms[5] = {
#include "boss_bigfoot/object_archives/bigfoot_texture_pvms.inc"
};
