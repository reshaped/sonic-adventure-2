/* the PVMs that texture_load_shared loads for Wild Canyon, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg16/object_archives/text.inc"
#include <ninja.h>

/* The generated registry names these object texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/board.h"
#include "shared/board2.h"
#include "shared/bone.h"
#include "shared/cage.h"
#include "shared/canyon_wind_art.h"
#include "shared/chao_key.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/emerald.h"
#include "shared/fan.h"
#include "shared/flag.h"
#include "shared/flag_low.h"
#include "shared/floating_rock.h"
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
#include "shared/pole.h"
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
#include "shared/stone_head.h"
#include "shared/switch.h"
#include "shared/treasure_box.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/wall_warp.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg16/object_archives.h"
#include "stg16/wild_canyon.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

#include "stg16/object_archive.h"

static NJS_TEXLIST *stage_textures[16] = {
#include "stg16/object_archives/stage_textures.inc"
};

NJS_TEXLIST **wild_canyon_texture_groups[47] = {
#include "stg16/object_archives/wild_canyon_texture_groups.inc"
};

struct texture_pvm wild_canyon_pvms[6] = {
#include "stg16/object_archives/wild_canyon_pvms.inc"
};
