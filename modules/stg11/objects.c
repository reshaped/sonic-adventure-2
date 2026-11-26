/* the PVMs that texture_load_shared loads for Iron Gate, and its objects'
 * texture lists and kinds. each list takes its textures from the PVMs by name.
 * the stage's set files name the kinds */
#include "stg11/objects/text.inc"
#include <ninja.h>

/* The generated SET roster names these actor constructors. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "object/animal.h"
#include "object/set.h"
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
#include "shared/dynamite.h"
#include "shared/effect_object.h"
#include "shared/emerald.h"
#include "shared/enemy_ai.h"
#include "shared/enemy_emi.h"
#include "shared/enemy_kyoko.h"
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
#include "shared/light_switch.h"
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
#include "shared/skull.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/stop.h"
#include "shared/switch.h"
#include "shared/technique_link.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/water_bubbles.h"
#include "shared/water_ripple.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg11/alarm_lamp.h"
#include "stg11/bridge.h"
#include "stg11/elevator.h"
#include "stg11/fence.h"
#include "stg11/fluorescent_lamp.h"
#include "stg11/gas_cylinder.h"
#include "stg11/iron_gate.h"
#include "stg11/lift.h"
#include "stg11/lock_on_missile.h"
#include "stg11/machine.h"
#include "stg11/missile_shutter.h"
#include "stg11/security_camera.h"
#include "stg11/shutter.h"
#include "stg11/side_shutter.h"
#include "stg11/sign.h"
#include "stg11/sound_point.h"
#include "stg11/tank.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_names[37] = {
#include "stg11/objects/stage_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg11/objects/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg11/objects/stage_textures.inc"
};

NJS_TEXLIST **iron_gate_texture_groups[47] = {
#include "stg11/objects/iron_gate_texture_groups.inc"
};

struct texture_pvm iron_gate_pvms[5] = {
#include "stg11/objects/iron_gate_pvms.inc"
};

struct object_kind iron_gate_kinds[IRON_GATE_KINDS] = {
#include "stg11/objects/iron_gate_kinds.inc"
};
