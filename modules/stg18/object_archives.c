/* the PVMs that texture_load_shared loads for Dry Lagoon, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg18/object_archives/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/object_archives.h"
#include "system/texture.h"

/* The generated group and PVM directories use these external texture lists. */
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
#include "shared/iron_box.h"
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
#include "shared/small_pot.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/water_ripple.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg18/butterflies.h"
#include "stg18/canoe.h"
#include "stg18/chin.h"
#include "stg18/chin2.h"
#include "stg18/chin3.h"
#include "stg18/container_art.h"
#include "stg18/dry_lagoon.h"
#include "stg18/fish.h"
#include "stg18/flower.h"
#include "stg18/leaf.h"
#include "stg18/palm.h"
#include "stg18/plant.h"
#include "stg18/plant2.h"
#include "stg18/plant3.h"
#include "stg18/plant4.h"
#include "stg18/pool.h"
#include "stg18/pot.h"
#include "stg18/sasa.h"
#include "stg18/shelf.h"
#include "stg18/sign_plate.h"
#include "stg18/specks.h"
#include "stg18/spray.h"
#include "stg18/stake.h"
#include "stg18/stone_head.h"
#include "stg18/turtle.h"
#include "stg18/wall_warp.h"
#include "stg18/water.h"

#include "shared/emerald_shard.h"
#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[42] = {
#include "stg18/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg18/object_archives/stage_archive.inc"
};

/* a shard's texture list of three textures, its model and its node. the
 * stage's texture groups name the list. no code reads the model or the node */
static NJS_TEXNAME lagoon_shard_names[3] = {
#include "stg18/object_archives/lagoon_shard_names.inc"
};
static NJS_TEXLIST lagoon_shard_texlist = {
#include "stg18/object_archives/lagoon_shard_texlist.inc"
};

static struct emerald_shard_polygon_list emerald_shard_polygons = {
#include "stg18/object_archives/emerald_shard_polygons.inc"
};

static Sint32 emerald_shard_vertices[747] = {
#include "stg18/object_archives/emerald_shard_vertices.inc"
};

static NJS_CNK_MODEL lagoon_shard_model = {
#include "stg18/object_archives/lagoon_shard_model.inc"
};

static NJS_CNK_OBJECT lagoon_shard_object = {
#include "stg18/object_archives/lagoon_shard_object.inc"
};

static NJS_TEXLIST *stage_textures[19] = {
#include "stg18/object_archives/stage_textures.inc"
};

NJS_TEXLIST **dry_lagoon_texture_groups[56] = {
#include "stg18/object_archives/dry_lagoon_texture_groups.inc"
};

struct texture_pvm dry_lagoon_pvms[7] = {
#include "stg18/object_archives/dry_lagoon_pvms.inc"
};
