/* the PVMs that texture_load_shared loads for Aquatic Mine, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg07/object_archives/text.inc"
#include <ninja.h>

/* The generated registry names these object texture lists. */
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
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/skull.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/water_ripple.h"
#include "shared/water_specks.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg07/aquatic_mine.h"
#include "stg07/cmdoor.h"
#include "stg07/drum_can.h"
#include "stg07/fire_statue.h"
#include "stg07/floating_foothold.h"
#include "stg07/info.h"
#include "stg07/kiln.h"
#include "stg07/lamp.h"
#include "stg07/mimocl.h"
#include "stg07/mine_tool.h"
#include "stg07/mine_truck.h"
#include "stg07/object_archives.h"
#include "stg07/pickup.h"
#include "stg07/scenery.h"
#include "stg07/solid_models.h"
#include "stg07/water.h"
#include "stg07/waterweed.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[47] = {
#include "stg07/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg07/object_archives/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[17] = {
#include "stg07/object_archives/stage_textures.inc"
};

NJS_TEXLIST **aquatic_mine_texture_groups[44] = {
#include "stg07/object_archives/aquatic_mine_texture_groups.inc"
};

struct texture_pvm aquatic_mine_pvms[6] = {
#include "stg07/object_archives/aquatic_mine_pvms.inc"
};
