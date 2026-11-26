/* Eternal Engine's reactor: its texture lists, models, fragments and
 * collision objects, and its panels' places */
#ifndef STG24_REACTOR_ART_H
#define STG24_REACTOR_ART_H

#include <ninja.h>

#include "draw/texture_anim.h"

extern NJS_TEXLIST reactor_textures;
extern NJS_TEXLIST reactor_lid_textures;
extern NJS_TEXLIST reactor_rim_textures;
extern NJS_TEXLIST reactor_smoke_textures;
extern NJS_TEXLIST reactor_panel_bright;
extern NJS_TEXLIST reactor_panel_dim;
extern NJS_TEXLIST reactor_fragment_1_texlist;
extern NJS_TEXLIST reactor_fragment_2_texlist;
extern NJS_TEXLIST reactor_fragment_3_texlist;
extern NJS_TEXLIST reactor_fragment_4_texlist;
extern NJS_TEXLIST reactor_fragment_5_texlist;
extern NJS_TEXLIST reactor_fragment_6_texlist;
extern NJS_TEXLIST reactor_fragment_7_texlist;
extern NJS_TEXLIST reactor_fragment_8_texlist;

extern NJS_CNK_OBJECT reactor_fragment_1;
extern NJS_CNK_OBJECT reactor_fragment_2;
extern NJS_CNK_OBJECT reactor_fragment_3;
extern NJS_CNK_OBJECT reactor_fragment_4;
extern NJS_CNK_OBJECT reactor_fragment_5;
extern NJS_CNK_OBJECT reactor_fragment_6;
extern NJS_CNK_OBJECT reactor_fragment_7;
extern NJS_CNK_OBJECT reactor_fragment_8;

extern NJS_TEXLIST *reactor_fragment_textures[8];
extern NJS_CNK_OBJECT *reactor_fragments[8];
extern NJS_POINT3 reactor_panel_positions[9];

extern NJS_CNK_OBJECT reactor_left_gun[1];
extern NJS_CNK_OBJECT reactor_left_support[1];
extern NJS_CNK_OBJECT reactor_first_coil[1];
extern NJS_CNK_OBJECT reactor_second_coil[1];
extern NJS_CNK_OBJECT reactor_right_gun[1];
extern NJS_CNK_OBJECT reactor_right_support[1];
extern NJS_CNK_OBJECT reactor_body[1];
extern NJS_CNK_OBJECT reactor_lid_model[1];
extern NJS_CNK_OBJECT reactor_rim[1];
extern NJS_CNK_OBJECT reactor_panel[1];
extern NJS_OBJECT reactor_body_collision[1];
extern NJS_OBJECT reactor_lid_collision[1];
extern struct texture_anim reactor_rim_anim[1];

#endif
