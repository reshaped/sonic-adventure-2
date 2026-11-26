/* the PVMs that texture_load_shared loads for the fight, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "boss_hotshot/object_archives/text.inc"
#include <ninja.h>

#include "boss_hotshot/object_archives.h"
#include "shared/common_pvm_textures.h"
#include "shared/gun_mech/stage_textures.h"
#include "shared/mech_boss_beam.h"
#include "system/texture.h"

NJS_TEXLIST **hotshot_texture_groups[20] = {
#include "boss_hotshot/object_archives/hotshot_texture_groups.inc"
};

struct texture_pvm hotshot_texture_pvms[4] = {
#include "boss_hotshot/object_archives/hotshot_texture_pvms.inc"
};
