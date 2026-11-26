/* the arrows' texture lists, and the two texture names that the red, green
 * and blue lists share. arrow_red.c defines the names */
#ifndef STG26_ARROW_TEXTURES_H
#define STG26_ARROW_TEXTURES_H
#include <ninja.h>

extern const char arrow_light_texture[12];
extern const char arrow_base_texture[15];
extern NJS_TEXLIST arrow_red_texlist;
extern NJS_TEXLIST arrow_green_texlist;
extern NJS_TEXLIST arrow_blue_texlist;

#endif
