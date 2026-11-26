/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg22/object_textures/text.inc"
/* The generated PVM and group directories reference these texture lists. */
#include "stg24/fixture_textures.h"
#include "stg22/poison.h"
#include "stg22/rail_art.h"
#include <ninja.h>
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/core_laser.h"
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
#include "shared/pickup_object_2.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/rolling_drum.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/skull.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/switch_ring_group.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg22/background_textures.h"
#include "stg22/breakables.h"
#include "stg22/gravity_switch.h"
#include "stg22/grind_rail.h"
#include "stg22/rail_marker.h"
#include "stg22/scenery.h"
#include "stg22/shutter.h"
#include "stg22/stage_data.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_texlist_names[21] = {
#include "stg22/object_textures/stage_texlist_names.inc"
};
static NJS_TEXLIST stage_texlist = {
#include "stg22/object_textures/stage_texlist.inc"
};

static NJS_TEXLIST *common_group[17] = {
#include "stg22/object_textures/common_group.inc"
};

NJS_TEXLIST **gadget_object_groups[51] = {
#include "stg22/object_textures/gadget_object_groups.inc"
};

struct texture_pvm gadget_object_pvms[6] = {
#include "stg22/object_textures/gadget_object_pvms.inc"
};
