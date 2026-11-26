/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg25/object_archives/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "stg25/object_archives.h"
#include "system/texture.h"

/* The generated archive directories reference the texture lists declared here.
 */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/egg_key_art.h"
#include "shared/egg_key_geometry.h"
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
#include "shared/painting.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/sandglass.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/shaft.h"
#include "shared/skull.h"
#include "shared/solid_box_e.h"
#include "shared/spinner.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/store_box.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/water_ripple.h"
#include "shared/water_specks.h"
#include "shared/weight.h"
#include "shared/wood_box_e.h"
#include "stg25/bats.h"
#include "stg25/biribiri.h"
#include "stg25/bug_ornament.h"
#include "stg25/death_chamber.h"
#include "stg25/door_light.h"
#include "stg25/fan.h"
#include "stg25/fire_mouth.h"
#include "stg25/fire_pot.h"
#include "stg25/floor_tiles.h"
#include "stg25/ground_warp.h"
#include "stg25/lion_head.h"
#include "stg25/map.h"
#include "stg25/mecha.h"
#include "stg25/mikoshi.h"
#include "stg25/round_shadow.h"
#include "stg25/sandglass_door.h"
#include "stg25/scorpion.h"
#include "stg25/torch.h"
#include "stg25/torch2.h"
#include "stg25/treasure.h"
#include "stg25/wall_warp.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[50] = {
#include "stg25/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg25/object_archives/stage_archive.inc"
};

static NJS_TEXNAME chamber_flame_blue_texlist_names[16] = {
#include "stg25/object_archives/chamber_flame_blue_texlist_names.inc"
};
NJS_TEXLIST chamber_flame_blue_texlist = {
#include "stg25/object_archives/chamber_flame_blue_texlist.inc"
};

static NJS_TEXNAME chamber_flame_green_texlist_names[16] = {
#include "stg25/object_archives/chamber_flame_green_texlist_names.inc"
};
NJS_TEXLIST chamber_flame_green_texlist = {
#include "stg25/object_archives/chamber_flame_green_texlist.inc"
};

static NJS_TEXNAME chamber_flame_red_texlist_names[16] = {
#include "stg25/object_archives/chamber_flame_red_texlist_names.inc"
};
NJS_TEXLIST chamber_flame_red_texlist = {
#include "stg25/object_archives/chamber_flame_red_texlist.inc"
};

static NJS_TEXNAME death_chamber_map_texlist_names[3] = {
#include "stg25/object_archives/death_chamber_map_texlist_names.inc"
};
NJS_TEXLIST death_chamber_map_texlist = {
#include "stg25/object_archives/death_chamber_map_texlist.inc"
};

static NJS_TEXNAME eggman_container_textures_names[4] = {
#include "stg25/object_archives/eggman_container_textures_names.inc"
};
static NJS_TEXLIST eggman_container_textures = {
#include "stg25/object_archives/eggman_container_textures.inc"
};

static NJS_TEXNAME egg_key_names[3] = {
#include "stg25/object_archives/egg_key_names.inc"
};

NJS_TEXLIST egg_key_texlist = {
#include "stg25/object_archives/egg_key_texlist.inc"
};

static Sint16 egg_key_polygons[699] = {
#include "stg25/object_archives/egg_key_polygons.inc"
};

static struct egg_key_vertices egg_key_vertices = {
#include "stg25/object_archives/egg_key_vertices.inc"
};

NJS_CNK_MODEL egg_key_model[1] = {
#include "stg25/object_archives/egg_key_model.inc"
};

static NJS_CNK_OBJECT egg_key_object = {
#include "stg25/object_archives/egg_key_object.inc"
};

static NJS_TEXNAME egg_key_glow_names[3] = {
#include "stg25/object_archives/egg_key_glow_names.inc"
};

static NJS_TEXLIST egg_key_glow_texlist = {
#include "stg25/object_archives/egg_key_glow_texlist.inc"
};

static Sint16 egg_key_glow_polygons[65] = {
#include "stg25/object_archives/egg_key_glow_polygons.inc"
};

static Sint32 egg_key_glow_vertices[99] = {
#include "stg25/object_archives/egg_key_glow_vertices.inc"
};

NJS_CNK_MODEL egg_key_glow_model[1] = {
#include "stg25/object_archives/egg_key_glow_model.inc"
};

static NJS_CNK_OBJECT egg_key_glow_object = {
#include "stg25/object_archives/egg_key_glow_object.inc"
};

static NJS_TEXLIST *stage_textures[19] = {
#include "stg25/object_archives/stage_textures.inc"
};

NJS_TEXLIST **death_chamber_texture_groups[58] = {
#include "stg25/object_archives/death_chamber_texture_groups.inc"
};

struct texture_pvm death_chamber_pvms[11] = {
#include "stg25/object_archives/death_chamber_pvms.inc"
};
