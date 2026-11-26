/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg37/object_archives/text.inc"
#include <ninja.h>

#include "stg37/object_archives.h"
#include "system/texture.h"

/* The generated group and PVM directories use these external texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/core_block.h"
#include "shared/core_goal.h"
#include "shared/core_laser.h"
#include "shared/core_scenery_art.h"
#include "shared/core_step.h"
#include "shared/core_switch_parts.h"
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
#include "shared/time_switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/water_ripple.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg37/core_rouge.h"
#include "stg37/water_scenery.h"

#include "shared/common_pvm_textures.h"

#include "shared/core_object_archive.h"

static NJS_TEXNAME surface_textures_names[16] = {
#include "stg37/object_archives/surface_textures_names.inc"
};
NJS_TEXLIST surface_textures = {
#include "stg37/object_archives/surface_textures.inc"
};

static NJS_TEXNAME moving_water_textures_names[MOVING_WATER_TEXTURES] = {
#include "stg37/object_archives/moving_water_textures_names.inc"
};
NJS_TEXLIST moving_water_textures = {
#include "stg37/object_archives/moving_water_textures.inc"
};

#include "shared/core_block_glow_archive.h"
NJS_TEXLIST core_block_glow_texlist = {
#include "stg37/object_archives/core_block_glow_texlist.inc"
};

static NJS_TEXLIST *stage_textures[17] = {
#include "stg37/object_archives/stage_textures.inc"
};

NJS_TEXLIST **core_rouge_object_groups[38] = {
#include "stg37/object_archives/core_rouge_object_groups.inc"
};

struct texture_pvm core_rouge_object_pvms[8] = {
#include "stg37/object_archives/core_rouge_object_pvms.inc"
};
