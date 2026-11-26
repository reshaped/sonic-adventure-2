/* Three writable texture lists bind the same three stage-specific names. */
#ifndef SHARED_STORE_TEXTURES_H
#define SHARED_STORE_TEXTURES_H
#include <ninja.h>

extern const char store_box_shadow_name[20];
extern const char store_box_side_name[20];
extern const char store_box_lid_name[22];

extern NJS_TEXLIST store_box_blue_texlist;
extern NJS_TEXLIST store_box_green_texlist;
extern NJS_TEXLIST store_box_red_texlist;

#endif
