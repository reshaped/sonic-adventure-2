/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg27/object_archives/text.inc"
#include <ninja.h>

#include "stg27/object_archives.h"
#include "system/texture.h"

/* The generated texture groups use these declarations. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/capsule_bomb.h"
#include "shared/chao_key.h"
#include "shared/core_laser.h"
#include "shared/core_shutter.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/dynamite_bridge_textures.h"
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
#include "shared/last_step.h"
#include "shared/lens_flare.h"
#include "shared/patrol_camera_textures.h"
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
#include "stg27/colony_spark.h"
#include "stg27/lost_colony.h"
#include "stg27/red_lamps.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_names[27] = {
#include "stg27/object_archives/stage_names.inc"
};
static NJS_TEXLIST colony_stage_archive = {
#include "stg27/object_archives/colony_stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg27/object_archives/stage_textures.inc"
};

NJS_TEXLIST **colony_object_groups[42] = {
#include "stg27/object_archives/colony_object_groups.inc"
};

struct texture_pvm colony_object_pvms[6] = {
#include "stg27/object_archives/colony_object_pvms.inc"
};
