/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg44/object_archives/text.inc"
#include <ninja.h>

#include "stg44/object_archives.h"
#include "system/texture.h"

/* The generated groups and PVMs reference these texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/emerald.h"
#include "shared/empty_object_texlists.h"
#include "shared/frstage.h"
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
#include "shared/space_pickup.h"
#include "shared/spin_crates.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg44/dock_marker.h"
#include "stg44/fence.h"
#include "stg44/fireball.h"
#include "stg44/foothold.h"
#include "stg44/landing.h"
#include "stg44/lights.h"
#include "stg44/mad_space.h"
#include "stg44/ore.h"
#include "stg44/planter.h"
#include "stg44/props.h"
#include "stg44/shutter.h"
#include "stg44/sky.h"
#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[46] = {
#include "stg44/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg44/object_archives/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg44/object_archives/stage_textures.inc"
};

NJS_TEXLIST **mad_space_texture_groups[50] = {
#include "stg44/object_archives/mad_space_texture_groups.inc"
};

struct texture_pvm mad_space_pvms[6] = {
#include "stg44/object_archives/mad_space_pvms.inc"
};
