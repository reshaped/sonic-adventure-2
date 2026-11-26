/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg40/object_archives/text.inc"
#include <ninja.h>

#include "stg40/object_archives.h"
#include "system/texture.h"

/* The generated groups and PVMs reference these texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/arrow.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chao_key.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/emerald.h"
#include "shared/frstage.h"
#include "shared/goal_ring.h"
#include "shared/grind_rail.h"
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
#include "shared/light_bar.h"
#include "shared/pic.h"
#include "shared/pickup_object_2.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/ringkaz.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/search_box.h"
#include "shared/sign_end_e.h"
#include "shared/skull.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg40/gravity_cylinder.h"
#include "stg40/meteor.h"
#include "stg40/sign_end.h"
#include "stg40/sky_land.h"
#include "stg40/terrain.h"
#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[48] = {
#include "stg40/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg40/object_archives/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg40/object_archives/stage_textures.inc"
};

NJS_TEXLIST **final_chase_texture_groups[42] = {
#include "stg40/object_archives/final_chase_texture_groups.inc"
};

struct texture_pvm final_chase_pvms[7] = {
#include "stg40/object_archives/final_chase_pvms.inc"
};
