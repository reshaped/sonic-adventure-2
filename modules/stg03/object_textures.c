/* the PVMs that texture_load_shared loads for Green Forest, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg03/object_textures/text.inc"
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
#include "shared/fallen_tree.h"
#include "shared/goal_ring.h"
#include "shared/hint_box.h"
#include "shared/hover_lift.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/jungle_pickup.h"
#include "shared/jungle_prop.h"
#include "shared/knudai.h"
#include "shared/knudai_door.h"
#include "shared/knudai_foothold.h"
#include "shared/knudai_warp_hole.h"
#include "shared/reel.h"
#include "shared/revolving_vine.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/skull.h"
#include "shared/solid_box.h"
#include "shared/solid_scenery.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/test_wood.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/vine.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "shared/wood_fence.h"
#include "stg03/arrow_sign.h"
#include "stg03/bungee_vine.h"
#include "stg03/dragonfly.h"
#include "stg03/falling_cliff.h"
#include "stg03/green_forest.h"
#include "stg03/stars.h"
#include "system/texture.h"

#include "shared/objtex_common.h"
#include "shared/efftex_common.h"

static NJS_TEXNAME stage_texture_names[22] = {
#include "stg03/object_textures/stage_texture_names.inc"
};

static NJS_TEXLIST stage_textures = {
#include "stg03/object_textures/stage_textures.inc"
};

static NJS_TEXLIST *common_group[23] = {
#include "stg03/object_textures/common_group.inc"
};

NJS_TEXLIST **green_forest_object_texture_groups[39] = {
#include "stg03/object_textures/green_forest_object_texture_groups.inc"
};

struct texture_pvm green_forest_object_pvms[8] = {
#include "stg03/object_textures/green_forest_object_pvms.inc"
};
