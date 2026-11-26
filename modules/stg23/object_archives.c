/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg23/object_archives/text.inc"
#include <ninja.h>

/* The generated PVM and group directories reference these texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/emerald.h"
#include "shared/goal_ring.h"
#include "shared/hint_box.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/iron_box_e.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/knudai.h"
#include "shared/knudai_door.h"
#include "shared/knudai_foothold.h"
#include "shared/knudai_warp_hole.h"
#include "shared/lens_flare.h"
#include "shared/ocean_fence.h"
#include "shared/ocean_pot.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/skull.h"
#include "shared/solid_box.h"
#include "shared/solid_box_e.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "shared/wood_box_e.h"
#include "stg21/corner.h"
#include "stg21/flag.h"
#include "stg21/stacked_platform.h"
#include "stg23/background.h"
#include "stg23/breakables.h"
#include "stg23/hidden_base.h"
#include "stg23/object_archives.h"
#include "stg23/oil_derrick.h"
#include "stg23/platforms.h"
#include "stg23/scenery.h"
#include "stg23/staff.h"
#include "stg23/tower.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[34] = {
#include "stg23/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg23/object_archives/stage_archive.inc"
};

static NJS_TEXNAME quicksand_names[32] = {
#include "stg23/object_archives/quicksand_names.inc"
};
NJS_TEXLIST base_quicksand_texlist = {
#include "stg23/object_archives/base_quicksand_texlist.inc"
};

static NJS_TEXNAME eggman_container_names[4] = {
#include "stg23/object_archives/eggman_container_names.inc"
};
static NJS_TEXLIST eggman_container_texlist = {
#include "stg23/object_archives/eggman_container_texlist.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg23/object_archives/stage_textures.inc"
};

NJS_TEXLIST **hidden_base_texture_groups[51] = {
#include "stg23/object_archives/hidden_base_texture_groups.inc"
};

struct texture_pvm hidden_base_pvms[8] = {
#include "stg23/object_archives/hidden_base_pvms.inc"
};
