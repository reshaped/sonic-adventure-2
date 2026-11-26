/* the PVMs that texture_load_shared loads for City Escape, and its objects'
 * texture lists and kinds. each list takes its textures from the PVMs by name.
 * the stage's set files name the kinds */
#include "stg13/objects/text.inc"
#include <ninja.h>

/* The generated SET roster and texture directories use these declarations. */
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
#include "shared/common_pvm_textures.h"
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
#include "shared/path_enemy.h"
#include "shared/plain_tree.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_group.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/skull.h"
#include "shared/soap_switch.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/stop.h"
#include "shared/switch.h"
#include "shared/switch_ring_group.h"
#include "shared/technique_link.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg13/board.h"
#include "stg13/board_trigger.h"
#include "stg13/car.h"
#include "stg13/city_escape.h"
#include "stg13/guidance.h"
#include "stg13/hammer.h"
#include "stg13/iron_bar.h"
#include "stg13/lamp.h"
#include "stg13/model_objects.h"
#include "stg13/music_change.h"
#include "stg13/pickup.h"
#include "stg13/poster.h"
#include "stg13/ramp.h"
#include "stg13/road_objects.h"
#include "stg13/scenery.h"
#include "stg13/signboard.h"
#include "stg13/stage_sounds.h"
#include "stg13/tree.h"
#include "stg13/tree_shadows.h"
#include "stg13/truck.h"
#include "system/texture.h"

static NJS_TEXNAME stage_objects_names[100] = {
#include "stg13/objects/stage_objects_names.inc"
};
static NJS_TEXLIST stage_objects_textures = {
#include "stg13/objects/stage_objects_textures.inc"
};

static NJS_TEXLIST *stage_textures[17] = {
#include "stg13/objects/stage_textures.inc"
};

NJS_TEXLIST **city_escape_object_texture_groups[48] = {
#include "stg13/objects/city_escape_object_texture_groups.inc"
};

struct texture_pvm city_escape_object_pvms[5] = {
#include "stg13/objects/city_escape_object_pvms.inc"
};

struct object_kind city_escape_kinds[CITY_ESCAPE_KINDS] = {
#include "stg13/objects/city_escape_kinds.inc"
};
