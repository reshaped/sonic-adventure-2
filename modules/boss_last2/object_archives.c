/* the PVMs that texture_load_shared loads for the fight, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "boss_last2/object_archives/text.inc"
#include <ninja.h>

#include "boss_last2/object_archives.h"
#include "system/texture.h"
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/boss.h"
#include "shared/goal_ring.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/lens_flare.h"
#include "shared/ring.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXLIST *stage_textures[7] = {
#include "boss_last2/object_archives/stage_textures.inc"
};

NJS_TEXLIST **finalhazard_common_groups[6] = {
#include "boss_last2/object_archives/finalhazard_common_groups.inc"
};

struct texture_pvm finalhazard_common_pvms[4] = {
#include "boss_last2/object_archives/finalhazard_common_pvms.inc"
};
