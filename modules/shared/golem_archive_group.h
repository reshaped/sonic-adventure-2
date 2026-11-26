/* Texture lists used by the common actors in both Egg Golem fights. */
#ifndef SHARED_GOLEM_ARCHIVE_GROUP_H
#define SHARED_GOLEM_ARCHIVE_GROUP_H

#include <ninja.h>
/* IWYU pragma: begin_exports */
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
#include "shared/golem_foothold_art.h"
#include "shared/golem_land.h"
#include "shared/golem_reel.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/ring.h"
#include "shared/rocket.h"
#include "shared/save_point.h"
#include "shared/skull.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/ugokidai.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
/* IWYU pragma: end_exports */

static NJS_TEXLIST *common_group[16] = {
#include "shared/golem_archive_group/common_group.inc"
};

#endif
