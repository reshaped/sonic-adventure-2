/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg28/object_archives/text.inc"
#include <ninja.h>

/* The generated registry names these object texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/emerald.h"
#include "shared/goal_ring.h"
#include "shared/hint_box.h"
#include "shared/iron_ball.h"
#include "shared/iron_box_e.h"
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
#include "shared/solid_box_e.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/switch_ring_group.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box_e.h"
#include "stg28/awning.h"
#include "stg28/block.h"
#include "stg28/cave_board.h"
#include "stg28/cave_pyramid.h"
#include "stg28/chimney.h"
#include "stg28/eye.h"
#include "stg28/firepot.h"
#include "stg28/hang_ring.h"
#include "stg28/iron_bar.h"
#include "stg28/key.h"
#include "stg28/key_door.h"
#include "stg28/keyhole.h"
#include "stg28/neon.h"
#include "stg28/object_archives.h"
#include "stg28/pyramid_cave.h"
#include "stg28/sandglass.h"
#include "stg28/sandglass_door.h"
#include "stg28/ship.h"
#include "stg28/snake_dish.h"
#include "stg28/snake_dish2.h"
#include "stg28/snake_head.h"
#include "stg28/snake_rail.h"
#include "stg28/snake_statue.h"
#include "stg28/spider_web.h"
#include "stg28/torch_cup.h"
#include "stg28/wall.h"
#include "stg28/weed.h"
#include "stg28/windmill.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[49] = {
#include "stg28/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg28/object_archives/stage_archive.inc"
};

static NJS_TEXNAME pyramid_cave_fire_texlist_names[16] = {
#include "stg28/object_archives/pyramid_cave_fire_texlist_names.inc"
};
NJS_TEXLIST pyramid_cave_fire_texlist = {
#include "stg28/object_archives/pyramid_cave_fire_texlist.inc"
};

static NJS_TEXNAME eggman_container_textures_names[4] = {
#include "stg28/object_archives/eggman_container_textures_names.inc"
};
static NJS_TEXLIST eggman_container_textures = {
#include "stg28/object_archives/eggman_container_textures.inc"
};

static NJS_TEXLIST *stage_textures[17] = {
#include "stg28/object_archives/stage_textures.inc"
};

NJS_TEXLIST **pyramid_cave_texture_groups[62] = {
#include "stg28/object_archives/pyramid_cave_texture_groups.inc"
};

struct texture_pvm pyramid_cave_pvms[7] = {
#include "stg28/object_archives/pyramid_cave_pvms.inc"
};
