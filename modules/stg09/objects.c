/* the PVMs that texture_load_shared loads for Prison Lane, and its objects'
 * texture lists and kinds. each list takes its textures from the PVMs by name.
 * the stage's set files name the kinds */
#include "stg09/objects/text.inc"
#include <ninja.h>

#include "effect/burst.h"
#include "effect/texlist.h"
#include "object/animal.h"
#include "object/set.h"
#include "shared/alarm_lamp.h"
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
#include "shared/enemy_emi.h"
#include "shared/enemy_kyoko.h"
#include "shared/fluorescent_lamp.h"
#include "shared/goal_ring.h"
#include "shared/gold_beetle.h"
#include "shared/hidden_animal.h"
#include "shared/hint_box.h"
#include "shared/hornet.h"
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
#include "shared/machine.h"
#include "shared/messenger.h"
#include "shared/missile.h"
#include "shared/modifier_volume.h"
#include "shared/path_enemy.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_group.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/security_camera.h"
#include "shared/sign.h"
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
#include "stg09/bridge.h"
#include "stg09/cage_door.h"
#include "stg09/cage_gate.h"
#include "stg09/elevator.h"
#include "stg09/fence.h"
#include "stg09/gas_cylinder.h"
#include "stg09/lift.h"
#include "stg09/prison_lane.h"
#include "stg09/roller.h"
#include "stg09/searchlight.h"
#include "stg09/sliding_door.h"
#include "stg09/sound_point.h"
#include "stg09/tank.h"
#include "stg09/wall_gate.h"
#include "stg09/wide_cage_door.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_names[32] = {
#include "stg09/objects/stage_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg09/objects/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg09/objects/stage_textures.inc"
};

NJS_TEXLIST **prison_lane_texture_groups[49] = {
#include "stg09/objects/prison_lane_texture_groups.inc"
};

struct texture_pvm prison_lane_pvms[5] = {
#include "stg09/objects/prison_lane_pvms.inc"
};

struct object_kind prison_lane_kinds[PRISON_LANE_KINDS] = {
#include "stg09/objects/prison_lane_kinds.inc"
};
