/* the PVMs that texture_load_shared loads for the fight, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "boss_bigbogy/object_archives/text.inc"
#include <ninja.h>

#include "boss_bigbogy/object_archives.h"
#include "system/texture.h"
#include "boss_bigbogy/door_art.h"
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
#include "shared/sandglass.h"
#include "shared/save_point.h"
#include "shared/skull.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/upgrade.h"
#include "shared/weight.h"
#include "shared/wood_box.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME room_textures[6] = {
#include "boss_bigbogy/object_archives/room_textures.inc"
};

static NJS_TEXLIST room_texlist = {
#include "boss_bigbogy/object_archives/room_texlist.inc"
};

static NJS_TEXLIST *stage_textures[17] = {
#include "boss_bigbogy/object_archives/stage_textures.inc"
};

NJS_TEXLIST **bigbogy_texture_groups[22] = {
#include "boss_bigbogy/object_archives/bigbogy_texture_groups.inc"
};

struct texture_pvm bigbogy_pvms[5] = {
#include "boss_bigbogy/object_archives/bigbogy_pvms.inc"
};
