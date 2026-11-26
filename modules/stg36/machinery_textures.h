/* the background's PVM table and texture lists. texture_load_shared fills the
 * machinery's lists from the background's PVM */
#ifndef STG36_MACHINERY_TEXTURES_H
#define STG36_MACHINERY_TEXTURES_H
#include <ninja.h>
#include "system/texture.h"

extern const char tails_folded_pipe_texture_name[];

extern NJS_TEXLIST core_tails_background_textures;
extern NJS_TEXLIST **core_tails_bg_groups[];
extern struct texture_pvm core_tails_bg_pvms[];
extern const char tails_rotor_blade_name[16];
extern const char tails_rotor_hub_name[16];

#endif
