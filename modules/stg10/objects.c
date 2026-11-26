/* the PVMs that texture_load_shared loads for Metal Harbor, and its objects'
 * texture lists and kinds. each list takes its textures from the PVMs by name.
 * the stage's set files name the kinds */
#include "stg10/objects/text.inc"
#include <ninja.h>

#include "object/set.h"
#include "stg10/metal_harbor.h"
#include "system/texture.h"

/* The generated SET and PVM tables name these callbacks and textures. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "object/animal.h"
#include "shared/beetle.h"
#include "shared/big_jump.h"
#include "shared/big_the_cat.h"
#include "shared/cage.h"
#include "shared/chao_box.h"
#include "shared/chao_key.h"
#include "shared/chao_pipe.h"
#include "shared/collision_object.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dummy_object.h"
#include "shared/dynamite.h"
#include "shared/effect_object.h"
#include "shared/emerald.h"
#include "shared/enemy_kyoko.h"
#include "shared/global_light_switch.h"
#include "shared/goal_ring.h"
#include "shared/gold_beetle.h"
#include "shared/hidden_animal.h"
#include "shared/hint_box.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/knudai.h"
#include "shared/knudai_door.h"
#include "shared/knudai_foothold.h"
#include "shared/knudai_item_box.h"
#include "shared/knudai_ring_group.h"
#include "shared/knudai_spring.h"
#include "shared/knudai_warp_hole.h"
#include "shared/lens_flare.h"
#include "shared/light_switch.h"
#include "shared/messenger.h"
#include "shared/missile.h"
#include "shared/modifier_volume.h"
#include "shared/path_enemy.h"
#include "shared/pickup_object.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_group.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/shouko.h"
#include "shared/skull.h"
#include "shared/soap_switch.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/stop.h"
#include "shared/stopper.h"
#include "shared/switch.h"
#include "shared/technique_link.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg10/board.h"
#include "stg10/board_trigger.h"
#include "stg10/breakable_slab.h"
#include "stg10/clouds.h"
#include "stg10/container.h"
#include "stg10/container_0.h"
#include "stg10/container_b.h"
#include "stg10/harbor_missile.h"
#include "stg10/harbor_stopper.h"
#include "stg10/harbor_tank.h"
#include "stg10/missile_bridge.h"
#include "stg10/radar.h"
#include "stg10/scenery.h"
#include "stg10/seagulls.h"
#include "stg10/signal.h"
#include "stg10/truck.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_names[31] = {
#include "stg10/objects/stage_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg10/objects/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg10/objects/stage_textures.inc"
};

NJS_TEXLIST **metal_harbor_object_texture_groups[48] = {
#include "stg10/objects/metal_harbor_object_texture_groups.inc"
};

struct texture_pvm metal_harbor_object_pvms[5] = {
#include "stg10/objects/metal_harbor_object_pvms.inc"
};

struct object_kind metal_harbor_kinds[METAL_HARBOR_KINDS] = {
#include "stg10/objects/metal_harbor_kinds.inc"
};
