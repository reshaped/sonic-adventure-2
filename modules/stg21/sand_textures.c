/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg21/sand_textures/text.inc"
#include <ninja.h>

#include "shared/ocean_util.h"
#include "stg21/background_art.h"
#include "stg21/sand_ocean.h"
#include "system/texture.h"

/* The generated texture groups use these declarations. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/chimney.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/emerald.h"
#include "shared/fire_dish.h"
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
#include "shared/moving_foothold.h"
#include "shared/ocean_fence.h"
#include "shared/ocean_pot.h"
#include "shared/pillar2.h"
#include "shared/pillar3.h"
#include "shared/pillar4.h"
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
#include "shared/turning_platform.h"
#include "shared/upgrade.h"
#include "shared/wall_fire.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg21/charge.h"
#include "stg21/corner.h"
#include "stg21/fire_fixture.h"
#include "stg21/flag.h"
#include "stg21/flag_stand.h"
#include "stg21/grave.h"
#include "stg21/pillar.h"
#include "stg21/stacked_platform.h"
#include "stg21/stone_box.h"
#include "stg21/swaying_light.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_materials_names[22] = {
#include "stg21/sand_textures/stage_materials_names.inc"
};

static NJS_TEXLIST stage_materials = {
#include "stg21/sand_textures/stage_materials.inc"
};

static NJS_TEXNAME ocean_fire_texlist_names[16] = {
#include "stg21/sand_textures/ocean_fire_texlist_names.inc"
};

NJS_TEXLIST ocean_fire_texlist = {
#include "stg21/sand_textures/ocean_fire_texlist.inc"
};

static NJS_TEXNAME ocean_quicksand_texlist_names[33] = {
#include "stg21/sand_textures/ocean_quicksand_texlist_names.inc"
};

NJS_TEXLIST ocean_quicksand_texlist = {
#include "stg21/sand_textures/ocean_quicksand_texlist.inc"
};

static NJS_TEXLIST *common_group[16] = {
#include "stg21/sand_textures/common_group.inc"
};

NJS_TEXLIST **sand_ocean_texture_groups[53] = {
#include "stg21/sand_textures/sand_ocean_texture_groups.inc"
};

struct texture_pvm sand_ocean_pvms[8] = {
#include "stg21/sand_textures/sand_ocean_pvms.inc"
};
