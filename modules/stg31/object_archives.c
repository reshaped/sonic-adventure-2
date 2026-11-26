/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg31/object_archives/text.inc"
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
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/knudai.h"
#include "shared/knudai_door.h"
#include "shared/knudai_foothold.h"
#include "shared/knudai_warp_hole.h"
#include "shared/lens_flare.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/skull.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg31/green_hill.h"
#include "stg31/object_archives.h"
#include "stg31/platforms.h"
#include "stg31/scenery.h"
#include "stg31/spikes.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[4] = {
#include "stg31/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg31/object_archives/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg31/object_archives/stage_textures.inc"
};

NJS_TEXLIST **green_hill_texture_groups[35] = {
#include "stg31/object_archives/green_hill_texture_groups.inc"
};

struct texture_pvm green_hill_pvms[5] = {
#include "stg31/object_archives/green_hill_pvms.inc"
};
