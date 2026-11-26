/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg32/object_archives/text.inc"
#include <ninja.h>

#include "stg32/object_archives.h"
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
#include "shared/light_effect.h"
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
#include "stg32/bar.h"
#include "stg32/fence.h"
#include "stg32/machinery.h"
#include "stg32/meteor_gate.h"
#include "stg32/meteor_herd.h"
#include "stg32/meteors.h"
#include "stg32/platform.h"
#include "stg32/sky.h"
#include "stg32/tank.h"
#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[30] = {
#include "stg32/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg32/object_archives/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg32/object_archives/stage_textures.inc"
};

NJS_TEXLIST **meteor_herd_texture_groups[51] = {
#include "stg32/object_archives/meteor_herd_texture_groups.inc"
};

struct texture_pvm meteor_herd_pvms[7] = {
#include "stg32/object_archives/meteor_herd_pvms.inc"
};
