/* the PVMs that texture_load_shared loads for Sonic and Shadow's first duel,
 * and its objects' texture lists. each list takes its textures from the PVMs by
 * name */
#include "stg19/object_archives/text.inc"
#include <ninja.h>

/* The generated PVM and group directories reference these texture lists. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/boss.h"
#include "shared/cage.h"
#include "shared/fallen_tree.h"
#include "shared/flame.h"
#include "shared/hint_box.h"
#include "shared/hover_lift.h"
#include "shared/iron_box.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/jungle_prop.h"
#include "shared/looping_vine.h"
#include "shared/reel.h"
#include "shared/revolving_vine.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/search_box.h"
#include "shared/skull.h"
#include "shared/solid_scenery.h"
#include "shared/stage_texture_groups.h"
#include "shared/test_wood.h"
#include "shared/vine.h"
#include "shared/white_breath.h"
#include "shared/wood_box.h"
#include "shared/wood_fence.h"
#include "stg19/object_archives.h"
#include "stg19/versus_sonic_1.h"
#include "system/texture.h"

#include "shared/common_pvm_textures.h"

static NJS_TEXNAME stage_archive_names[24] = {
#include "stg19/object_archives/stage_archive_names.inc"
};
static NJS_TEXLIST stage_archive = {
#include "stg19/object_archives/stage_archive.inc"
};

static NJS_TEXNAME water_frames[14] = {
#include "stg19/object_archives/water_frames.inc"
};
NJS_TEXLIST versus_sonic_1_water_texlist = {
#include "stg19/object_archives/versus_sonic_1_water_texlist.inc"
};

static NJS_TEXLIST *stage_textures[16] = {
#include "stg19/object_archives/stage_textures.inc"
};

NJS_TEXLIST **versus_sonic_1_object_texture_groups[16] = {
#include "stg19/object_archives/versus_sonic_1_object_texture_groups.inc"
};

struct texture_pvm versus_sonic_1_object_pvms[6] = {
#include "stg19/object_archives/versus_sonic_1_object_pvms.inc"
};
