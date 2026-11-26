/* the PVMs that texture_load_shared loads for Radical Highway, and its objects'
 * texture lists and kinds. each list takes its textures from the PVMs by name.
 * the stage's set files name the kinds */
#include "stg14/objects/text.inc"
#include <ninja.h>

#include "object/set.h"
#include "stg14/radical_highway.h"
#include "system/texture.h"

/* The generated SET and texture directories use these declarations. */
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
#include "shared/enemy_ai.h"
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
#include "shared/no_input.h"
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
#include "shared/switch.h"
#include "shared/technique_link.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg14/airship.h"
#include "stg14/chase_light.h"
#include "stg14/fan.h"
#include "stg14/glow_light.h"
#include "stg14/gust_sound.h"
#include "stg14/hammer.h"
#include "stg14/highway_scenery.h"
#include "stg14/iron_bar.h"
#include "stg14/lane.h"
#include "stg14/nights.h"
#include "stg14/poster.h"
#include "stg14/pulsing_light.h"
#include "stg14/rail.h"
#include "stg14/ramp.h"
#include "stg14/road_props.h"
#include "stg14/spinning_hammer.h"
#include "stg14/updraft.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_names[56] = {
#include "stg14/objects/stage_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg14/objects/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg14/objects/stage_textures.inc"
};

NJS_TEXLIST **radical_highway_texture_groups[49] = {
#include "stg14/objects/radical_highway_texture_groups.inc"
};

struct texture_pvm radical_highway_pvms[5] = {
#include "stg14/objects/radical_highway_pvms.inc"
};

struct object_kind radical_highway_kinds[RADICAL_HIGHWAY_KINDS] = {
#include "stg14/objects/radical_highway_kinds.inc"
};
