/* the PVMs that texture_load_shared loads for White Jungle, and its objects'
 * texture lists and kinds. each list takes its textures from the PVMs by name.
 * the stage's set files name the kinds */
#include "stg04/objects/text.inc"
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
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dummy_object.h"
#include "shared/dynamite.h"
#include "shared/effect_object.h"
#include "shared/emerald.h"
#include "shared/enemy_ai.h"
#include "shared/enemy_emi.h"
#include "shared/goal_ring.h"
#include "shared/gold_beetle.h"
#include "shared/hidden_animal.h"
#include "shared/hint_box.h"
#include "shared/hover_lift.h"
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
#include "shared/messenger.h"
#include "shared/missile.h"
#include "shared/modifier_volume.h"
#include "shared/no_input.h"
#include "shared/path_enemy.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_group.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
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
#include "shared/wood_fence.h"
#include "stg04/bungee_vine.h"
#include "stg04/fallen_tree.h"
#include "stg04/hanging_vine.h"
#include "stg04/jungle_pickup.h"
#include "stg04/jungle_prop.h"
#include "stg04/revolving_vine.h"
#include "stg04/side_vine.h"
#include "stg04/slgate.h"
#include "stg04/solid_scenery.h"
#include "stg04/stars.h"
#include "stg04/taki.h"
#include "stg04/torch.h"
#include "stg04/white_jungle.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_objects_names[45] = {
#include "stg04/objects/stage_objects_names.inc"
};
static NJS_TEXLIST stage_objects_textures = {
#include "stg04/objects/stage_objects_textures.inc"
};

static NJS_TEXLIST *stage_textures[24] = {
#include "stg04/objects/stage_textures.inc"
};

NJS_TEXLIST **white_jungle_object_texture_groups[37] = {
#include "stg04/objects/white_jungle_object_texture_groups.inc"
};

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side: the table's fifth
 * entry, zero, is its own stop */
extern struct texture_pvm white_jungle_object_pvms[5];
struct texture_pvm white_jungle_object_pvms[] = {
#include "stg04/objects/white_jungle_object_pvms.inc"
};
#else
/* @bug memory: the table has no null entry, so texture_load_shared and
 * texture_release_shared read on into white_jungle_kinds[0], whose zero range
 * stops them. the range is at 0x8c58cb3c. texture_load_shared reads it as a
 * texlist at 0x8c060a64, and texture_release_shared at 0x8c060b08 */
struct texture_pvm white_jungle_object_pvms[4] = {
#include "stg04/objects/white_jungle_object_pvms.inc"
};
#endif

struct object_kind white_jungle_kinds[WHITE_JUNGLE_KINDS] = {
#include "stg04/objects/white_jungle_kinds.inc"
};
