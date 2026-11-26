/* the PVMs that texture_load_shared loads for the stage, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "stg45/background_models/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

/* The generated texture groups use these declarations. */
#include "effect/burst.h"
#include "effect/texlist.h"
#include "shared/big_jump.h"
#include "shared/cage.h"
#include "shared/chimney.h"
#include "shared/dash_panel.h"
#include "shared/dynamite.h"
#include "shared/fire_dish.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/lens_flare.h"
#include "shared/moving_foothold.h"
#include "shared/ocean_fence.h"
#include "shared/ocean_pot.h"
#include "shared/ocean_util.h"
#include "shared/pillar2.h"
#include "shared/pillar3.h"
#include "shared/pillar4.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket.h"
#include "shared/solid_box.h"
#include "shared/spring.h"
#include "shared/stage_texture_groups.h"
#include "shared/switch.h"
#include "shared/triple_spring.h"
#include "shared/turning_platform.h"
#include "shared/weight.h"
#include "shared/wood_box.h"
#include "stg45/background_art.h"
#include "stg45/sand_ocean_2p.h"
#include "system/texture.h"

/* The object archive precedes the effect archive in this member. */
#include "shared/objtex_common.h"

#include "shared/efftex_common.h"

static NJS_TEXNAME ocean_2p_stage_texlist_names[76] = {
#include "stg45/background_models/ocean_2p_stage_texlist_names.inc"
};

NJS_TEXLIST ocean_2p_stage_texlist = {
#include "stg45/background_models/ocean_2p_stage_texlist.inc"
};

static NJS_TEXNAME fire_names[16] = {
#include "stg45/background_models/fire_names.inc"
};

NJS_TEXLIST ocean_fire_texlist = {
#include "stg45/background_models/ocean_fire_texlist.inc"
};

static NJS_TEXNAME quicksand_texlist_names[32] = {
#include "stg45/background_models/quicksand_texlist_names.inc"
};

NJS_TEXLIST ocean_2p_quicksand_texlist = {
#include "stg45/background_models/ocean_2p_quicksand_texlist.inc"
};

static Sint16 quicksand_object_polygons[533] = {
#include "stg45/background_models/quicksand_object_polygons.inc"
};

static Sint32 quicksand_object_vertices[295] = {
#include "stg45/background_models/quicksand_object_vertices.inc"
};

static NJS_CNK_MODEL quicksand_object_model = {
#include "stg45/background_models/quicksand_object_model.inc"
};

NJS_CNK_OBJECT ocean_2p_quicksand_object = {
#include "stg45/background_models/ocean_2p_quicksand_object.inc"
};

static NJS_TEXNAME sky_texlist_names[5] = {
#include "stg45/background_models/sky_texlist_names.inc"
};

NJS_TEXLIST ocean_2p_sky_texlist = {
#include "stg45/background_models/ocean_2p_sky_texlist.inc"
};

static Sint16 sky_object_polygons[753] = {
#include "stg45/background_models/sky_object_polygons.inc"
};

static Sint32 sky_object_vertices[551] = {
#include "stg45/background_models/sky_object_vertices.inc"
};

static NJS_CNK_MODEL sky_object_model = {
#include "stg45/background_models/sky_object_model.inc"
};

NJS_CNK_OBJECT ocean_2p_sky_object = {
#include "stg45/background_models/ocean_2p_sky_object.inc"
};
static NJS_TEXNAME moon_texlist_names[1] = {
#include "stg45/background_models/moon_texlist_names.inc"
};

NJS_TEXLIST ocean_2p_moon_texlist = {
#include "stg45/background_models/ocean_2p_moon_texlist.inc"
};

static Sint16 moon_object_polygons[25] = {
#include "stg45/background_models/moon_object_polygons.inc"
};

static Sint32 moon_object_vertices[19] = {
#include "stg45/background_models/moon_object_vertices.inc"
};

static NJS_CNK_MODEL moon_object_model = {
#include "stg45/background_models/moon_object_model.inc"
};

NJS_CNK_OBJECT ocean_2p_moon_object = {
#include "stg45/background_models/ocean_2p_moon_object.inc"
};

static NJS_TEXLIST *common_group[17] = {
#include "stg45/background_models/common_group.inc"
};

NJS_TEXLIST **sand_ocean_2p_texture_groups[26] = {
#include "stg45/background_models/sand_ocean_2p_texture_groups.inc"
};

struct texture_pvm sand_ocean_2p_pvms[6] = {
#include "stg45/background_models/sand_ocean_2p_pvms.inc"
};
