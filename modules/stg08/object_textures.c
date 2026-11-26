/* the PVMs that texture_load_shared loads for Security Hall, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg08/object_textures/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

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
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/knudai.h"
#include "shared/knudai_door.h"
#include "shared/knudai_foothold.h"
#include "shared/knudai_warp_hole.h"
#include "shared/lens_flare.h"
#include "shared/pickup_object_2.h"
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
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg08/background.h"
#include "stg08/cash_box.h"
#include "stg08/lamp.h"
#include "stg08/laser.h"
#include "stg08/machines.h"
#include "stg08/paper.h"
#include "stg08/security_hall.h"
#include "stg08/security_props.h"
#include "stg08/solid_props.h"
#include "system/texture.h"

#include "shared/emerald_shard.h"
#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_names[40] = {
#include "stg08/object_textures/stage_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg08/object_textures/stage_archive.inc"
};

/* the texture lists of the three emerald pieces, one texture each, which
 * security_hall.c sets in emerald_look. the pieces share one model */

static NJS_TEXNAME security_piece_3_names[1] = {
#include "stg08/object_textures/security_piece_3_names.inc"
};
NJS_TEXLIST security_piece_3_texlist = {
#include "stg08/object_textures/security_piece_3_texlist.inc"
};

static NJS_TEXNAME security_piece_1_names[1] = {
#include "stg08/object_textures/security_piece_1_names.inc"
};
NJS_TEXLIST security_piece_1_texlist = {
#include "stg08/object_textures/security_piece_1_texlist.inc"
};

static NJS_TEXNAME security_piece_2_names[1] = {
#include "stg08/object_textures/security_piece_2_names.inc"
};
NJS_TEXLIST security_piece_2_texlist = {
#include "stg08/object_textures/security_piece_2_texlist.inc"
};

static struct emerald_shard_polygon_list emerald_shard_polygons = {
#include "stg08/object_textures/emerald_shard_polygons.inc"
};

static Sint32 emerald_shard_vertices[747] = {
#include "stg08/object_textures/emerald_shard_vertices.inc"
};

NJS_CNK_MODEL security_piece_model = {
#include "stg08/object_textures/security_piece_model.inc"
};

static NJS_CNK_OBJECT security_piece_object = {
#include "stg08/object_textures/security_piece_object.inc"
};

static NJS_TEXLIST *stage_textures[19] = {
#include "stg08/object_textures/stage_textures.inc"
};

NJS_TEXLIST **security_texture_groups[45] = {
#include "stg08/object_textures/security_texture_groups.inc"
};

struct texture_pvm security_pvms[5] = {
#include "stg08/object_textures/security_pvms.inc"
};
