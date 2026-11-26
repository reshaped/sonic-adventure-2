/* the background's PVM table and texture lists. texture_load_shared fills the
 * machinery's lists from the background's PVM */
#ifndef STG35_MACHINERY_TEXTURES_H
#define STG35_MACHINERY_TEXTURES_H
#include <ninja.h>
#include "system/texture.h"

extern NJS_TEXLIST core_eggman_background_textures;
extern NJS_TEXLIST **core_eggman_bg_groups[];
extern struct texture_pvm core_eggman_bg_pvms[];

#endif
