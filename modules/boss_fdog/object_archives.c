/* the PVMs that texture_load_shared loads for the fight, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "boss_fdog/object_archives/text.inc"
#include <ninja.h>

#include "boss_fdog/object_archives.h"
#include "shared/common_pvm_textures.h"
#include "shared/gun_mech/stage_textures.h"
#include "shared/mech_boss_beam.h"
#include "system/texture.h"

NJS_TEXLIST **fdog_texture_groups[20] = {
#include "boss_fdog/object_archives/fdog_texture_groups.inc"
};

struct texture_pvm fdog_texture_pvms[4] = {
#include "boss_fdog/object_archives/fdog_texture_pvms.inc"
};
