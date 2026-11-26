/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg29/object_archives/text.inc"
#include <ninja.h>

/* The generated PVM and group directories reference these texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/boss.h"
#include "shared/cage.h"
#include "shared/capsule_bomb.h"
#include "shared/dash_panel.h"
#include "shared/dynamite.h"
#include "shared/hint_box.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/lens_flare.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/skull.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg29/object_archives.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[22] = {
#include "stg29/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg29/object_archives/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[14] = {
#include "stg29/object_archives/stage_textures.inc"
};

NJS_TEXLIST **versus_mech_2_texture_groups[20] = {
#include "stg29/object_archives/versus_mech_2_texture_groups.inc"
};

struct texture_pvm versus_mech_2_pvms[5] = {
#include "stg29/object_archives/versus_mech_2_pvms.inc"
};
