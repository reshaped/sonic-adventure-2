/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg39/object_archives/text.inc"
#include <ninja.h>

/* The generated PVM and group directories reference these texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/dash_panel.h"
#include "shared/dynamite.h"
#include "shared/glow_light.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/lane.h"
#include "shared/lens_flare.h"
#include "shared/parked_car.h"
#include "shared/pulsing_light.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/sand_barrel.h"
#include "shared/signboards.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/street_fan.h"
#include "shared/street_hammer.h"
#include "shared/street_panel.h"
#include "shared/street_prop.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg39/mission_street_2p.h"
#include "stg39/object_archives.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[98] = {
#include "stg39/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg39/object_archives/stage_archive.inc"
};

static NJS_TEXLIST *stage_textures[14] = {
#include "stg39/object_archives/stage_textures.inc"
};

NJS_TEXLIST **mission_street_2p_texture_groups[26] = {
#include "stg39/object_archives/mission_street_2p_texture_groups.inc"
};

struct texture_pvm mission_street_2p_pvms[5] = {
#include "stg39/object_archives/mission_street_2p_pvms.inc"
};
