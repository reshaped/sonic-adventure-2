#include <ninja.h>
#include <sg_xpt.h>
#include "chao/garden_scene.h"
#include "chaostgrace/edit_menu.h"
#include "chaostgrace/scene.h"
#include "chaostgrace/scene_files.h"
#include "object/set.h"
#include "shared/sparkle.h"
#include "shared/water_ripple.h"
#include "system/texture.h"

Sint32 race_hide_selected_camera = 0;
static NJS_TEXLIST *effects[3] = {
#include "chaostgrace/archives/effects.inc"
};
NJS_TEXLIST **race_effect_groups[2] = {
#include "chaostgrace/archives/race_effect_groups.inc"
};
struct texture_pvm race_effect_textures[2] = {
#include "chaostgrace/archives/race_effect_textures.inc"
};
struct set_head *race_layout = NULL;
