/* the PVMs that texture_load_shared loads for the fight, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "boss_last1/object_archives/text.inc"
#include <ninja.h>

#include "boss_last1/object_archives.h"
#include "system/texture.h"
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/boss.h"
#include "shared/cage.h"
#include "shared/dash_panel.h"
#include "shared/dirt.h"
#include "shared/dynamite.h"
#include "shared/emerald.h"
#include "shared/goal_ring.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/lens_flare.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/skull.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/water_ripple.h"
#include "shared/weight.h"
#include "shared/wood_box.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXLIST *stage_textures[18] = {
#include "boss_last1/object_archives/stage_textures.inc"
};

NJS_TEXLIST **biolizard_texture_groups[20] = {
#include "boss_last1/object_archives/biolizard_texture_groups.inc"
};

struct texture_pvm biolizard_pvms[4] = {
#include "boss_last1/object_archives/biolizard_pvms.inc"
};
