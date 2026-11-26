/* the texture groups that the three mech bosses' fights share. stage_textures
 * is static, so each fight's object_archives.c holds its own copy */
#ifndef SHARED_GUN_MECH_STAGE_TEXTURES_H
#define SHARED_GUN_MECH_STAGE_TEXTURES_H

#include <ninja.h>
/* IWYU pragma: begin_exports */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
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
#include "shared/mech_boss.h"
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
#include "shared/weight.h"
#include "shared/wood_box.h"
/* IWYU pragma: end_exports */

static NJS_TEXLIST *stage_textures[17] = {
#include "shared/gun_mech/stage_textures/stage_textures.inc"
};

#endif
